#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdbool.h>

#define set_bit(reg, bit)    (reg |= (1<<bit))
#define clr_bit(reg, bit)    (reg &= ~(1<<bit))
#define toggle_bit(reg, bit) (reg ^= (1<<bit))
#define test_bit(reg, bit)   (reg & (1<<bit))

void setup(void);
void loop(void);
void f_timers(void);
void read_bt_1(void);
void select_option(unsigned char option);
void read_bt_2(void);
void change_direction(void);
void stop(void);
void start_slowly(void);


unsigned char option = 0;
unsigned int max_value = 0;
bool motor_slowly = 0;

ISR(TIMER0_OVF_vect)
{
  TCNT0 =  6; 
  f_timers();
}

int main()
{
  setup();
  while(1) loop();
  return 0;
}

//256 - (0,001*16000000/64)
void setup(void)
{
  cli();  //desabilito as interrupções
  DDRD  = 0x06;  //PD1 e PD2 como saída
  PORTD = 0x18;   //PD3 e PD4 com pull up ativado
  DDRB = 0x04;    //PB2 como saída
  PORTB = 0x00;
  TCCR0B = 0x03;  //prescaler de 64
  TCNT0 =  6;   //inicializo em 6 para uma base de tempo de 1ms
  TIMSK0 = 0x01;
  TCCR1A = 0xA3;
  TCCR1B = 0x0C;  //freq. de 61Hz
  //TIMSK1 = 0x01;
  sei();
}

void loop(void)
{
}

void f_timers(void)
{
  static unsigned int counter1 = 1, counter2 = 1, counter3 = 1;

  if(counter1 < 1000) counter1++;

  else
  {
    read_bt_1();
    counter1 = 1;
  }


  if(counter2 < 10) counter2++;

  else
  {
    read_bt_2();
    counter2 = 1;
  }

  if(counter3 < 20) counter3++;

  else
  {
    start_slowly();
    counter3 = 1;
  }
}

void read_bt_1()
{
  static bool button = 0;

  button = test_bit(PIND, PD3);

  if(!button)//se botão pressionado incrementa a opção
  {
    option++;

  }

  else  select_option(option); //quando soltar o botao
  
}

void read_bt_2(void)
{
  static bool button = 0;

  button = test_bit(PIND, PD4);

  if(!button)//se botão pressionado incrementa a opção
  {    
    if(max_value > 1024)  max_value = 0;
    else
    {
      max_value+=100;
    }
  }
}

void select_option(unsigned char option)
{
  switch (option)//verifica modeo de operacao
    {
      case 0: break;
      case 1:
        change_direction();
        motor_slowly = 0;
        break;
      
      case 2:
        stop();
        motor_slowly = 0;
        break;
      
      case 3:
      case 4:
        motor_slowly = 1;
        option = 0;
        break;

      default:
        motor_slowly = 1;
        option = 0;
        break;
  }
}

void change_direction()
{
  static bool direction = 0;

  if(!direction)
  {
    set_bit(PORTD, PD1);
    clr_bit(PORTD, PD2);
    direction = 1;
  }

  else
  {
    clr_bit(PORTD, PD1);
    set_bit(PORTD, PD2);
    direction = 0;
  }
  option = 0;
}

void stop(void)
{
  clr_bit(PORTD, PD1);
  clr_bit(PORTD, PD2);
  option = 0;
}

void start_slowly(void)
{
  if(motor_slowly){
    if (OCR1B != max_value)   OCR1B++;
    else  
    { motor_slowly = 0;
      max_value = 0;
    }
    if(OCR1B > 1023)  OCR1B = 0;
    option = 0;
  }
}