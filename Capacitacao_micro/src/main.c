//#define F_CPU 16000000   //frequencia do MCU
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#define set_pin(reg,pin)  (reg |= (1<<pin)) //seto a saída em 1, OR
#define clear_pin(reg,pin)  (reg &= ~(1<<pin))  //seto a saída em 0, AND e NOT
#define toggle_pin(reg,pin)  (reg ^= (1<<pin))  //troco e estado, XOR
#define test_pin(reg,pin)  (reg & (1<<pin))     //testo a entrada, AND

/*tempo = 65536 * Prescaler/Fosc = 65536 * 1024/16000000 = 4, 19s   
 * tempo = X_bit_timer * Prescaler/Fosc
 Valor inicial de contagem = 256 - tempo_desejado*Fosc/Prescaler = 256 - 0,01*16000000/1024 = 98,75 = 99
 Valor inicial de contagem = X_bit_timer - tempo_desejado*Fosc/Prescaler*/

//vi = 256 - (0,001*16000000/64) = 6 


unsigned int Maxtimer1 = 0, Maxtimer2 = 0;

void setup(void); //definicao das funcoes
void loop(void);
void f_timers(void);  //funcao onde se encontra outras bases de tempo
void f_timer1(void);
void f_timer2(void);
void read_bt(void);

ISR (TIMER0_OVF_vect)
{
  TCNT0 = 6;
  f_timers(); //chamada a cada 1ms
}

ISR(PCINT2_vect)
{
  read_bt();
}

int main()
{
  setup();
  while (1) loop();
  return 0;
}

void setup()
{
  cli();  //desabilito as interrupções
  DDRD   = 0b00110100; //defino PD2, PD4 e PD5 como saída
  PORTD  = 0b00001000; //0x00, inicializo com o led apagado
  TCCR0B = 0b00000011;  //prescaler de 64
  TCNT0 = 6;  //configuro para uma base de tempo de 1ms
  TIMSK0 = 0b00000001; //habilita a interrupção do timer0
  PCICR  = 0b00000100;  //Habilito PCINT2
  PCMSK2 = 0b00001000; //Habilito PCINT19
  Maxtimer1 = 300;
  Maxtimer2 = 800;
  sei();  //habilito as interrupções
}

void loop()
{
  toggle_pin(PORTD, PD4); //inverto o estado do led
  _delay_ms(500);
}

void f_timers(void)
{
  static unsigned int counter1 = 1, counter2 = 1;

  if(counter1 < Maxtimer1)  counter1++;

  else
  {
    f_timer1(); //chamada a cada 300ms
    counter1 = 1;
  }

  if(counter2 < Maxtimer2) counter2++;

  else
  {
    //f_timer2(); //chamada a cada 800ms
    counter2 = 1;
  }
}

void f_timer1(void)
{
  toggle_pin(PORTD, PD5);
}

void f_timer2(void)
{
  //toggle_pin(PORTD, PD2);
}

void read_bt(void)
{
  static unsigned char button = 0, flag_bt = 0;

  button = test_pin(PIND, PD3);

  if(!button && !flag_bt) //botão pressionado e flag = 0? ...
  {
    toggle_pin(PORTD, PD2);
    flag_bt = 1;
  }

  else if(button && flag_bt) //botão solto e flag = 1? ...
  {
    flag_bt = 0;
  }


}