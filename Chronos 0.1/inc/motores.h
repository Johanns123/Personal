#include "HAL_STM32f103c8t6.h"

//Lado direito
#define AIN1 13	 // Quando em HIGH, roda direita anda para frente
#define AIN2 12

//Lado Esquerdo
#define BIN1 15 // Quando em HIGH, roda esquerda anda para frente
#define BIN2 14

void motores_frente();
void motores_direita_frente();
void motores_esquerda_frente();
void motores_giro_esquerda();
void motores_giro_direita();
void motores_direita_tras();
void motores_esquerda_tras();
void motores_tras();
void motores_off();
void motores_freio();
