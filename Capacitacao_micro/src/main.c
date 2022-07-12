#include "main.h"

unsigned int Max_timer1;  //variavel que determina a contagem maxima

int main()
{

  setup();  //acontece uma única vez
  while (1) loop();
  return 0;
}


void setup() 
{

  DDRD = 0x0C;  //0b00001100 -> PD2 e PD3 como saida
  PORTD = 0x00; //0b00000000
  TCCR0B = 0x03;  //prescaler de 64
  TCNT0 = 6;      //250 contagens gera 1ms de tempo
  TIMSK0 = 0x01;  //habilito a interrupção do timer0
  Max_timer1 = 200; //conto até 200 para 200ms de tempo
  sei();  //chave geral das interrupções
}

void loop() 
{
  toggle_bit(PORTD, PD3);
  _delay_ms(500);
}


void temporizador(void)
{
  static unsigned int contador1 = 1;

  if(contador1 < Max_timer1)  contador1++;

  else
  {
    rotina1();
    contador1 = 1;
  }
}

void rotina1(void)
{
  toggle_bit(PORTD, PD2);
}