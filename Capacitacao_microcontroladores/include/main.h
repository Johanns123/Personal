#include <avr/io.h>
#include <util/delay.h>
#include <stdbool.h>
#include <stdio.h>
#include <avr/interrupt.h>
#include "uart.h"
#include "ADC.h"
#include "Display.h"
#include "PWM.h"

#define set_bit(reg,bit)  (reg |= (1<<bit))
#define clear_bit(reg,bit)  (reg &= ~(1<<bit))
#define toggle_bit(reg,bit)  (reg ^= (1<<bit))
#define test_bit(reg,bit)  (reg & (1<<bit))


void setup(void);
void loop(void);
void contador(void);
void read_key(void);
void rotina1(void);
void rotina2(void);

unsigned char MaxTimer1, MaxTimer2;

volatile char ch;
volatile char flag_com;

char buffer [10] = {0};

ISR(USART_RX_vect)
{
  ch  = UDR0;
  uart_caractere_sending_service(ch);
  flag_com = 1;
}

ISR(ADC_vect)
{
  tratar_leitura_do_ADC();
}

ISR(TIMER0_OVF_vect)
{
    TCNT0 = 6;
    contador();
}

ISR(PCINT2_vect)
{
    read_key();
}
