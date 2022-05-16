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

/*tempo = 65536 * Prescaler/Fosc = 65536 * 1024/16000000 = 4, 19s   
 * tempo = X_bit_timer * Prescaler/Fosc
 Valor inicial de contagem = 256 - tempo_desejado*Fosc/Prescaler = 256 - 0,01*16000000/1024 = 98,75 = 99
 Valor inicial de contagem = X_bit_timer - tempo_desejado*Fosc/Prescaler*/

void f_timers(void);
void f_timer1(void);
void ext_int(void);
void setup(void);
void loop(void);

volatile char ch;
volatile char flag_com;

short int Max_timer1 = 0;

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

ISR (TIMER0_OVF_vect)
{
  f_timers();
}

ISR(PCINT2_vect)
{
  ext_int();
}

int main()
{

  setup();
  while (1) loop();
  return 0;
}

void f_timers()
{
  static unsigned short int counter1 = 1;

  if(counter1 < Max_timer1) counter1++;

  else
  {
    f_timer1();
    counter1 = 1;
  }

}

void setup() 
{
  /*DDRD = 0xff;
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
  tratar_leitura_do_ADC();*/

  DDRD = 0x0B;
  PORTD = 0x04;
  TCCR0B = 0x03;  //prescaler de 64
  TCNT0 = 6;      //250 contagens gera 1ms de tempo
  TIMSK0 = 0x01;  //habilito a interrupção do timer0
  PCICR = 0x04;
  PCMSK2 = 0x04;
  Max_timer1 = 500;
  sei();
}

void loop() 
{
  toggle_pin(PORTD, PD1);
  _delay_ms(800);
  /*static unsigned char unidade, dezena, centena;
  static int Volt;

  static float duty = 0;

  Volt = (500.0*AD_pins[0])/255.0;

  centena = Volt/100;
  dezena = (Volt%100)/10;
  unidade = Volt%10;

  duty = (AD_pins[0]<<2) * 100.0/1023.0;*/

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

  /*pwm_set_duty_service(AD_pins[0]<<2, PWM_CHANNEL_1);
  pwm_set_duty_service(AD_pins[0]<<2, PWM_CHANNEL_2);
  sprintf(buffer, "%d.%d%d duty %3.2f%%", centena, dezena, unidade, duty);
  cmd_LCD(0xC0, 0); //desloca o cursor para a segunda linha
  escreve_LCD(buffer);*/
}

void f_timer1()
{
  toggle_pin(PORTD, PD0);
}

void ext_int(void)
{
  static bool flag_bt = 0;
  static unsigned char read_bt = 0;

  read_bt = test_pin(PIND, PD2);

  if(!read_bt && flag_bt)
  {
    toggle_pin(PORTD, PD3);
    flag_bt = 0;
  }

  else if(read_bt && !flag_bt)
  {
    flag_bt = 1;
  }

}