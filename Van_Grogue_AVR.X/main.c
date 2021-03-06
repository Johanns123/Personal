/*
 * File:   main.c
 * Author: Johann
 *
 * Created on 30 de Maio de 2021, 16:52
 * Last update on July 7th of 2021 at 15:43
 */

/*Bibliotecas e frequ�ncia do uc*/
#define F_CPU 16000000      //define a frequencia do uC para 16MHz
#include <avr/io.h>         //Biblioteca geral dos AVR
#include <avr/interrupt.h>  //Biblioteca de interrup��o
#include <stdio.h>          //Bilioteca do C
#include <util/delay.h>     //Biblioteca geradora de atraso
#include "UART.h"           //Biblioteca da comunica��o UART
#include "ADC.h"            //Biblioteca do conversor AD
#include "PWM_10_bits.h"    //Biblioteca de PWM fast mode de 10 bits
#include "Driver_motor.h"   //Biblioteca das fun��es de controle dos motores
#include "PID.h"            //Biblioteca do controle PID
//#include "configbits.txt"   //configura os fus�veis
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
int erro = 0; //�rea PID
int PWMA = 0, PWMB = 0; // Modula��o de largura de pulso enviada pelo PID

//Vari�veis globais da calibra��o de sensores
unsigned int valor_max [] = {1023, 1023, 1023, 1023, 1023, 1023}; //vari�veis usadas na calibra��o do sensores
unsigned int valor_min [] = {0, 0, 0, 0, 0, 0};
unsigned int valor_min_abs = 0, valor_max_abs = 1023;

//Vari�veis globais do timer0
unsigned int millis = 0;
unsigned int counter1 = 0, counter2 = 0;

//Vari�veis globais da UART
char s [] = "In�cio da leitura";
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
void entrou_na_curva(int valor_erro);
void parada(int value_erro);
void calibra_sensores();
void seta_calibracao();
void sensores();
void setup();
void setup_Hardware();
void setup_logica();
void loop();
void area_de_parada();
void sentido_de_giro();
void PWM_limit();
void correcao_do_PWM();
/*===========================================================================*/


/*Interrup��es*/
ISR(USART_RX_vect) {
    ch = UDR0; //Faz a leitura do buffer da serial

    UART_enviaCaractere(ch); //Envia o caractere lido para o computador
    flag_com = 1; //Aciona o flag de comunica��o
}

ISR(TIMER0_OVF_vect) {
    TCNT0 = 240; //Recarrega o Timer 0 para que a contagem seja 1ms novamente
    millis++; //Incrementa a vari�vel millis a cada 1ms
    counter1++; //incrementa a cada 1ms
    counter2++; //increenta a cada 1ms
    sensores(); //faz a leitura dos sensores e se estiverem com valores fora do limiar, a corre��o ser� feita.
    if(counter1 == 5) //tempo de 5ms
    {
        correcao_do_PWM(); //controle PID
        PWM_limit();       //Muda o valor do PWM caso o PID gere um valor acima de 8 bits no final
        counter1 = 0;
    }
    
    if(counter2 == 20)    //chama a cada 20ms
    {
        area_de_parada(); //Verfica se � uma parada ou um cruzamento
        sentido_de_giro(); //Verifica qual o sentido da curva
        counter2 = 0;
    }
}//end TIMER_0

/*============================================================================*/


/*Fun��o principal*/
int main(void) {
    setup();

    while (1) loop();
}//end main

//===Fun��es n�o vis�veis ao usu�rio======//

void setup() {

    setup_Hardware();
    setup_logica();
    sei(); //Habilita as interrup��es

}


void setup_Hardware(){
    DDRD = 0b01111000; //PD3 - PD6 definidos como sa�da, PD7 como entrada
    PORTD = 0b10000000; //inicializados em n�vel baixo e PD7 com pull up
    DDRB = 0b00100110; //Habilita PB0 como entrada e PB5, PB1 e PB2 como sa�da
    PORTB = 0b00000001; //PORTB inicializa desligado e pull up no PB0
    DDRC = 0b00000000; //PORTC como entrada
    PORTC = 0b00001111; //PC3 - PC0 com pull up (colocar resistor de pull up nos pinos A6 e A7)

    //esquerdo pino 4 - PD2
    UART_config(); //Inicializa a comunica��o UART
    inicializa_ADC(); //Configura o ADC
    UART_enviaString(s); //Envia um texto para o computador

    TCCR0B = 0b00000101; //TC0 com prescaler de 1024
    TCNT0 = 240; //Inicia a contagem em 100 para, no final, gerar 1ms
    TIMSK0 = 0b00000001; //habilita a interrup��o do TC0
    
    TCCR1A = 0xA2; //Configura opera��o em fast PWM, utilizando registradores OCR1x para compara��o

    setFreq(4); //Seleciona op��o para frequ�ncia

}

void setup_logica(){
    //----> Calibra��o dos Sensores frontais <----//
    set_bit(PORTB, led); //subrotina de acender e apagar o LED 13
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
    
    
}


void loop()//loop vazio
{

}

//=========Fun��es vis�veis ao usu�rio===========//

void entrou_na_curva(int valor_erro) {
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
}


void parada(int value_erro) {

    static char contador = 0, numParada = 4; // Borda   //contador - n�mero de marcadores de curva;

    if ((!tst_bit(leitura_curva, sensor_de_curva)) && tst_bit(leitura_parada, sensor_de_parada)) {
        contador++;
        entrou_na_curva(value_erro); // Verifica se � uma curva
    } else if ((!tst_bit(leitura_curva, sensor_de_curva)) && (!tst_bit(leitura_parada, sensor_de_parada))) //verifica se � crizamento
    {
        frente();
        setDuty_1(PWMA);
        setDuty_2(PWMB);
    }

    while (contador == numParada) {
        freio();
    }
}

void calibra_sensores() {
    //=====Fun��o que inicializa a calibra��o====//
    for (int i = 0; i < 120; i++) {
        int sensores_frontais[] = {le_ADC(3), le_ADC(2), le_ADC(1), le_ADC(0), le_ADC(7), le_ADC(6)};
        for (int i = 0; i < 6; i++) {
            if (valor_min [i] < sensores_frontais [i]) {
                valor_min[i] = sensores_frontais[i];
            }
            else if (valor_max [i] > sensores_frontais[i]) {
                valor_max[i] = sensores_frontais [i];
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
        if (valor_min_abs < valor_min [i]) {
            valor_min_abs = valor_min [i];
        } else if (valor_max_abs > valor_max [i]) {
            valor_max_abs = valor_max [i];
        }
    }
}

void sensores() {

    int sensores_frontais[6] = {le_ADC(3), le_ADC(2), le_ADC(1), le_ADC(0), le_ADC(7), le_ADC(6)};
    //======Estabelece o limiar da leitura dos sensores====//
    //fun��o de corre��o da calibra��o
    for (int i = 0; i < 6; i++) {
        if (valor_min_abs < sensores_frontais[i]) {
            sensores_frontais[i] = valor_min_abs;
        }
        else if (valor_max_abs > sensores_frontais[i]) {
            sensores_frontais [i] = valor_max_abs;
        }

        sprintf(buffer, "%4d", sensores_frontais[i]); //Converte para string
        UART_enviaString(buffer); //Envia para o computador
        UART_enviaCaractere(0x20); //espa�o
    }
    UART_enviaCaractere(0x0A); //pula linha
}

void area_de_parada() {
    //--------------->AREA DOS SENSORES<---------------
    static int ejetor = 0;
    static unsigned int delta_T = 0;
    static unsigned int tempo_atual = 0;
    static unsigned int timer2, TempoEspera = 100;
    
    tempo_atual = millis;
    delta_T = tempo_atual - timer2;
    switch (ejetor) {
        case 0:
            if ((!(tst_bit(leitura_curva, sensor_de_curva))) || (!(tst_bit(leitura_parada, sensor_de_parada))))//verifica se sos sensores est�o em n�vel 0
            {
                timer2 = tempo_atual;
                ejetor = 1;
            }
            break;

        case 1:
            if ((delta_T) > TempoEspera) {
                parada(erro); // Verifica se � um marcador de parada
                ejetor = 2;
            }
            break;

        case 2:
            if ((tst_bit(leitura_curva, sensor_de_curva)) && (tst_bit(leitura_parada, sensor_de_parada))) {
                timer2 = 0;
                ejetor = 0;
            }
            break;
    }
}

    void sentido_de_giro() {
        //-----> �rea do senstido de giro


        if (erro < 0) //virar para a esquerda
        {
            entrou_na_curva(erro);
            set_bit(PORTB, led); //liga o LED
            /*while (erro < 0) {
                frente();
                setDuty_1(PWMA_C);
                setDuty_2(PWMB_C);
            }*/

        } 
        else if (erro > 0) { //cirar para a direita
            entrou_na_curva(erro);
            set_bit(PORTB, led); //liga o LED
            /*while (erro > 0) {
                frente();
                setDuty_1(PWMA_C);
                setDuty_2(PWMB_C);
            }*/
        }

        //A fun��o que fazia o rob� rodar em seu pr�prio eixo foi removida

    }

void PWM_limit() {
    //------> Limitando PWM

    if (PWMA > 1023) {
        PWMA = 1000;
    } else if (PWMB > 1023) {
        PWMB = 1000;
    }
}

void correcao_do_PWM() {

    int soma_direito = 0, soma_esquerdo = 0, denominador_direito = 6, denominador_esquerdo = 6, soma_total = 0;
    static unsigned int PWMR = 400; // valor da for�a do motor em linha reta
    int u = 0; //valor de retorno do PID
    
    static int peso [] = {-3, -2, -1, 1, 2, 3}; //utilizando um prescale de 2000

    int sensores_frontais[6] = {le_ADC(3), le_ADC(2), le_ADC(1), le_ADC(0), le_ADC(7), le_ADC(6)};
    for (int j = 0; j < 3; j++) {
        soma_esquerdo += (sensores_frontais[j] * peso[j]);
        soma_direito += (sensores_frontais[5 - j] * peso[5 - j]);
    }

    soma_total = (soma_esquerdo + soma_direito) / (denominador_esquerdo + denominador_direito);

    erro = 0 - soma_total;   //valor esperado(estar sempre em cima da linha) - valor medido

    /*sprintf(buffer, "%5d\n", erro); //Converte para string
    UART_enviaString(buffer); //Envia para o computador
    UART_enviaCaractere(0x0D); //pula linha*/

    //--------------->AREA DO PID<---------------

    u = PID(erro);

    PWMA = PWMR - u;
    PWMB = PWMR + u;

    frente();
    setDuty_1(PWMA);
    setDuty_2(PWMB);
}//fim do programa