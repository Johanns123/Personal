#include "HAL_atmega328p.h"


void setup_Hardware()
{
    MCUCR &= 0xef;      //habilita pull up quando configurado e desabilita algumas configura��es pr�vias do MCU

    DDRD = 0b01111010; //PD3 - PD6 definidos como sa�da, PD7 como entrada, PD0 como entrada(RX) e PD1 como sa�da(TX)
    PORTD = 0b10000000; //inicializados em n�vel baixo e PD7 com pull up
    DDRB = 0b00100110; //Habilita PB0 como entrada e PB5, PB1 e PB2 como sa�da
    PORTB = 0b00000001; //PORTB inicializa desligado e pull up no PB0
    DDRC = 0b00000000; //PORTC como entrada
    PORTC = 0b00001111; //PC3 - PC0 com pull up (colocar resistor de pull up nos pinos A6 e A7)
    TCCR1A = 0xA2; //Configura opera��o em fast PWM, utilizando registradores OCR1x para compara��o
    //TCCR1A = 0xA3 //Configura em phase correct PWM OC1A e OC1B limpos
    
    //esquerdo pino 4 - PD2
    UART_config(16); //Inicializa a comunica��o UART com 57.6kbps
    
    setFreq(4); //Seleciona op��o para frequ�ncia
    //62,5Hz de PWM
    
    ADC_init(); //Inicializa o AD
    INT_INIT(); //Inicializa o Timer0

}//end setup_hardware

void INT_INIT()
{    //f�rmula nas primeiras linhas do c�digo
    TCCR0B = 0b00000010; //TC0 com prescaler de 8
    TCNT0 = 56; //Inicia a contagem em 56 para, no final, gerar 100us
    TIMSK0 = 0b00000001; //habilita a interrup��o do TC0
}