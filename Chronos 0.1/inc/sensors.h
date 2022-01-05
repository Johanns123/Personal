#include "HAL_STM32f103c8t6.h"
#include <stdbool.h>

/*Macros*/
#define PWM_RETURN      80
//#define NOP() __asm__ __volatile__ ("nop")

#define LED	13
#define sensor_de_curva 11
#define sensor_de_parada 12

#define leitura_sensores GPIOA->IDR

void sensors_ADC_maq();
void sensors_le_marcadores(void);
void sensors_sentido_de_giro();
void sensors_leitura_de_pista(int *erro_sensores, int *speedW, int *speedX, unsigned int *PWM_general, unsigned int *PWMR, unsigned int *PWM_Curva);
