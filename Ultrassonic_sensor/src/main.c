/* ===========================================================================

    Projeto 2 da Aula 2 do Módulo 2 Curso AVR Projetos Profissionais

    ARQUIVO main.c
    
    Sensor Ultrassônico com AVR
	
	O processador vai ler o pulso de echo do sensor e acionar as saídas
	conforme a distância para obstáculos.

    MCU:   ATmega328p
    Clock: 16MHz externo

    Compilador: Microchip Studio 7.0.2542
    Autor: Dr. Eng. Wagner Rambo
    Data de criação: Março de 2022
    Última atualização: 15/03/2022

============================================================================ */


// ============================================================================
// --- Frequência do MCU ---
#define   F_CPU   16000000


// ============================================================================
// --- Bibliotecas ---
#include <avr/io.h>                            //biblioteca de entradas e saídas padrão
#include <util/delay.h>                        //biblioteca delay
#include <avr/interrupt.h>	                   //biblioteca para interrupções 
#include <stdbool.h>

// ============================================================================
// --- Mapeamento de Hardware ---
#define    trig    (1<<PB1)
#define    R_LED   (1<<PC5)
#define    Y_LED   (1<<PC4)
#define    G_LED   (1<<PC3)


// ============================================================================
// --- Protótipo das Funções ---
void sens_out(uint16_t pulse, int gval, int yval, int rval);
void alarm_detecter(uint16_t pulse, uint8_t distance);
void f_timers(void);
void start_trigger(void);
void start_alarm(void);
void stop_trigger(void);
void setup(void);
void loop(void);
// ============================================================================
// --- Variáveis Globais ---
uint16_t  ptime = 0;                           //pulso de echo em micro segundos
bool start_blynking = 0;
unsigned char Maxtimer1 = 0, Maxtimer3 = 0;
unsigned int Maxtimer2 = 0;


// ============================================================================
// --- Interrupções ---
ISR(INT0_vect)
{

  if(!(TCCR1B&(1<<CS11)))                      //timer1 desligado?
  {                                            //sim
    TCCR1B |= (1<<CS11);                      //habilita contagem com PS 1:8
	  EICRA &= ~(1<<ISC00);                     //configura interrupção externa para borda de descida
	 
  }
  
  else
  {
    TCCR1B = 0x00;                             //desliga timer1
    ptime  = TCNT1;                            //salva largura de pulso em µs x 2
    TCNT1  = 0x00;                             //reinicia valor do timer1
    EICRA |= (1<<ISC00);                       //configura interrupção externa para borda de subida  
    ptime >>= 1;
    
  }
	
} //end ISR INT0


ISR(TIMER0_OVF_vect)
{
  //base de tempo de 1ms
	TCNT0 = 0x06;                              //reinicializa Timer0
  f_timers();
	
} //end ISR Timer0


ISR(TIMER2_OVF_vect)
{
  TCNT2 = 236;
  stop_trigger();
  TCCR2B = 0x00; //zero o timer de 10us
}

// ============================================================================
// --- Função Principal ---
int main(void)
{
  setup();
  while(1)  loop();	
} //end main


// ============================================================================
// --- Desenvolvimento das Funções ---

void setup()
{
  cli();                                       //desliga interrupções
  Maxtimer1 = 80;
  Maxtimer2 = 1000;
  Maxtimer3 = 10;
  TCNT0  = 0x06;                               //inicia Timer0 
  TCCR0B = 0x03;                               //prescaler 1:64
  TIMSK0 = 0x01;                               //habilita interrupção do Timer0
  TCNT2 = 236;                                //base de 10us 
  TIMSK2 = 0x01;
  EICRA |= (1<<ISC01) | (1<<ISC00);            //interrupção externa por borda de subida inicialmente
  EIMSK |= (1<<INT0);                          //habilita interrupção externa
  DDRB  |=  trig;                              //configura IOs do PORTB
  DDRC  |=  R_LED |	Y_LED | G_LED;             //configura IOs do PORTC
  PORTB &= ~trig;                              //inicializa trigger em LOW
  PORTC &= ~R_LED & ~Y_LED & ~G_LED;           //inicializa LEDs desligados
  sei();                                       //liga interrupções	
}

void loop(void)
{
  {
    //sens_out(ptime,20,15,10);
    //alarm_detecter(ptime, 18);
  }
}
// ============================================================================
// --- sens_out ---
// Aciona saídas conforme sensor
void sens_out(uint16_t pulse, int gval, int yval, int rval)
{

  if(pulse >= (gval*58))
    PORTC &= ~R_LED & ~Y_LED & ~G_LED;
	 
  if(pulse < (gval*58) && pulse >= (yval*58)) 
  {
	 PORTC |= G_LED;
	 PORTC &= ~R_LED & ~Y_LED; 
  }
  if(pulse < (yval*58) && pulse >= (rval*58)) //15cm
  {
	 PORTC |= G_LED | Y_LED; 
	 PORTC &= ~R_LED;
  }
  if(pulse < (rval*58)) //10cm
    PORTC |= R_LED | Y_LED | G_LED;
   
 	
} //end sens_out

void alarm_detecter(uint16_t pulse, uint8_t distance)
{
  if(pulse < distance*58)
  {
    start_blynking = 1;
  }
  else  start_blynking = 0;
}

void f_timers(void)
{
  static unsigned char cnt1 = 1, cnt3;
  static unsigned int cnt2 = 1;
	
  if(cnt1 < Maxtimer1)  cnt1++;

  else
  {
    //base de tempo de 80ms 
    cnt1 = 1;
    start_trigger();
  }

	if(cnt2 < Maxtimer2)  cnt2++;

  else
  {
    cnt2 = 1;
    if(start_blynking) start_alarm();
  }

  if(cnt3 < Maxtimer3)  cnt3++;

  else
  {
    //base de tempo de 10ms 
    cnt3 = 1;
    alarm_detecter(ptime, 18);
  }
}

void start_trigger(void)
{
  PORTB |=  trig;
  /*
  Para evitar o delay:
  Usar o timer 2 com base de tempo de 10 us ou 1us
  Ativa-lo nesta linha para dar a base de tempo de 10us e trocar o estado do pino
  */
  if(!TCCR2B)
  {
    TCCR2B = 0x02;  //prescaler de 8 	
  }
}

void start_alarm(void)
{
   PORTC ^= R_LED;
}

void stop_trigger(void)
{
  PORTB &= ~trig;
}
/* ============================================================================

                                       _
                                      / \
                                     |oo >
                                     _\=/_
                    ___         #   /  _  \   #
                   /<> \         \\//|/.\|\\//
                 _|_____|_        \/  \_/  \/
                | | === | |          |\ /|
                |_|  0  |_|          \_ _/
                 ||  0  ||           | | |
                 ||__*__||           | | |
                |* \___/ *|          []|[]
                /=\ /=\ /=\          | | |
________________[_]_[_]_[_]_________/_]_[_\_______________________________


============================================================================ */
/* --- Final do Arquivo main.c --- */