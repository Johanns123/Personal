#include <stdio.h>            //Bilioteca do C
#include <stdbool.h>          //Biblioteca que permite utilizar vari�vel booleana
#include "PID.h"              //Biblioteca do controle PID
#include "sensors.h"     //lógica utilizando os sensores
#include "dados.h"            //biblioteca que contém as funções atraladas ao envio de informações via UART

#define atmega328p
#ifdef atmega328p
#include "HAL_atmega328p.h"
#include "ADC.h"              //Biblioteca do conversor AD
#include "UART.h"             //Biblioteca da comunicação UART
#include "PWM.h"      //Biblioteca de PWM fast mode de 10 bits
#include "motores.h"     //Biblioteca das funções de controle dos motores  //usado para ponte H tb6612fng
#define NOP() __asm__ __volatile__ ("nop")
#endif


/*Prot�tipo das fun��es*/
void setup();
void setup_logica();        //vari�veis utilizadas na l�gica
void loop();
void estrategia();          //estrategia do robo
//---------------------------------------------------------------//
void parada();              //Leitura dos sensores laterais
void fim_de_pista();        //verifica se � o fim da pista 
//---------------------------------------------------------------------//
void count_pulsesE();
void count_pulsesD();
void millis(void);
//---------------------------------------------------------------------//
void f_timers (void);       //fun��o de temporiza��o das rotinas
void f_timer1(void);
void f_timer2(void);
void f_timer3(void);
void f_timer4(void);
void f_timer5(void);
/*===========================================================================*/