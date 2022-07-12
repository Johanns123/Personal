/*--------------Definicao das bibliotecas-------------*/
#include <avr/io.h>
#include <util/delay.h>
#include <stdbool.h>
//#include <stdio.h>
#include <avr/interrupt.h>
//#include "uart.h"
//#include "ADC.h"
//#include "Display.h"
//#include "PWM.h"

/*--------------Definicao das macros-------------*/
#define set_bit(reg,bit)  (reg |= (1<<bit))
#define clear_bit(reg,bit)  (reg &= ~(1<<bit))
#define toggle_bit(reg,bit)  (reg ^= (1<<bit))
#define test_bit(reg,bit)  (reg & (1<<bit))

/*--------------Definicao do protóripo das funcoes-------------*/
void temporizador(void);
void rotina1(void);
void setup(void);
void loop(void);

/*tempo = 65536 * Prescaler/Fosc = 65536 * 1024/16000000 = 4, 19s   
 * tempo = X_bit_timer * Prescaler/Fosc
 Valor inicial de contagem = 256 - tempo_desejado*Fosc/Prescaler = 256 - 0,01*16000000/1024 = 98,75 = 99
 Valor inicial de contagem = X_bit_timer - tempo_desejado*Fosc/Prescaler*/

//Vi = 256 - (0,001*16000000)/64 = 6 -> 255 = 250 contagens  


/*--------------Definicao das interrupcoes-------------*/

//ISR(USART_RX_vect)
//{
//  ch  = UDR0;
//  uart_caractere_sending_service(ch);
//  flag_com = 1;
//}
//
//ISR(ADC_vect)
//{
//  tratar_leitura_do_ADC();
//}


//ISR(PCINT2_vect)
//{
//  
//}


ISR (TIMER0_OVF_vect)
{
  temporizador();
}