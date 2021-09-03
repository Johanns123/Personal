/*
 * File:   main.c
 * Author: Johann
 *
 * Created on 30 de Maio de 2021, 16:52
 * Last update on July 7th of 2021 at 15:43
 */

/*Bibliotecas e frequ�ncia do uc*/
#define F_CPU 16000000                                  //define a frequencia do uC para 16MHz
#include <avr/io.h>                                     //Biblioteca geral dos AVR
#include <avr/interrupt.h>                              //Biblioteca de interrup��o
#include <stdio.h>                                      //Bilioteca do C
#include <math.h>                                       //Biblioteca para c�lculo do m�dulo
#include <util/delay.h>                                 //Biblioteca geradora de atraso
#include "UART.h"                                       //Biblioteca da comunica��o UART
#include "ADC.h"                                        //Biblioteca do conversor AD
#include "PWM_10_bits.h"                                //Biblioteca de PWM fast mode de 10 bits
#include "Driver_motor.h"                               //Biblioteca das fun��es de controle dos motores
#include "PID.h"                                        //Biblioteca do controle PID
//#include "configbits.txt"                             //configura os fus�veis
/*============================================================*/


//macros de comando para os registradores
#define set_bit(y,bit) (y|=(1<<bit))                    //coloca em 1 o bit x da vari�vel Y
#define clr_bit(y,bit) (y&=~(1<<bit))                   //coloca em 0 o bit x da vari�vel Y
#define cpl_bit(y,bit) (y^=(1<<bit))                    //troca o estado l�gico do bit x da vari�vel Y
#define tst_bit(y,bit) (y&(1<<bit))                     //retorna 0 ou 1 conforme leitura do bit
/*==============================================================*/

/*Mapeamento de Hardware*/
#define sensor_de_curva   PB0
#define sensor_de_parada  PD7
#define led               PB5
#define leitura_curva    PINB
#define leitura_parada   PIND

/*Por falta de porta estas n�o s�o as macros definitivas dos encoders*/
#define    encoder_C1D   PD2                            //Conex�o C1 do encoder Direito
#define    encoder_C2D   PD4                            //Conex�o C2 do encoder

#define    encoder_C1E   PD3                            //Conex�o C1 do encoder Esquerdo
#define    encoder_C2E   PD5                            //Conex�o C2 do encoder

/*==============================================================*/

/*Vari�veis globais*/
char erro = 0; //Vari�vel utilizada no controle PID
int PWMA = 0, PWMB = 0; // Modula��o de largura de pulso enviada pelo PID
unsigned char curva1 = 0, curva2;
char flag = 0;
unsigned char pulse_numberD = 0, pulse_numberE = 0; //vari�veis para contagem dos pulsos dos encoders
//int *ptr = NULL; //ponteiro utilizado para receber os valores dos sensores frontais
unsigned char sensores_frontais[6];

//Vari�veis globais da calibra��o de sensores
unsigned char valor_max [] = {255, 255, 255, 255, 255, 255}; //vari�veis usadas na calibra��o do sensores
unsigned char valor_min [] = {0, 0, 0, 0, 0, 0};
unsigned char valor_min_abs = 255, valor_max_abs = 0;
//unsigned char valor_max_abs = 255;    //colocar assim quando testar no rob�, n�o fica pr�tico simular desta forma
//unsigned char valor_min_abs = 0;

unsigned int counter = 0;

//Vari�veis globais do timer0


//Vari�veis globais da UART
char buffer[5]; //String que armazena valores de entrada para serem printadas
volatile char ch; //armazena o caractere lido
volatile char flag_com = 0; //flag que indica se houve recep��o de dado
// Interrup��o da UART


/*======================================================*/


/*tempo =65536 * Prescaler/Fosc = 65536 * 1024/16000000 = 4, 19s
 tempo = X_bit_timer * Prescaler/Fosc
 Valor inicial de contagem = 256 - tempo_desejado*Fosc/Prescaler = 256 - 0,001*16000000/1024 = 255
 Valor inicial de contagem = X_bit_timer - tempo_desejado*Fosc/Prescaler */
/*===========================================================================*/

/*Prot�tipo das fun��es*/
void ADC_maq();
void INT_INIT();
int calculo_do_raio();
int PID_encoderE(int duty);
void count_pulsesD();
void count_pulsesE();
int PID_encoderD(int duty);
void tomada_de_tempo();
void estrategia();
void mapeamento();
void coleta_de_dados();
void tomada_de_tempo();
void entrou_na_curva();
void parada();
void calibra_sensores();
void seta_calibracao();
void sensores();
void setup();
void setup_Hardware();
void setup_logica();
void loop();
void sentido_de_giro(int PWM_Curva, int error);
void PWM_limit();
void correcao_do_PWM(int PWMR);
/*===========================================================================*/

/*Interrup��es*/
ISR(USART_RX_vect) {
    ch = UDR0; //Faz a leitura do buffer da serial

    UART_enviaCaractere(ch); //Envia o caractere lido para o computador
    //flag_com = 1; //Aciona o flag de comunica��o
}

ISR(TIMER0_OVF_vect) {
    TCNT0 = 56; //Recarrega o Timer 0 para que a contagem seja 1ms novamente
    estrategia();
}//end TIMER_0

ISR(INT0_vect) {

    count_pulsesD(); //conta os pulsos do encoder_direito

}//end INT0

ISR(INT1_vect) {
    count_pulsesE(); //conta os pulsos do encoder_esquerdo

}//end INT1

ISR(ADC_vect)
{
    ADC_maq();
}
/*============================================================================*/

/*Fun��o principal*/
int main(void) {
    setup();

    while (1) loop();
}//end main

//===Fun��es n�o vis�veis ao usu�rio======//

void setup() {

    setup_Hardware();
    ADC_init();
    sei(); //Habilita as interrup��es
    setup_logica();
    INT_INIT();    //inicializo interrup��o do TIMER0 ap�s a calibra��o

}//end setup

void setup_Hardware() {
    DDRD = 0b01111010; //PD3 - PD6 definidos como sa�da, PD7 como entrada
    PORTD = 0b10000000; //inicializados em n�vel baixo e PD7 com pull up
    DDRB = 0b00100110; //Habilita PB0 como entrada e PB5, PB1 e PB2 como sa�da
    PORTB = 0b00000001; //PORTB inicializa desligado e pull up no PB0
    DDRC = 0b00000000; //PORTC como entrada
    PORTC = 0b00001111; //PC3 - PC0 com pull up (colocar resistor de pull up nos pinos A6 e A7)


    TCCR1A = 0xA2; //Configura opera��o em fast PWM, utilizando registradores OCR1x para compara��o

    setFreq(4); //Seleciona op��o para frequ�ncia

}//end setup_hardware

void setup_logica() {
    //----> Calibra��o dos Sensores frontais <----//
    set_bit(PORTB, led); //subrotina de acender e apagar o LED 13
    ADC_maq();
    calibra_sensores(); //calibra��o dos sensores
    seta_calibracao(); //estabelece o limiar dos sensores atrav�s dos valores da fun��o de cima
    sensores(); //determina o limiar dos sensores e printa seus valores na tela

    clr_bit(PORTB, led);
    _delay_ms(500);
    set_bit(PORTB, led); //subrotina de acender e apagar o LED 13
    _delay_ms(1000);
    clr_bit(PORTB, led);
    _delay_ms(500);
    set_bit(PORTB, led);
    _delay_ms(500);
    clr_bit(PORTB, led);
    _delay_ms(2000);


    estrategia(); //fun��o que define qual estrat�gia ser� usada


}//end setup_logica

void INT_INIT()
{
    TCCR0B = 0b00000010; //TC0 com prescaler de 8
    TCNT0 = 56; //Inicia a contagem em 100 para, no final, gerar 100us
    TIMSK0 = 0b00000001; //habilita a interrup��o do TC0

    EICRA = 0x05; //qualquer mudan�a de estado nos pinos INT0 e INT1
    EIMSK = 0x03; //habilita INT0 e INT1
}

void ADC_maq () {
    
    static unsigned char estado = 10;
    
    switch (estado) {
        
        case 0:
            estado = 1;
            sensores_frontais[0] = ADC_ler();
            ADC_conv_ch(2);
            break;
            
        case 1:
            estado = 2;
            sensores_frontais[1] = ADC_ler();
            ADC_conv_ch(1);
            break;
            
        case 2:
            estado = 3;
            sensores_frontais[2] = ADC_ler();
            ADC_conv_ch(0);
            break;
            
        case 3:
            estado = 4;
            sensores_frontais[3] = ADC_ler();
            ADC_conv_ch(7);
            break;
            
        case 4:
            estado = 5;
            sensores_frontais[4] = ADC_ler();
            ADC_conv_ch(6);
            break;
            
        case 5:
            estado = 6;
            sensores_frontais[5] = ADC_ler();
            ADC_conv_ch(3);
            break;
            
        default:
            estado = 0;
            ADC_conv_ch(3);
            sensores_frontais[0] = ADC_ler();
            break; 
    }
    
}


void loop()//loop vazio
{

}//end loop

void estrategia() {
    /*Configura��o das estrat�gias*/
    /*Utiliza��o do PB3 e PB4 como entrada para o dip switch de 2 vias*/

    char sw; //vari�vel do switch
    sw = 0b00000001; //determina��o de 8 bits para uma l�gica AND com o PINx

    sw &= PINB >> 3; //l�gica AND com a leitura dos pinos deslocados tr�s bits � direta

    switch (sw) //sele��o da estrat�gia de acordo com a posi��o das chaves do DIP switch 
    {
        case 0:
            //esquerdo pino 4 - PD2
            UART_config(16); //Inicializa a comunica��o UART com 57.6kbps
            flag = 1; //vari�vel de controle para a fun��o entrou_na_curva
            mapeamento(); //estrategia de mapeamento
            break;

        case 1:
            //esquerdo pino 4 - PD2
            tomada_de_tempo(); //estrat�gia de tomada de tempo
            break;

    }


}

void mapeamento() {
    static unsigned int PWMR = 400; // valor da for�a do motor em linha reta
    static unsigned int PWM_Curva = 350; //PWM ao entrar na curva
    counter++;
    parada();   //verifica se � parada a cada 100us
    if(counter == 5)    //em 500us...
    {
        sensores(); //faz a leitura dos sensores e se estiverem com valores fora do limiar, a corre��o ser� feita.

        correcao_do_PWM(PWMR);
        sentido_de_giro(PWM_Curva, erro); //Verifica qual o sentido da curva
        counter = 0;
    }
    

}


void tomada_de_tempo() {

    static unsigned int PWMR = 800; //valor da for�a do motor em linha reta
    static unsigned int PWM_Curva = 700; //PWM ao entrar na curva
    counter++;
    
    if(counter == 2)
    {
        sensores(); //faz a leitura dos sensores e se estiverem com valores fora do limiar, a corre��o ser� feita.

        //correcao_do_PWM(PWMR);                          //controle PID
        //PWM_limit();                                    //Muda o valor do PWM caso o PID gere um valor acima de 8 bits no final


        correcao_do_PWM(PWMR); //controle PID
        PWM_limit(); //Muda o valor do PWM caso o PID gere um valor acima de 8 bits no final

        sentido_de_giro(PWM_Curva, erro); //Verifica qual o sentido da curva
        counter = 0;
    }


}

/*int PID_encoderD(int duty) {
    int RPM_ideal = duty * 3; //PWMR para PID reta e PWMC para PID_curva
    int RPM_encoder = pulse_numberD * 150; //RPM medido pelo encoder

    int erro = RPM_ideal - RPM_encoder; //erro dado pelo ideal - o medido

    static unsigned int Kp = 2, Kd = 0, Ki = 0; //constantes do PID
    static unsigned int prescale = 2048; //prescale na pot�ncia de 2: 2^n
    static int integral = 0, erroAnterior = 0;
    int p = 0, i = 0, d = 0, Turn = 0;

    p = (erro * Kp); // Proporcao

    integral += erro; // Integral
    i = (Ki * integral);

    d = (Kd * (erro - erroAnterior)); // Derivada
    erroAnterior = erro;

    Turn = (p + i + d) / prescale;

    return Turn; //Retorna o c�lculo do PID

}

int PID_encoderE(int duty) {
    int RPM_ideal = duty * 3; //PWMR para PID reta e PWMC para PID_curva
    int RPM_encoder = pulse_numberE * 150; //RPM medido pelo encoder

    int erro = RPM_ideal - RPM_encoder; //erro dado pelo ideal - o medido

    static unsigned int Kp = 2, Kd = 0, Ki = 0;
    static unsigned int prescale = 2048; //prescale na pot�ncia de 2: 2^n
    static int integral = 0, erroAnterior = 0;
    int p = 0, i = 0, d = 0, Turn = 0;

    p = (erro * Kp); //Proporcao

    integral += erro; //Integral
    i = (Ki * integral);

    d = (Kd * (erro - erroAnterior)); //Derivada
    erroAnterior = erro;

    Turn = (p + i + d) / prescale;

    return Turn;


}*/

void count_pulsesD() {
    static int Encoder_C1Last = 0, direction_m;

    int Lstate = tst_bit(PIND, encoder_C1D); //vari�vel de leitura de um dos pinos do encoderD

    if (!Encoder_C1Last && Lstate) { //Verifica se Encoder_C1Last � falso e Lstate � verdadeiro
        int val = tst_bit(PIND, encoder_C2D); //Vari�vel de leitura do segundo pino do encoderD

        if (!val && direction_m) direction_m = 0; //sentido hor�rio

        else if (val && !direction_m) direction_m = 1; //sentido anti-hor�rio
    }

    Encoder_C1Last = Lstate;

    if (!direction_m) pulse_numberD++; //sentido hor�rio
    else pulse_numberD--;



}

void count_pulsesE() {
    static int Encoder_C1Last = 0, direction_m;

    int Lstate = tst_bit(PIND, encoder_C1E); //vari�vel de leitura de um dos pinos do encoderD

    if (!Encoder_C1Last && Lstate) { //Verifica se Encoder_C1Last � falso e Lstate � verdadeiro
        int val = tst_bit(PIND, encoder_C2E); //Vari�vel de leitura do segundo pino do encoderD

        if (!val && direction_m) direction_m = 0; //sentido hor�rio

        else if (val && !direction_m) direction_m = 1; //sentido anti-hor�rio
    }

    Encoder_C1Last = Lstate;

    if (!direction_m) pulse_numberD++; //sentido hor�rio
    else pulse_numberD--; //sentido anti-hor�rio


}

/*int calculo_do_raio() //esta fun��o calcula o raio a partir da dis�ncia percorrida pelas duas rodas do rob�
{
    static unsigned int raio = 0;
    unsigned int diametro = 126; //126mm, di�metro sas rodas
    static unsigned int modulo = 0;

    if (!(curva1) && !(curva2)); //n�o calcula o raio se n�o mediu a curva

    else if (curva1 != curva2) {
        modulo = (diametro / 2) * ((curva1 + curva2) / (curva1 - curva2)); //C�lculo do raio em m�dulo
        raio = fabs(modulo); //calcula o m�dulo do raio
        sprintf(buffer, "Raio %5d\n", raio); //Converte para string
        UART_enviaString(buffer); //Envia para o computador
        UART_enviaCaractere(0x0D); //pula linha
    }

    return raio;
}*/

//=========Fun��es vis�veis ao usu�rio===========//
//reavaliar esta fun��o devido � pista possuir um "S" e somente tr�s marcadores de curva

void entrou_na_curva() {
    static unsigned int dist_reta1 = 0, dist_curva1 = 0, dist_reta2 = 0, dist_curva2 = 0; //vari�veis para c�lculo do raio
    static unsigned int entrou = 0;

    if ((!tst_bit(leitura_curva, sensor_de_curva)) && tst_bit(leitura_parada, sensor_de_parada))
        //li branco no sensor de curva e li preto no sensor de parada
    {
        switch (entrou) {
            case 0: //entrou na curva
                if (flag) //se for rotina de mapeamento ocorre:
                {
                    dist_reta1 = pulse_numberD * 0.812;
                    dist_reta2 = pulse_numberE * 0.812;
                    if (!dist_reta1); //nao printa;
                    else {
                        sprintf(buffer, "RetaD %5d\n", dist_reta1); //Converte para string
                        UART_enviaString(buffer); //Envia para o computador
                        UART_enviaCaractere(0x0D); //pula linha
                    } //printa na serial a distancia;

                    if (!dist_reta2); //nao printa;
                    else { //printa na serial a distancia;
                        sprintf(buffer, "RetaE %5d\n", dist_reta2); //Converte para string
                        UART_enviaString(buffer); //Envia para o computador
                        UART_enviaCaractere(0x0D); //pula linha
                    }
                    pulse_numberD = 0x00;
                    dist_reta1 = 0x00;
                    pulse_numberE = 0x00;
                    dist_reta2 = 0x00; //zera as vari�veis de c�lculo da dist�ncia
                }
                entrou = 1;
                break;

            case 1:
                if (flag) {
                    sensores();
                    if ((sensores_frontais[0] < 200 && sensores_frontais[5] > 900) || (sensores_frontais[0] > 900 && sensores_frontais[5] < 200))
                    {//se o primeiro sensor ou o �ltimo sensor estiverem lendo branco...
                        dist_curva1 = pulse_numberD * 0.812; //converte o n�mero de pulsos em mm
                        dist_curva2 = pulse_numberE * 0.812;
                        if (!dist_curva1); //nao printa;
                        else 
                        {
                            sprintf(buffer, "CurvaD %5d\n", dist_curva1); //Converte para string
                            UART_enviaString(buffer); //Envia para o computador
                            UART_enviaCaractere(0x0D); //pula linha
                        } //printa na serial a distancia;
                        if (!dist_curva2); //nao printa;
                        else 
                        {
                            sprintf(buffer, "CurvaE %5d\n", dist_curva2); //Converte para string
                            UART_enviaString(buffer); //Envia para o computador
                            UART_enviaCaractere(0x0D); //pula linha
                        } //printa na serial a distancia;
                        curva1 = dist_curva1;
                        curva2 = dist_curva2;
                        calculo_do_raio();
                        dist_curva1 = 0x00;
                        pulse_numberD = 0x00;
                        dist_curva2 = 0x00;
                        pulse_numberE = 0x00; //zera as vari�veis de c�lculo do raio da curva

                    }
                    else{
                        dist_curva1 = pulse_numberD * 0.812; //converte o n�mero de pulsos em mm
                        dist_curva2 = pulse_numberE * 0.812;
                        if (!dist_curva1); //nao printa;
                        else {
                            sprintf(buffer, "CurvaD %5d\n", dist_curva1); //Converte para string
                            UART_enviaString(buffer); //Envia para o computador
                            UART_enviaCaractere(0x0D); //pula linha
                        } //printa na serial a distancia;
                        if (!dist_curva2); //nao printa;
                        else {
                            sprintf(buffer, "CurvaE %5d\n", dist_curva2); //Converte para string
                            UART_enviaString(buffer); //Envia para o computador
                            UART_enviaCaractere(0x0D); //pula linha
                        } //printa na serial a distancia;
                        curva1 = dist_curva1;
                        curva2 = dist_curva2;
                        calculo_do_raio();
                        dist_curva1 = 0x00;
                        pulse_numberD = 0x00;
                        dist_curva2 = 0x00;
                        pulse_numberE = 0x00; //zera as vari�veis de c�lculo do raio da curva
                        entrou = 0;
                        frente();
                        setDuty_1(PWMA); //t�mino da curva
                        setDuty_2(PWMB);
                    }

                break;
            }
        }
    }
}

void parada() {

    static char contador = 0, numParada = 4; // Borda   //contador - n�mero de marcadores de curva;
    static char parada = 0;
    if ((!tst_bit(leitura_curva, sensor_de_curva)) && tst_bit(leitura_parada, sensor_de_parada)) {
        contador++;
    }
    else if ((!tst_bit(leitura_curva, sensor_de_curva)) && (!tst_bit(leitura_parada, sensor_de_parada))) //verifica se � crizamento
    {
        frente();
        setDuty_1(PWMA);
        setDuty_2(PWMB);
    }

    else if ((tst_bit(leitura_curva, sensor_de_curva)) && (!tst_bit(leitura_parada, sensor_de_parada)))  parada++;

    //leu o n�mero total de marca��es e leu as duas marca��es de largada e chegada
    while (contador == numParada && parada == 2)
    {
        freio();
    }
}

void calibra_sensores() {
    //=====Fun��o que inicializa a calibra��o====//
    for (int i = 0; i < 120; i++) {
        for (int i = 0; i < 6; i++) {
            if (sensores_frontais[i] < valor_min [i]) {
                valor_min[i] = sensores_frontais[i];
            }
            if (sensores_frontais[i] > valor_max [i]) {
                valor_max[i] = sensores_frontais[i];
            }
        }

        _delay_ms(10);  //tempo o suficiente para o pessoa calibrar os sensores mecanicamente
        
        /*
        Ap�s isso determinar o limiar de todos os sensores para que eles tenham os mesmos valores do AD. 
        Para que todos tenham um limite inferior e superior igual.
         */
    }

}

void seta_calibracao() {
    //----> Calibra��o dos Sensores frontais <----//

    //fun��o que seta o limiar dos sensores
    for (int i = 0; i < 6; i++) {
        if (valor_min [i] < valor_min_abs && valor_min[i] !=0 ) {
            valor_min_abs = valor_min [i];
        } 
        
        if (valor_max [i] > valor_max_abs) {
            valor_max_abs = valor_max [i];
        }

    }
}

void sensores() {

    //======Estabelece o limiar da leitura dos sensores====//
    //fun��o de corre��o da calibra��o
    for (int i = 0; i < 6; i++) {
        if (sensores_frontais[i] < valor_min_abs) {
            sensores_frontais[i] = valor_min_abs;
        }
        if (sensores_frontais[i] > valor_max_abs) {
            sensores_frontais[i] = valor_max_abs;
        }

    }
}


void sentido_de_giro(int PWM_Curva, int error) {
    //-----> �rea do senstido de giro
    int u_curva = 0, u_encD = 0, u_encE = 0;
    static unsigned int PWMA_C = 0, PWMB_C = 0; //PWM de curva com ajuste do PID;

    if ((sensores_frontais[0] < 200 && sensores_frontais[5] > 900) || (sensores_frontais[0] > 900 && sensores_frontais[5] < 200))
        //se o primeiro sensor ou o �ltimo sensor estiverem lendo branco...
        //necess�rio teste com monitor serial
        //estudar a melhor quantidade de sensores e seu espa�amento
    {
        //u_encE = PID_encoderE(PWM_Curva);
        //u_encD = PID_encoderD(PWM_Curva); //C�lculo dos PIDs
        u_curva = PID(error);
        PWMA_C = PWM_Curva - u_curva;// - u_encD + u_encE; //atribui��o do PID no PWM dos motores
        PWMB_C = PWM_Curva + u_curva;// + u_encD - u_encE;
        frente();
        setDuty_1(PWMA_C);
        setDuty_2(PWMB_C);
        if(flag)    entrou_na_curva();
    }//em cima da linha

    else 
    { //pra frente - reta
        frente();
        setDuty_1(PWMA);
        setDuty_2(PWMB);
    }

    //A fun��o que fazia o rob� rodar em seu pr�prio eixo foi removida

}

void PWM_limit() {
    //------> Limitando PWM

    if (PWMA > 1023) {
        PWMA = 1000;
    }
    else if (PWMB > 1023) {
        PWMB = 1000;
    }
}

void correcao_do_PWM(int PWMR) 
{
    int soma_direito = 0, soma_esquerdo = 0, denominador_direito = 6, denominador_esquerdo = 6, soma_total = 0;
    static int peso [] = {-3, -2, -1, 1, 2, 3}; //utilizando um prescale de 2000
    int u = 0, u_encD = 0, u_encE = 0; //valor de retorno do PID
    //sensores_frontais[6] = {le_ADC(3), le_ADC(2), le_ADC(1), le_ADC(0), le_ADC(7), le_ADC(6)}


    for (int j = 0; j < 3; j++) {
        soma_esquerdo += (sensores_frontais[j] * peso[j]);
        soma_direito += (sensores_frontais[5 - j] * peso[5 - j]);
    }

    soma_total = (soma_esquerdo + soma_direito) / (denominador_esquerdo + denominador_direito);

    //soma_total = sensores();

    erro = 0 - soma_total; //valor esperado(estar sempre em cima da linha) - valor medido

    //--------------->AREA DO PID<---------------

    u = PID(erro);
    //u_encD = PID_encoderD(PWMR);
    //u_encE = PID_encoderE(PWMR);

    PWMA = PWMR - u; //- u_encD + u_encE;
    PWMB = PWMR + u; //+ u_encD - u_encD;

    frente();
    setDuty_1(PWMA);
    setDuty_2(PWMB);
}//fim do programa
