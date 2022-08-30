/* ===========================================================================

    Projeto da Aula 4 do M�dulo 2 Curso AVR Projetos Profissionais
	
	Controle de Servos com microcontrolador AVR e IHM

    ARQUIVO main.c
    
    Projeto Principal

    MCU:   Atmega328p
    Clock: 16MHz Cristal Externo

    Compilador: Microchip Studio 7.0.2542
    Autor: Dr. Eng. Wagner Rambo
    Data de cria��o: Mar�o de 2022
    �ltima atualiza��o: 19/03/2022

============================================================================ */


// ============================================================================
// --- Fun��es do LCD ---
#include "lcd.h"
#include <stdbool.h>

// ============================================================================
// --- Mapeamento de Hardware ---
#define   SV1   PB4
#define   SV2   PB5


// ============================================================================
// --- Prot�tipo das Fun��es ---
void sv_angle(uint8_t angle);                  //temporiza��o conforme �ngulo do servo


// ============================================================================
// --- Vari�veis Globais ---
uint16_t  psv1 =  90,
          psv2 =  0;

bool flag_timer = 0;
bool timer1_start = 0;

// ============================================================================
// --- Interrup��o ---
ISR(TIMER0_OVF_vect)
{
  static uint8_t cnt = 0, cnt1 = 0;
  //base de tempo de 1ms
  TCNT0 = 0x06;
  cnt++;
  cnt1++;

  if(cnt==16)
  {
	//base de tempo de 16ms  
    cnt = 0;
	
    if(!timer1_start)
    {
      set_bit(PORTB,SV1);
      sv_angle(psv1);	 
      set_bit(PORTB,SV2);
      sv_angle(psv2);
      timer1_start = 1;
    }
  } //end cnt
  	
  if(cnt1 == 5)
  {
    static bool sentido = 0;
    if(psv2 >= 0 && !sentido)
    {
      psv2++;
      if(psv2 == 180) sentido = 1;
    }
    if(psv2 <= 180 && sentido)
    {  
      psv2--;
      if(!psv2) sentido = 0;
    }

    cnt1 = 0;
  }

} //end ISR


ISR(TIMER1_OVF_vect)
{
  if(flag_timer)
  {
    TCCR1B &= ~(1<<CS11);                      //desliga Timer1
    flag_timer = 0;
    clr_bit(PORTB,SV1);
    clr_bit(PORTB,SV2);
    timer1_start = 0;
  }

}

// ============================================================================
// --- Fun��o Principal ---
int main(void)
{
  set_bit(DDRB,SV1);                           //configura sa�da para o servo 1
  set_bit(DDRB,SV2);                           //configura sa�da para o servo 2 
  
  cli();                                       //desliga interrup��es
  TCCR0B =  0x03;                              //Timer0 com PS 1:64
  TCNT0  =  0x06;                              //Timer0 contar� at� 250
  TIMSK0 =  0x01;                              //habilita interrup��o do Timer0  
  TIMSK1 =  0x01;                              //habilita interrup��o do Timer0  
  sei();                                       //habilita interrup��es
       
  
  while(1)
  {
  }
	
} // end main



// ============================================================================
// --- Desenvolvimento das Fun��es ---


// ============================================================================
// --- sv_angle ---
// Posiciona o servo no �ngulo desejado
void sv_angle(uint8_t angle)
{
	static uint16_t s_delay;
	
	if(angle>180) angle=180;                   //�ngulo m�ximo: 180 graus
	
	s_delay = angle*10 + 600;                  //calcula o valor de delay para o servo
	TCNT1   = 65536-(s_delay<<1);              //inicializa Timer1 conforme �ngulo desejado
	TCCR1B |=  (1<<CS11);                      //liga Timer1 com PS 1:8
	flag_timer = 1;
 
} //end sv_angle



/* ============================================================================

                                       _
                                      / \
                                     |oo >      <-- (SERVO, I wouldn't exist without you!)
                                     _\=/_
                    ___         #   /  _  \   #
                   /<> \         \\//|/.\|\\//
 (Me neither) -->_|_____|_        \/  \_/  \/
                | | === | |          |\ /|
                |_|  0  |_|          \_ _/
                 ||  0  ||           | | |
                 ||__*__||           | | |
                |* \___/ *|          []|[]
                /=\ /=\ /=\          | | |
________________[_]_[_]_[_]_________/_]_[_\_______________________________


============================================================================ */
/* --- Final do Arquivo main.c --- */



