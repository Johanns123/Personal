/* ===========================================================================

    Projeto da Aula 3 do Módulo 2 Curso AVR Projetos Profissionais
	
	LCD com microcontrolador AVR

    ARQUIVO main.c
    
    Projeto Principal

    MCU:   Atmega328p
    Clock: 16MHz Cristal Externo

    Compilador: Microchip Studio 7.0.2542
    Autor: Dr. Eng. Wagner Rambo
    Data de criação: Março de 2022
    Última atualização: 19/03/2022

============================================================================ */


// ============================================================================
// --- Funções do LCD ---
#include "lcd.h"
#include "ADC.h"
#include <avr/interrupt.h>
#include <stdbool.h>

#define toggle_bit(reg, bit) (reg^= (1<<bit))

// ============================================================================
// --- Mapeamento de Hardware ---
#define   ENT   PC5
#define   ESC   PC4
#define   INC   PC3
#define   DEC   PC2
#define   LED   PC1
#define MENU_MAX  3

// ============================================================================
// --- Protótipo das Funções ---
void menu_ihm();
void contrast(uint8_t duty);
void backlight(uint8_t duty);
void led_ctr(uint8_t ledst);
void read_keyb();
void boot_func();
void setup(void);
void loop(void);
void led_sleep_routine(uint16_t sleep);
void counter(void);
void routine1(void);
void start_game();
void pswd_choose(void);

uint16_t sleep_time;
bool bt_press;
char btp = '0';
uint8_t number1 = 0;
uint8_t number2 = 0;
uint8_t number3 = 0;
uint8_t number4 = 0;
uint8_t password[4] = {2,4,8,9};
uint8_t memory_password[4] = {0};
uint8_t i = 0;
bool check = 0;

ISR(ADC_vect) 
{
  tratar_leitura_do_ADC();
}

ISR(TIMER0_OVF_vect)
{
  TCNT0 = 6;
  counter();
}

ISR(PCINT2_vect)
{
    read_keyb();
}
// ============================================================================
// --- Função Principal ---
int main(void)
{
  setup();
  while(1)	 loop();
	
} // end main


// ============================================================================
// --- Desenvolvimento das Funções ---

void setup(void)
{
  set_bit(DDRB,RS);		                          //configura RS como saída
  set_bit(DDRB,EN);						                  //configura EN como saída
  set_bit(DDRB,PB2);
  set_bit(DDRB,PB3);
  set_bit(DDRB,PB4);
  set_bit(DDRD,D4);                            //configura D4 como saída	
  set_bit(DDRD,D5);                            //configura D5 como saída
  set_bit(DDRD,D6);                            //configura D6 como saída
  set_bit(DDRD,D7);                            //configura D7 como saída 

  DDRB |= (1<<PB2) | (1<<PB3);

  set_bit(PORTD, PD2);
  set_bit(PORTD, PD3);

  disp_init();                                 //inicializa o display
  disp_cmd(0x0C);                              //desliga o cursor e o blink

  adc_setup();
  tratar_leitura_do_ADC();
  TCCR0B = 0x03;  //prescaler de 64
  TCNT0  = 6;     //1ms de tempo
  TIMSK0 = 0x01;

  PCICR = 0x04;
  PCMSK2 |= (1<<PCINT18) | (1<<PCINT19);
  sei();
  boot_func();                                 //chama função de boot do sistema
}

void counter(void)
{
  static uint16_t counter1 = 1;

    if(counter1 < 10) counter1++;

    else
    {
      pswd_choose();
      counter1 = 1;
    }
}

void loop(void)
{
  menu_ihm();
}
// ============================================================================
// --- menu_ihm ---
// Apresentação dos menus
void menu_ihm()
{
  start_game();
} //end menu_ihm


// ============================================================================
// --- read_keyb ---
// Leitura do Teclado
void read_keyb()
{
  static uint8_t data1 = 0, data2 = 0, read1 = 0, read2 = 0, memory_read1 = 0, memory_read2 = 0;

  data1 = test_bit(PIND, PD2);
  data2 = test_bit(PIND, PD3);

  if(!data1)  //botao pressionado
  {
    read1 = 0;
    
  }

  else
  {
    read1 = 1;
  }

  if(read1 < memory_read1)
  {
    switch (i)
    {
      case 0:
        memory_password[i] = number1;
        break;
  
      case 1:
        memory_password[i] = number2;
        break;

      case 2:
        memory_password[i] = number3;
        break;

      case 3:
        memory_password[i] = number4;
        break;

      default:
        break;
    }
    i++;
    if(i > 4) i = 4;
    toggle_bit(PORTB, PB2);
  }

  if(!data2)  //botao pressionado
  {
    read2 = 0;
  }

  else
  {
    read2 = 1;
  }

  if(read2 < memory_read2)
  {
    check = 1;
    toggle_bit(PORTB, PB3);
  }

  memory_read1 = read1;
  memory_read2 = read2;
} //end read_keyb


// ============================================================================
// --- boot_func ---
// Boot do sistema
void boot_func()
{
    
  disp_wr_po('C',0,3);
  disp_write('O');
  disp_write('F');
  disp_write('R');
  disp_write('E');
  disp_wr_po('.',1,0);
  
  for(int i=0; i<16;i++)
  {
    disp_write('.');
	  _delay_ms(125);
  } //end for
  
  disp_clear();
    
	
} //end boot_func

void start_game(void)
{
  static uint8_t correct = 0;

  disp_wr_po('A',0,0);
  disp_write('D');
  disp_write('I');
  disp_write('V');
  disp_write('I');
  disp_write('N');
  disp_write('H');
  disp_write('E');
  disp_write(' ');
  disp_write('S');
  disp_write('E');
  disp_write('N');
  disp_write('H');
  disp_write('A');
  
  switch (i)
  {
    case 0:
      disp_number(number1, 1, 2);
      break;
  
    case 1:
      disp_number(number2, 1, 3);
      break;

    case 2:
      disp_number(number3, 1, 4);
      break;

    case 3:
      disp_number(number4, 1, 5);
      break;

    default:
      break;
  }

  if(check)
  { 
    disp_clear();
    for(uint8_t i = 0; i < 3; i++)
    {
      if(memory_password[i] == password[i])
      {
        correct++;
      }
    }
    if(correct > 2)
    {
      for(uint8_t i = 0; i < 16; i++)
      {
        disp_write('.');
        _delay_ms(10);
      }
      disp_clear();
      disp_wr_po('P', 0, 0);
      disp_write('A');
      disp_write('R');
      disp_write('A');
      disp_write('B');
      disp_write('E');
      disp_write('N');
      disp_write('S');
      disp_wr_po('D', 1, 0);
      disp_write('E');
      disp_write('S');
      disp_write('T');
      disp_write('R');
      disp_write('A');
      disp_write('V');
      disp_write('A');
      disp_write('D');
      disp_write('O');
      disp_write('!');
      while(1);
    }

    else
    {
      for(uint8_t i = 0; i < 16; i++)
      {
        disp_write('.');
        _delay_ms(10);
      }
      disp_clear();
      disp_wr_po('Q', 0, 0);
      disp_write('U');
      disp_write('E');
      disp_write(' ');
      disp_write('P');
      disp_write('E');
      disp_write('N');
      disp_write('A');
      disp_wr_po('T', 1, 0);
      disp_write('E');
      disp_write('N');
      disp_write('T');
      disp_write('E');
      disp_write(' ');
      disp_write('N');
      disp_write('O');
      disp_write('V');
      disp_write('A');
      disp_write('M');
      disp_write('E');
      disp_write('N');
      disp_write('T');
      disp_write('E');
      disp_write('!');
      while(1);
    }
  }
}

void pswd_choose(void)
{
  static uint8_t PSWD = 0;
  PSWD = (sensores_de_tensao[0] >> 4);

  if(PSWD > 9)
    PSWD = 9;

  switch (i)
  {
  case 0:
    number1 = PSWD;
    break;
  
  case 1:
    number2 = PSWD;
    break;

  case 2:
    number3 = PSWD;
    break;

  case 3:
    number4 = PSWD;
    break;

  default:
    break;
  }
  
}



/* ============================================================================

                                       _
                                      / \
                                     |oo >      <-- (IHM, it works!)
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


