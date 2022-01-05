#include "motores.h"



void motores_frente() 
{

    set_bit(GPIOA->ODR, AIN1); //frente direita
    clr_bit(GPIOA->ODR, AIN2);
    set_bit(GPIOA->ODR, BIN1); //frente esquerda
    clr_bit(GPIOA->ODR, BIN2);
}

void motores_direita_frente()  //direita sentido direto
{
    set_bit(GPIOA->ODR, AIN1); //frente direita
	clr_bit(GPIOA->ODR, AIN2);
	clr_bit(GPIOA->ODR, BIN1); //frente esquerda
	clr_bit(GPIOA->ODR, BIN2);
}

void motores_esquerda_frente()  //direita sentido direto
{
	clr_bit(GPIOA->ODR, AIN1); //frente direita
	clr_bit(GPIOA->ODR, AIN2);
	set_bit(GPIOA->ODR, BIN1); //frente esquerda
	clr_bit(GPIOA->ODR, BIN2);
}

void motores_giro_esquerda()
{
	set_bit(GPIOA->ODR, AIN1); //frente direita
	clr_bit(GPIOA->ODR, AIN2);
	clr_bit(GPIOA->ODR, BIN1); //frente esquerda
	set_bit(GPIOA->ODR, BIN2);
}

void motores_giro_direita()
{
	clr_bit(GPIOA->ODR, AIN1); //frente direita
	set_bit(GPIOA->ODR, AIN2);
	set_bit(GPIOA->ODR, BIN1); //frente esquerda
	clr_bit(GPIOA->ODR, BIN2);
}

void motores_direita_tras()  //direita sentido reverso
{
	clr_bit(GPIOA->ODR, AIN1); //frente direita
	set_bit(GPIOA->ODR, AIN2);
	clr_bit(GPIOA->ODR, BIN1); //frente esquerda
	clr_bit(GPIOA->ODR, BIN2);
}


void motores_esquerda_tras()  //esquerda sentido reverso
{
	clr_bit(GPIOA->ODR, AIN1); //frente direita
	clr_bit(GPIOA->ODR, AIN2);
	clr_bit(GPIOA->ODR, BIN1); //frente esquerda
	set_bit(GPIOA->ODR, BIN2);
}

void motores_tras()
{
	clr_bit(GPIOA->ODR, AIN1); //frente direita
	set_bit(GPIOA->ODR, AIN2);
	clr_bit(GPIOA->ODR, BIN1); //frente esquerda
	set_bit(GPIOA->ODR, BIN2);

}

void motores_off()
{
	clr_bit(GPIOA->ODR, AIN1); //frente direita
	clr_bit(GPIOA->ODR, AIN2);
	clr_bit(GPIOA->ODR, BIN1); //frente esquerda
	clr_bit(GPIOA->ODR, BIN2);
}

void motores_freio()
{
    motores_off();        //desliga os motores para deoxar o pr�prio atrito frear o rob�
    __HAL_TIM_SET_COMPARE(&htim1, PWMA_Pin, 0);	//PWM de 100Hz, variável varia de 0 a 10000
    __HAL_TIM_SET_COMPARE(&htim1, PWMB_Pin, 0);
}
