/*
 * File:   main.c
 * Author: Johann
 *
 * Created on 30 de Maio de 2021, 16:52
 * Last update on July 7th of 2021 at 15:43
 */

/*Pro Jeff ficar feliz, aqui j�s um coment�rio :)*/

/*Bibliotecas e frequ�ncia do uc*/
#define F_CPU 16000000        //define a frequencia do uC para 16MHz
#include <avr/io.h>           //Biblioteca geral dos AVR
#include <avr/interrupt.h>    //Biblioteca de interrup��o
#include <stdio.h>            //Bilioteca do C
#include <util/delay.h>       //Biblioteca geradora de atraso
#include "UART.h"             //Biblioteca da comunica��o UART
#include "ADC.h"              //Biblioteca do conversor AD
#include "PWM_10_bits.h"      //Biblioteca de PWM fast mode de 10 bits
#include "Driver_motor.h"     //Biblioteca das fun��es de controle dos motores
#include "PID.h"              //Biblioteca do controle PID
/*============================================================*/

//vari�veis de comando para os registradores
#define set_bit(y,bit) (y|=(1<<bit)) //coloca em 1 o bit x da vari�vel Y
#define clr_bit(y,bit) (y&=~(1<<bit)) //coloca em 0 o bit x da vari�vel Y
#define cpl_bit(y,bit) (y^=(1<<bit)) //troca o estado l�gico do bit x da vari�vel Y
#define tst_bit(y,bit) (y&(1<<bit)) //retorna 0 ou 1 conforme leitura do bit
/*==============================================================*/

/*Mapeamento de Hardware*/
#define sensor_de_curva   PB0
#define sensor_de_parada  PD7
#define led               PB5
#define leitura_curva    PINB
#define leitura_parada   PIND

/*==============================================================*/

/*Vari�veis globais*/
char erro = 0; //�rea PID
int PWMA = 0, PWMB = 0; // Modula��o de largura de pulso enviada pelo PID
//int *ptr = NULL;                                        //ponteiro utilizado para receber os valores dos sensores frontais
unsigned char sensores_frontais[6];

//Vari�veis globais da calibra��o de sensores
unsigned char valor_max[6] = {0, 0, 0, 0, 0, 0};
unsigned char valor_min[6] = {255, 255, 255, 255, 255, 255};
unsigned char valor_max_abs = 255;
unsigned char valor_min_abs = 0;
//unsigned char valor_max_abs = 255;    //colocar assim quando testar no rob�, n�o fica pr�tico simular desta forma
//unsigned char valor_min_abs = 0;

//Vari�veis globais do timer0
unsigned char counter1 = 0, counter2 = 0;

//Vari�veis globais da UART
char buffer[5]; //String que armazena valores de entrada para serem printadas
volatile char ch; //armazena o caractere lido
volatile char flag_com = 0; //flag que indica se houve recep��o de dado
// Interrup��o da UART

/*======================================================*/

/*Vari�veis usadas no controle do loop*/
//char flag0 = 1, flag1 = 0;

/*tempo =65536 * Prescaler/Fosc = 65536 * 1024/16000000 = 4, 19s
 tempo = X_bit_timer * Prescaler/Fosc
 Valor inicial de contagem = 256 - tempo_desejado*Fosc/Prescaler = 256 - 0,001*16000000/1024 = 255
 Valor inicial de contagem = X_bit_timer - tempo_desejado*Fosc/Prescaler */
/*===========================================================================*/

/*Prot�tipo das fun��es*/
void ADC_maq();
void INT_INIT();
//void entrou_na_curva(char valor_erro);
void parada();
void calibra_sensores();
void seta_calibracao();
void sensores();
void setup();
void setup_Hardware();
void setup_logica();
void loop();
void sentido_de_giro();
void PWM_limit();
void correcao_do_PWM();
void estrategia();
/*===========================================================================*/


/*Interrup��es*/
ISR(USART_RX_vect) {
    ch = UDR0; //Faz a leitura do buffer da serial

    UART_enviaCaractere(ch); //Envia o caractere lido para o computador
    //flag_com = 1; //Aciona o flag de comunica��o
}

ISR(TIMER0_OVF_vect) 
{
    TCNT0 = 56; //Recarrega o Timer 0 para que a contagem seja 100us novamente
    
    estrategia();
    
}//end TIMER_0

ISR(ADC_vect)
{
    ADC_maq();
}

/*============================================================================*/


/*Fun��o principal*/
int main(void) 
{
    setup();

    while (1) loop();
    return 0;
}//end main

//===Fun��es n�o vis�veis ao usu�rio======//

void setup() {

    setup_Hardware();
    ADC_init();
    sei(); //Habilita as interrup��es
    setup_logica();
    INT_INIT();    //inicializo interrup��o do TIMER0 ap�s a calibra��o

}


void setup_Hardware(){
    MCUCR &= 0xef;      //habilita pull up quando configurado e desabilita algumas configura��es pr�vias do MCU

    DDRD = 0b01111010; //PD3 - PD6 definidos como sa�da, PD7 como entrada, PD0 como entrada(RX) e PD1 como sa�da(TX)
    PORTD = 0b10000000; //inicializados em n�vel baixo e PD7 com pull up
    DDRB = 0b00100110; //Habilita PB0 como entrada e PB5, PB1 e PB2 como sa�da
    PORTB = 0b00000001; //PORTB inicializa desligado e pull up no PB0
    DDRC = 0b00000000; //PORTC como entrada
    PORTC = 0b00001111; //PC3 - PC0 com pull up (colocar resistor de pull up nos pinos A6 e A7)

    //esquerdo pino 4 - PD2
    UART_config(16); //Inicializa a comunica��o UART com 57.6kbps
    
    TCCR1A = 0xA2; //Configura opera��o em fast PWM, utilizando registradores OCR1x para compara��o

    setFreq(1); //Seleciona op��o para frequ�ncia
    //16kHz de PWM
}

void setup_logica(){
    //----> Calibra��o dos Sensores frontais <----//
    set_bit(PORTB, led); //subrotina de acender e apagar o LED 13
    ADC_maq();
    calibra_sensores(); //calibra��o dos sensores //A calibra��o vai conseguir acompanhar o AD
                                                  //ou pode ser que o vetor n�o seja preenchido a tempo?
                                                  //� necess�rio colocar um contador
                                                  //para depois chamar a fun��o de calibra��o?
    
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
    
    
}

void INT_INIT()
{
        
    TCCR0B = 0b00000010; //TC0 com prescaler de 8
    TCNT0 = 56; //Inicia a contagem em 56 para, no final, gerar 100us
    TIMSK0 = 0b00000001; //habilita a interrup��o do TC0
}

void loop()//loop vazio
{
    /*unsigned char u_curva = 0;
    static unsigned int PWMA_C = 0, PWMB_C = 0; //PWM de curva com ajuste do PID;
    static unsigned int PWM_Curva = 350; //PWM ao entrar na curva
    if(flag0)
    {
        correcao_do_PWM();
        flag0 = 0x00;
    }
    
    if(flag1)
    {
       u_curva = PID(erro);
       PWMA_C = PWM_Curva - u_curva;
       PWMB_C = PWM_Curva + u_curva;
       frente();
       setDuty_1(PWMA_C);
       setDuty_2(PWMB_C);
       flag1 = 0x00;
    }*/
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

//=========Fun��es vis�veis ao usu�rio===========//
//Fun��o s� �til ap�s o mapeamneto
/*void entrou_na_curva(int valor_erro) {
    int u_curva = 0;
    static unsigned int PWMA_C = 0, PWMB_C = 0, entrou = 0; //PWM de curva com ajuste do PID;
    static unsigned int PWM_Curva = 350; //PWM ao entrar na curva

    if ((!tst_bit(leitura_curva, sensor_de_curva)) && tst_bit(leitura_parada, sensor_de_parada))
        //li branco no sensor de curva e li preto no sensor de parada
    {
        switch (entrou) {
            case 0: //entrou na curva
                u_curva = PID(valor_erro);
                PWMA_C = PWM_Curva - u_curva;
                PWMB_C = PWM_Curva + u_curva;
                frente();
                setDuty_1(PWMA_C);
                setDuty_2(PWMB_C);
                entrou = 1;
                break;

            case 1:
                entrou = 0;
                frente();
                setDuty_1(PWMA); //t�mino da curva
                setDuty_2(PWMB);
                clr_bit(PORTB, led);
                break;
        }
    }
}*/

void parada() 
{

    static char contador = 0, numcurva = 10; // Borda   //contador - n�mero de marcadores de curva;
    static char parada = 0;

    if ((!tst_bit(leitura_curva, sensor_de_curva)) && tst_bit(leitura_parada, sensor_de_parada)) 
    {
        contador++;
        //entrou_na_curva(value_erro); // Verifica se � uma curva
    } 
    else if ((!tst_bit(leitura_curva, sensor_de_curva)) && (!tst_bit(leitura_parada, sensor_de_parada))) //verifica se � crizamento
    {
        frente();
        setDuty_1(PWMA);
        setDuty_2(PWMB);
    }
    
    else if ((tst_bit(leitura_curva, sensor_de_curva)) && (!tst_bit(leitura_parada, sensor_de_parada)))  parada++;

    //leu o n�mero total de marca��es e leu as duas marca��es de largada e chegada
    while (contador == numcurva && parada == 2)
    {
        freio();
    }
}

void calibra_sensores() 
{
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
    //Este � o algoritmo a ser usado no rob�. Desmcomente antes de compilar e comente o outro.
    for (int i = 0; i < 6; i++) {
        if (valor_min [i] > valor_min_abs && valor_min[i] !=0 ) //esse !0 foi colocado pois estava havendo um bug ao simular
        {
            valor_min_abs = valor_min [i];
        } 
        
        if (valor_max [i] < valor_max_abs) {
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


void sentido_de_giro()
{
    //-----> �rea do senstido de giro
    unsigned char u_curva = 0;
    static unsigned int PWMA_C = 0, PWMB_C = 0; //PWM de curva com ajuste do PID;
    static unsigned int PWM_Curva = 350; //PWM ao entrar na curva

    if ((sensores_frontais[0] < 200 && sensores_frontais[5] > 900) || (sensores_frontais[0]  > 900 && sensores_frontais[5] < 200))    
        //se o primeiro sensor ou o �ltimo sensor estiverem lendo branco...
        //necess�rio teste com monitor serial
        //estudar a melhor quantidade de sensores e seu espa�amento
    {
        //colocar  PID no loop usando flags
        u_curva = PID(erro);
        PWMA_C = PWM_Curva - u_curva;
        PWMB_C = PWM_Curva + u_curva;
        frente();
        setDuty_1(PWMA_C);
        setDuty_2(PWMB_C);
        //flag1 = 1;
    } //em cima da linha
        
    else
    { //pra frente - reta
        //flag1 = 0x00;
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

void correcao_do_PWM() {

    unsigned int soma_direito = 0, denominador_direito = 6, denominador_esquerdo = 6;
    int soma_esquerdo = 0;
    unsigned char soma_total = 0;   //caso aumente o peso da m�dia_ponderada, tomar cuidado com a vari�vel char
    static unsigned int PWMR = 400; // valor da for�a do motor em linha reta
    unsigned char u = 0; //valor de retorno do PID
    
    static char peso [] = {-3, -2, -1, 1, 2, 3}; //utilizando um prescale de 2000
    //os pesos precisar�o ser corrigidos pois os sensores do Van Grogue est�o um pouco assim�tricos
    
    for (int j = 0; j < 3; j++) 
    {
        soma_esquerdo += (sensores_frontais[j] * peso[j]);
        soma_direito += (sensores_frontais[5-j] * peso[5 - j]);
    }

    soma_total = (soma_esquerdo + soma_direito) / (denominador_esquerdo + denominador_direito);

    erro = 0 - soma_total;   //valor esperado(estar sempre em cima da linha) - valor medido

    /*sprintf(buffer, "Erro = %5d\n", erro); //Converte para string
    UART_enviaString(buffer); //Envia para o computador
    UART_enviaHex(erro);
    UART_enviaCaractere(0x0D); //pula linha*/   /*usada somente para mapear*/

    //--------------->AREA DO PID<---------------

    u = PID(erro);

    PWMA = PWMR - u;
    PWMB = PWMR + u;

    frente();
    setDuty_1(PWMA);
    setDuty_2(PWMB);
    
}//fim do programa

void estrategia()
{
    counter1++; //incrementa a cada 100us
    parada();   //verifica se � parada a cada 100us
    if(counter1 == 5)   //em 500us
    {
        sensores();         //seta o limiar da leitura dos sensores
        //flag0 = 1;
        correcao_do_PWM();  //Corrige o PWM atrav�s do PID
        PWM_limit();        //Verifica se houve estouro no PWM por parte do PID
        sentido_de_giro();  //Verifica se precisa fazer uma curva
        counter1 = 0x00;
    }   
}
