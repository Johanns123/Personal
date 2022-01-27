/*Bibliotecas e frequ�ncia do uc*/
#define F_CPU 16000000        //define a frequencia do uC para 16MHz
#include <avr/io.h>           //Biblioteca geral dos AVR
#include <avr/interrupt.h>    //Biblioteca de interrup��o
#include <util/delay.h>
#include "ADC.h"
#include "PWM.h"
#include "UART.h"

//vari�veis de comando para os registradores
#define set_bit(y,bit) (y|=(1<<bit)) //coloca em 1 o bit x da vari�vel Y
#define clr_bit(y,bit) (y&=~(1<<bit)) //coloca em 0 o bit x da vari�vel Y
#define cpl_bit(y,bit) (y^=(1<<bit)) //troca o estado l�gico do bit x da vari�vel Y
#define tst_bit(y,bit) (y&(1<<bit)) //retorna 0 ou 1 conforme leitura do bit
/*==============================================================*/

/*Mapeamento de Hardware*/
#define sensor_de_curva          PB3
#define sensor_de_parada         PB4
#define led_placa                PB5
#define led_offboard             PC5
#define leitura_sensores        PINB
#define encoder_C1E              PB0
#define encoder_C2E              PB5
#define encoder_C1D              PD2
#define encoder_C2D              PD7
#define leitura_outros_sensores PIND


void setup_Hardware()
{
    MCUCR &= 0xef;      //habilita pull up quando configurado e desabilita algumas configura��es pr�vias do MCU

    DDRD  = 0b01111010;     //PD0, PD2 e PD7 como entrada, demais como sa�da
    PORTD = 0b00000000;     //todas as sa�das iniciam em 0 e entradas sem pull-up
    DDRB  = 0b00000110;     //PB1 e PB2 como sa�da e demais como entrada
    PORTB = 0b00011000;     //PB3 e PB4 com pull up ativado
    DDRC  = 0b00100000;     //PC5 como sa�da
    PORTC = 0b00011111;     //PC0-PC4 com pull-up ativado   
    TCCR1A = 0xA2; //Configura opera��o em fast PWM, utilizando registradores OCR1x para compara��o
    //TCCR1A = 0xA3 //Configura em phase correct PWM OC1A e OC1B limpos
    
    //esquerdo pino 4 - PD2
    UART_config(16); //Inicializa a comunica��o UART com 57.6kbps
    
    PWM_setFreq(4); //Seleciona op��o para frequ�ncia
    //62,5Hz de PWM
    
    ADC_init(); //Inicializa o AD
    INT_INIT(); //Inicializa o Timer0
    EXT_INIT(); //Inicializo as interrup��es externas

}//end setup_hardware

void INT_INIT()
{    //f�rmula nas primeiras linhas do c�digo
    TCCR0B = 0b00000010; //TC0 com prescaler de 8
    TCNT0 = 56; //Inicia a contagem em 56 para, no final, gerar 100us
    TIMSK0 = 0b00000001; //habilita a interrup��o do TC0
}

void EXT_INIT()
{
    EICRA  = 0b00000010; //habilito a interrup��o do INT0 por qualquer mudan�a de estado
    EIMSK  = 0b00000001; //habilito a interrup��o externa do INT0
    PCICR  = 0b00000001; //habilito a interrup��o externa do canal PCINT0
    PCMSK0 = 0b00000001; //habilito a porta PCINT0 como interrup��o externa do canal PCINT0
}
