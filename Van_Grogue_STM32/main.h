/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
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

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define LED_Pin GPIO_PIN_13
#define LED_GPIO_Port GPIOC
#define CS_Pin GPIO_PIN_0
#define CS_GPIO_Port GPIOB
#define Button_Pin GPIO_PIN_2
#define Button_GPIO_Port GPIOB
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
#define Buzzer_Pin GPIO_PIN_10
#define Buzzer_GPIO_Port GPIOA
#define B_multiplexer_Pin GPIO_PIN_11
#define B_multiplexer_GPIO_Port GPIOA
#define A_multiplexer_Pin GPIO_PIN_12
#define A_multiplexer_GPIO_Port GPIOA
#define Switch_Pin GPIO_PIN_15
#define Switch_GPIO_Port GPIOA
#define Encoder_E1_Pin GPIO_PIN_8
#define Encoder_E1_GPIO_Port GPIOB
#define Encoder_E1_EXTI_IRQn EXTI9_5_IRQn
#define Encoder_E2_Pin GPIO_PIN_9
#define Encoder_E2_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
