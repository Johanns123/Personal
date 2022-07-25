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

uint16_t sleep_time;
bool bt_press;
char btp = '0';

ISR(TIMER0_OVF_vect)
{
  TCNT0 = 6;
  counter();
}

ISR(PCINT1_vect)
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
  set_bit(DDRB,RS);		                       //configura RS como saída
  set_bit(DDRB,EN);						       //configura EN como saída
  set_bit(DDRD,D4);                            //configura D4 como saída	
  set_bit(DDRD,D5);                            //configura D5 como saída
  set_bit(DDRD,D6);                            //configura D6 como saída
  set_bit(DDRD,D7);                            //configura D7 como saída 
  
  set_bit(DDRC,LED);                           //configura saída para o LED
  set_bit(DDRD,CONTR);                         //configura saída para PWM do contraste
  set_bit(DDRB,BACKL);                         //configura saída para PWM do backlight
   
  set_bit(PORTC,ENT);                          //habilita pull-ups para os botões
  set_bit(PORTC,ESC);
  set_bit(PORTC,INC);
  set_bit(PORTC,DEC);
  
  disp_init();                                 //inicializa o display
  disp_cmd(0x0C);                              //desliga o cursor e o blink

  TCCR2A =  0xA3;                              //fast PWM em OC2A,OC2B para ajuste digital de backlight e contraste
  TCCR2B |= (1<<CS21);                         //f = 7812,5Hz
  OCR2A  =  0x80;                              //duty cycle inicial do PWM do backlight
  OCR2B  =  0x0A;                              //duty cycle inicial do PWM do contraste
  

  TCCR0B = 0x03;  //prescaler de 64
  TCNT0  = 6;
  TIMSK0 = 0x01;

  PCICR = 0x02;
  PCMSK1 |= (1<<ENT) | (1<<ESC) | (1<<INC) | (1<<DEC);
  sei();
  boot_func();                                 //chama função de boot do sistema
}

void counter(void)
{
  static uint16_t counter1 = 1;

  if(bt_press)
  {
    if(counter1 < sleep_time) counter1++;

    else
    {
      routine1();
      counter1 = 1;
    }
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
  static  int8_t menu_num =  0,
                 blpwm    = 50,
				         copwm    = 80;
  
  static uint8_t ledon =  0;
                
  static uint16_t led_time = 100;
  
  switch(btp)
  {
    case 'E':
	  disp_cmd(0x01);
	  menu_num++;
	  if(menu_num>MENU_MAX) menu_num=MENU_MAX;
	  break;
	
	case 'C':
	  disp_cmd(0x01);
	  menu_num--;
	  if(menu_num<0) menu_num=0;
	  break;
	  
	case 'I':
	  switch(menu_num)
	  {
		case 0:
		  copwm+=5;
		  if(copwm>100) copwm=0;
		  break; 
		  
		case 1:
		  blpwm+=5;
		  if(blpwm>100) blpwm=0;
		  break;  
		
		case 2:
		  ledon = 1;
		  break;   

		case 3:
		  led_time += 100;
      if(led_time > 1200)  led_time = 100;
		  break;  
	  }
	  break;  
	  
	case 'D':
	  switch(menu_num)
	  {
		case 0:
		  copwm-=5;
		  if(copwm<0) copwm=100;
		  break;
		  
		case 1:
		  blpwm-=5;
		  if(blpwm<0) blpwm=100;
		  break;  
		  
		case 2:
		  ledon = 0;
		  break;  

		case 3:
		  led_time -= 100;
      if(led_time < 100)  led_time = 1200;
		  break;  
	  }
	  break;    
  
  } //end switch read_keyb
   
  btp = '0';

  switch(menu_num)
  {
	case 0:  
      contrast(copwm);
	  break;
 
  case 1:
    backlight(blpwm);
	  break;
	  
	case 2:
	  led_ctr(ledon);
	  break;  
  
  case 3:
    led_sleep_routine(led_time);
    break;
  
  } //end switch menu_num
	
} //end menu_ihm


// ============================================================================
// --- contrast Menu 0 ---
// Ajuste de contraste
void contrast(uint8_t duty)
{
   
  //título	
  disp_wr_po('C',0,3);
  disp_write('o');
  disp_write('n');
  disp_write('t');
  disp_write('r');
  disp_write('a');
  disp_write('s');
  disp_write('t');
  //setas
  disp_wr_po('>',1,15);
  
  OCR2B = 100-duty;
  disp_number(duty,1,4);
  disp_write('%');
  
  
} //end contrast


// ============================================================================
// --- backlight Menu 1 ---
// Ajuste de backlight
void backlight(uint8_t duty)
{
  
  //título
  disp_wr_po('B',0,3);
  disp_write('a');
  disp_write('c');
  disp_write('k');
  disp_write(' ');
  disp_write('l');
  disp_write('i');
  disp_write('g');
  disp_write('h');
  disp_write('t');
  //setas
  disp_wr_po('<',1,0);
  disp_wr_po('>',1,15);
  
  OCR2A = duty*255/100;
  disp_number(duty,1,4);
  disp_write('%');
	
} //end backlight


// ============================================================================
// --- led_ctr Menu 2 ---
// Controle de LED
void led_ctr(uint8_t ledst)
{
  //título
  disp_wr_po('L',0,3);
  disp_write('E');
  disp_write('D');
  disp_write(' ');
  disp_write('C');
  disp_write('o');
  disp_write('n');
  disp_write('t');
  disp_write('r');
  disp_write('o');
  disp_write('l');
  //setas
  disp_wr_po('<',1,0);
  disp_wr_po('>',1,15);
  
  if(ledst)
  {
    disp_wr_po(' ',1,3);
	  disp_write('O');
	  disp_write('N');
	  bt_press = 1;
  }
  else
  {
	  disp_wr_po('O',1,3);
	  disp_write('F');
	  disp_write('F');
	  bt_press = 0;    
    clr_bit(PORTC, LED);
  }
	
} //end led_ctr

void led_sleep_routine(uint16_t sleep)
{
  disp_wr_po('T',0,1);
  disp_write('I');
  disp_write('M');
  disp_write('E');
  disp_write(' ');
  disp_number(sleep, 0, 6);
  disp_write('m');
  disp_write('s');
  disp_wr_po('<',1,0);

  sleep_time = sleep;
}

void routine1(void)
{
  toggle_bit(PORTC, LED);
}
// ============================================================================
// --- read_keyb ---
// Leitura do Teclado
void read_keyb()
{
  static uint8_t flag1 = 0,
                 flag2 = 0,
				         flag3 = 0,
				         flag4 = 0;

  if(!(inp_bit(PINC,ENT)) && !flag1) flag1 = 1;
  if(!(inp_bit(PINC,ESC)) && !flag2) flag2 = 1;
  if(!(inp_bit(PINC,INC)) && !flag3) flag3 = 1;
  if(!(inp_bit(PINC,DEC)) && !flag4) flag4 = 1;	
  
  if((inp_bit(PINC,ENT))&&flag1)
  {
	  flag1 = 0;  
	  btp   = 'E';	
  }
  
  else if((inp_bit(PINC,ESC))&&flag2)
  {
	  flag2 = 0;
	  btp   = 'C';	
  }
  
  else if((inp_bit(PINC,INC))&&flag3)
  {
	  flag3 = 0;
	  btp   = 'I';	
  }
  
  else if((inp_bit(PINC,DEC))&&flag4)
  {
	  flag4 = 0;
	  btp   = 'D';	
  }
} //end read_keyb


// ============================================================================
// --- boot_func ---
// Boot do sistema
void boot_func()
{
    
  disp_wr_po('I',0,3);
  disp_write('H');
  disp_write('M');
  disp_write(' ');
  disp_write('A');
  disp_write('V');
  disp_write('R');
  disp_wr_po('.',1,0);
  
  for(int i=0; i<16;i++)
  {
    disp_write('.');
	_delay_ms(125);
  } //end for
  
  disp_clear();
    
	
} //end boot_func



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


