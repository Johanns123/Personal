#include <stdio.h>            //Bilioteca do C
#include <stdbool.h>          //Biblioteca que permite utilizar variavel booleana
#include "UART.h"             //Biblioteca da comunicacao UART
#include "ADC.h"              //Biblioteca do conversor AD
#include "PWM.h"              //Biblioteca de PWM fast mode de 10 bits
#include "motores.h"          //Biblioteca das funcoes de controle dos motores  //usado para ponte H tb6612fng
#include "PID.h"              //Biblioteca do controle PID
#include "sensors.h"          //Logica utilizando os sensores
#include "dados.h"            //Biblioteca que contem as funcoes atreladas ao envio de informacoes via UART
#include "HAL_atmega328p.h"   //Biblioteca que configura os registradores e portas do uC ATMEGA328P
#include "Running.h"

/*Prototipo das funcoes*/
void setup();
void setup_variaveis();        //variaveis utilizadas na logica
void loop();
void tomada_de_tempo();     //estrategia de tomada de tempo
void mapeamento();          //estrategia de mapeamento
//---------------------------------------------------------------------//
void parada();              //Leitura dos sensores laterais
void fim_de_pista();        //verifica se e o fim da pista 
//---------------------------------------------------------------------//
void count_pulsesE();
void count_pulsesD();
void millis(void);
//---------------------------------------------------------------------//
void f_timers (void);       //funcao de temporizacao das rotinas
void f_timer1(void);
void f_timer2(void);
void f_timer3(void);
void f_timer4(void);
void f_timer5(void);
void sub_timer(void);
/*===========================================================================*/

/*Macros*/
#define NOP() __asm__ __volatile__ ("nop")
