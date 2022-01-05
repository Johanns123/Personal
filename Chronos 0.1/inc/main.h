/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"

#include <stdio.h>            //Bilioteca do C
#include <stdbool.h>          //Biblioteca que permite utilizar vari�vel booleana
#include "PID.h"              //Biblioteca do controle PID
#include "PWM.h"			//biblioteca do PWM
#include "sensors.h"     //lógica utilizando os sensores
#include "dados.h"            //biblioteca que contém as funções atraladas ao envio de informações via UART
#include "motores.h"     //Biblioteca das funções de controle dos motores  //usado para ponte H tb6612fng

#define stm32f103c8t6

#ifdef atmega328p
#include "HAL_atmega328p.h"
#include "ADC.h"              //Biblioteca do conversor AD
#include "UART.h"             //Biblioteca da comunicação UART
#define NOP() __asm__ __volatile__ ("nop")
#endif


#ifdef stm32f103c8t6
#include "HAL_STM32f103c8t6.h"
#endif

/*Prot�tipo das fun��es*/
void setup();
void setup_Hardware();
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

void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define LED_Pin GPIO_PIN_13
#define LED_GPIO_Port GPIOC
#define QTR0_Pin GPIO_PIN_0
#define QTR0_GPIO_Port GPIOA
#define QTR1_Pin GPIO_PIN_1
#define QTR1_GPIO_Port GPIOA
#define QTR2_Pin GPIO_PIN_2
#define QTR2_GPIO_Port GPIOA
#define QTR3_Pin GPIO_PIN_3
#define QTR3_GPIO_Port GPIOA
#define QTR4_Pin GPIO_PIN_4
#define QTR4_GPIO_Port GPIOA
#define QTR5_Pin GPIO_PIN_5
#define QTR5_GPIO_Port GPIOA
#define QTR6_Pin GPIO_PIN_6
#define QTR6_GPIO_Port GPIOA
#define Bat_voltage_Pin GPIO_PIN_7
#define Bat_voltage_GPIO_Port GPIOA
#define SW_Pin GPIO_PIN_1
#define SW_GPIO_Port GPIOB
#define Encoder_D1_Pin GPIO_PIN_10
#define Encoder_D1_GPIO_Port GPIOB
#define Encoder_D1_EXTI_IRQn EXTI15_10_IRQn
#define Encoder_D2_Pin GPIO_PIN_11
#define Encoder_D2_GPIO_Port GPIOB
#define AIN2_Pin GPIO_PIN_12
#define AIN2_GPIO_Port GPIOB
#define AIN1_Pin GPIO_PIN_13
#define AIN1_GPIO_Port GPIOB
#define BIN2_Pin GPIO_PIN_14
#define BIN2_GPIO_Port GPIOB
#define BIN1_Pin GPIO_PIN_15
#define BIN1_GPIO_Port GPIOB
#define PWMA_Pin GPIO_PIN_8
#define PWMA_GPIO_Port GPIOA
#define PWMB_Pin GPIO_PIN_9
#define PWMB_GPIO_Port GPIOA
#define Buzzer_Pin GPIO_PIN_10
#define Buzzer_GPIO_Port GPIOA
#define Curva_Pin GPIO_PIN_11
#define Curva_GPIO_Port GPIOA
#define Parada_Pin GPIO_PIN_12
#define Parada_GPIO_Port GPIOA
#define Button_Pin GPIO_PIN_15
#define Button_GPIO_Port GPIOA
#define Encoder_E1_Pin GPIO_PIN_3
#define Encoder_E1_GPIO_Port GPIOB
#define Encoder_E2_Pin GPIO_PIN_4
#define Encoder_E2_GPIO_Port GPIOB
#define INT_MPU_Pin GPIO_PIN_5
#define INT_MPU_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
