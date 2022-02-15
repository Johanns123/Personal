/*Bibliotecas e frequencia do uc*/
#define F_CPU 16000000        //define a frequencia do uC para 16MHz
#include <avr/io.h>           //Biblioteca geral dos AVR
#include <avr/interrupt.h>    //Biblioteca de interrupcao
#include <util/delay.h>
#include "ADC.h"
#include "PWM.h"
#include "UART.h"


//variaveis de comando para os registradores
#define set_bit(y,bit) (y|=(1<<bit)) //coloca em 1 o bit x da variavel Y
#define clr_bit(y,bit) (y&=~(1<<bit)) //coloca em 0 o bit x da variavel Y
#define cpl_bit(y,bit) (y^=(1<<bit)) //troca o estado logico do bit x da variavel Y
#define tst_bit(y,bit) (y&(1<<bit)) //retorna 0 ou 1 conforme leitura do bit
/*==============================================================*/

/*Mapeamento de Hardware*/
#define sensor_de_curva          PB3
#define sensor_de_parada         PB4
#define led_placa                PB5
#define led_offboard             PC5
#define leitura_sensores        PINB
#define encoder_C1E              PB0
#define encoder_C2E              PB5
#define encoder_C1D              PD2
#define encoder_C2D              PD7
#define leitura_outros_sensores PIND

void setup_Hardware();
void INT_INIT();
void EXT_INIT();
void enable_interrupts(void);
void disable_interrupts(void);