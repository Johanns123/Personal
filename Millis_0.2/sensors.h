#include "HAL_atmega328p.h"
#include <stdbool.h>

/* Macros */
#define PWM_RETURN      80
#define NOP() __asm__ __volatile__ ("nop")

void sensors_ADC_maq();
void sensors_laterais(void);
void sensors_sentido_de_giro();
void sensors_frontais(int *erro_sensores, int *speedW, int *speedX, unsigned int *PWM_general, unsigned int *PWMR, unsigned int *PWM_Curva);
//void sensors_volta_pra_pista(void);

// ================================================================================================================================================

/* 
 * Comentarios
 * 
 * sensors_leitura_de_pista <- sensors_frontais
 * 
 * sensors_le_marcadores    <- sensors_laterais
 *  
 */

/*  ================================
 *  TABELA VERDADE SENSORES LATERAIS
 *  ================================
 * 
 *  00 - CRUZAMENTO
 *  01 - CURVA
 *  10 - PARADA
 *  11 - NAO LE MARCADOR
 * 
 *  ================================
 *  TABELA VERDADE SENSORES FRONTAIS
 *  ================================
 *  
 *  00000 -
 *  00001 -
 *  00010 - 
 *  00011 - 
 *  00100 -
 *  00101 -
 *  00110 -
 *  00111 - 
 *  01000 - 
 *  01001 -
 *  01010 -
 *  01011 -
 *  01100 - 
 *  01101 -
 *  01110 -
 *  01111 -
 *  10000 -
 *  10001 - 
 *  10010 - 
 *  10011 -
 *  10100 -
 *  10101 -
 *  10110 -
 *  10111 -
 *  11000 -
 *  11001 -
 *  11010 -
 *  11011 -
 *  11100 -
 *  11101 - 
 *  11110 -
 *  11111 -
 * 
 */