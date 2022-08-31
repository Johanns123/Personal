#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>
#include "stepper.hpp"
/*

Passo completo com alto Torque:
0011
0110
1100 
1011

Passo completo com baixo torque:
0001
0010
0100
1000

Meio Passo:
0001
0011
0010
0110
0100
1100
1000
1001

*/

#define set_bit(reg, bit)        (reg |= (1<<bit))
#define clear_bit(reg, bit)      (reg &= ~(1<<bit))
#define toggle_bit(reg, bit)     (reg ^= (1<<bit))
#define test_bit(reg, bit)       (reg & (1<<bit))

void setup(void);
void loop(void);
void f_timers(void);
void steps(void);

uint8_t speed;
Stepper stepMotor;

ISR(TIMER0_OVF_vect)
{
  f_timers();
}

int main()
{
  setup();

  while(1)  loop();
  return 0;
}

void setup(void)
{
  DDRD = 0xf0;  //PD4 ao PD7 como saída
  PORTD = 0x00;
  DDRB |= (1<<PB5); 
  TCCR0B = 0x03;  //prescaler de 64
  TCNT0 = 6;    //tempo de 1ms
  TIMSK0 = 0x01;
  sei();
  stepMotor.set_stepper_motor_mode(Full_swing_High);
  speed = stepMotor.chose_speed(2); //2ms of each coil activation
}

void loop(void)
{

}

void f_timers(void)
{
  static uint8_t counter = 1;

  if(counter < speed)
    counter++;

  else
  {
    steps();
    counter = 1;
  }

}

void steps()  //stepper motor rotation to one direction  with an specific angle and turn to another
{
  static uint8_t estado = 0;
  static bool terminado = 0;
  switch (estado)
  {
  case 0:
    stepMotor.select_angle(360, 0);
    terminado = stepMotor.run();
    if(terminado)
    {
      estado = 1;
      stepMotor.clear();
    } 
    break;
  

  case 1:
    stepMotor.select_angle(360, 1);
    terminado = stepMotor.run();
    if(terminado)
    { 
      estado = 0;
      stepMotor.clear();
    }
    break;
  
  default:
    break;
  }
}