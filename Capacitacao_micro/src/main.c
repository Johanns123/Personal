#include <avr/io.h>
#include <util/delay.h>
#include <stdbool.h>
#include <stdio.h>
#include <avr/interrupt.h>
#include "uart.h"
#include "ADC.h"
#include "Display.h"
#include "PWM.h"

#define set_pin(reg,pin)  (reg |= (1<<pin))
#define clear_pin(reg,pin)  (reg &= ~(1<<pin))
#define toggle_pin(reg,pin)  (reg ^= (1<<pin))
#define test_pin(reg,pin)  (reg & (1<<pin))


void setup(void);
void loop(void);

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

int main()
{

  setup();
  while (1) loop();
  return 0;
}

void setup() 
{
  DDRD = 0xff;
  PORTD = 0x00;
  set_pin(DDRB, PB1);
  set_pin(DDRB, PB2);
  set_pin(DDRB, PB3);
  set_pin(DDRB, PB5);
  //uart_setup(16); //se colocar display no PD0 comentar esta linha
  adc_setup();
  inic_LCD_4bits();
  escreve_LCD("  Voltimetro");
  TCCR1A = 0xA3; //Configura operacao em fast PWM, utilizando registradores OCR1x para comparacao
  setup_pwm_setFreq(11);
  sei();
  tratar_leitura_do_ADC();
}

void loop() 
{
  static unsigned char unidade, dezena, centena;
  static int Volt;

  static float duty = 0;

  Volt = (500.0*AD_pins[0])/255.0;

  centena = Volt/100;
  dezena = (Volt%100)/10;
  unidade = Volt%10;

  duty = (AD_pins[0]<<2) * 100.0/1023.0;

  /*static unsigned char counter = 0;
  static bool flag_bt = 0;
  static bool read_bt = 0;
  static bool option = 0;
  static char buffer [10] = {0};

  read_bt = test_pin(PINB, PB0);

  if(!read_bt && !flag_bt)
  {
    if(!option)
    {
      option = 1;
    }
    else  option = 0;
    
    flag_bt = 1;
  }

  else if(read_bt && flag_bt)
  {
    flag_bt = 0;
  }

  if(!option)
  {
    PORTD = counter;
    _delay_ms(400);
  }
  
  else 
  {*/
    /*if(counter%2)
    {
      PORTD = 170;
    }

    else
    {
      PORTD = 85;
    }*/
    /*PORTD = AD_pins[0];
    sprintf(buffer, "%d\n", AD_pins[0]);
    uart_string_sending_service(buffer);
    _delay_ms(1);
  }
  counter++;*/
  /*static float Volt = 0;

  Volt = ((5.0*AD_pins[0])/255.0);

  sprintf(buffer, "  %.2f", Volt);
  cmd_LCD(0xC0, 0); //desloca o cursor para a segunda linha
  escreve_LCD(buffer);*/

  pwm_set_duty_service(AD_pins[0]<<2, PWM_CHANNEL_1);
  pwm_set_duty_service(AD_pins[0]<<2, PWM_CHANNEL_2);
  sprintf(buffer, "%d.%d%d duty %3.2f%%", centena, dezena, unidade, duty);
  cmd_LCD(0xC0, 0); //desloca o cursor para a segunda linha
  escreve_LCD(buffer);
}