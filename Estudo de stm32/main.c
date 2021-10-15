/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "adc.h"
#include "dma.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"
#include <stdio.h>
#include <string.h>
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
uint32_t AD[512];	//variável para armazenar os valores do AD
uint32_t line_sensor[2];
uint8_t rx_data[1] = {0}; //UART de 8 bits
uint8_t tx_data [100] = {0};
int f_button = 0;
/* USER CODE END 0 */
void f_timers(void);
/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_ADC1_Init();
  MX_USART1_UART_Init();
  MX_TIM2_Init();
  MX_TIM1_Init();
  HAL_TIM_Base_Start_IT(&htim2);
  HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
  HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_4);
  /* USER CODE BEGIN 2 */
	HAL_ADC_Start_DMA(&hadc1, AD, 512);	//start o ADC com DMA
	//esse valor é o buffer, onde os valores do AD são armazenados
	//2 são os valores que serão armazenados == número de canais do AD em uso ou mútiplos
	HAL_UART_Receive_DMA(&huart1, rx_data, 1);
	//HAL_UART_Transmit_DMA(&huart1, tx_data, 11);
	HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, 1);
   int f_ADC_Start = 0, flag = 0;
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
	while (1)
	{

		//UART com RX por DMA e TX por pooling
		HAL_UART_Receive_DMA(&huart1, rx_data, 1);
		// Caso tenha recebido um dado, inverte o estado dos LEDs.
		if(rx_data[0] != 0){
			switch(rx_data[0]){
				case '1':
					HAL_UART_Transmit(&huart1, rx_data, 1, 1);
					HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, 0);	//acende led da placa
					HAL_Delay(500);
					HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, 1);
					HAL_Delay(500);
					HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, 0);	//acende led da placa
					HAL_Delay(500);
					HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, 1);
					HAL_Delay(500);
					f_ADC_Start = 1;
					flag = 0;
					rx_data[0] = 0;                                               // Zera caractere recebido
					break;
				case '2':
					HAL_UART_Transmit(&huart1, rx_data, 1, 1);
					HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, 0);	//acende led da placa
					HAL_Delay(200);
					HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, 1);
					HAL_Delay(200);
					HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, 0);	//acende led da placa
					HAL_Delay(200);
					HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, 1);
					HAL_Delay(200);
					f_ADC_Start = 0;
					flag = 0;
					rx_data[0] = 0; 												 // Zera caractere recebido
					break;
				case '3':
					HAL_UART_Transmit(&huart1, rx_data, 1, 1);
					HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, 0);	//acende led da placa
					HAL_Delay(100);
					HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, 1);
					HAL_Delay(100);
					HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, 0);	//acende led da placa
					HAL_Delay(100);
					HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, 1);
					HAL_Delay(100);
					f_ADC_Start = 0;
					flag = 0;
					rx_data[0] = 0; 												 // Zera caractere recebido
				break;
			}

		}


		// Envia pela serial o caractere para limpar a tela do terminal
		if(!flag)
		{

			// Atualiza tela do terminal com os valores
			sprintf((char *)tx_data,
				"\nEstados dos LEDS:\n\r1 - Led Azul    : \n\r2 - Led Verde   : \n\r3 - Led Amarelo : \n\r");

			// Envia pela serial texto atualizado
			HAL_UART_Transmit(&huart1, tx_data, strlen((const char *)tx_data), 500);
			flag = 1;
		}


		if(f_ADC_Start)
		{
			for(int i = 0; i < 2; i++)
			{
				sprintf((char*)tx_data, "%d\t", (int)line_sensor[i]);
				HAL_UART_Transmit(&huart1, tx_data, 5, 2);
			}
			sprintf((char*)tx_data, "\n");
			HAL_UART_Transmit(&huart1, tx_data, 1, 1);
		}

		for(int i = 0; i < 10000 - 1; i+=100)
		{
			__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, i);	//PWM de 100Hz
			__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_4, i);
			HAL_Delay(1);
		}


		/*if(line_sensor[1] > 1023)
		{
		  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, 0);	//acende led da placa
		}

		else if(line_sensor[1] < 1023)
		{
		  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, 1);	//apaga led da placa
		}

		if(line_sensor[0] > 1023)
		{
		  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, 1);
		}

		else
		{
		  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, 0);
		}*/


    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	}
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */


//botões por interrupção
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
    int flag_bot = 0, flag_bot2 = 0;

    if(GPIO_Pin == Botao2_Pin)
    {
    	if(!flag_bot && HAL_GPIO_ReadPin(Botao2_GPIO_Port, Botao2_Pin))
    	{

    		HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
    		flag_bot = 1;
    	}
    	else if(flag_bot && !HAL_GPIO_ReadPin(Botao2_GPIO_Port, Botao2_Pin))
    	{

    		flag_bot = 0;
    	}
    }

    else if(GPIO_Pin == Botao_Pin)
    {
    	if(!flag_bot2 && HAL_GPIO_ReadPin(Botao_GPIO_Port, Botao_Pin))
		{

			HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
			flag_bot2 = 1;
		}
		else if(flag_bot2 && !HAL_GPIO_ReadPin(Botao_GPIO_Port, Botao_Pin))
		{

			flag_bot2 = 0;
		}
    }


    else;
}


void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc) {
	uint32_t val[2] = { 0 };

	for (int i = 0; i < 2; i++) {
	   val[i] = 0;
	}

	for (int i = 0; i < 512 / 2; i++) {
	   for (int j = 0; j < 2; j++) {
		   val[j] += AD[2*i + j];
	   }
	}

	for (int i = 0; i < 2; i++) {
	   val[i] /= 512 / 2;
	}

	for (int i = 0; i < 2; i++) {
	   line_sensor[i] = val[i];
	}
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	//esse if entra no timer2
    if (htim == &htim2)
    {
        f_timers();	//função com os timers de cada rotina
    }

	//e este também entra no timer2
	/*if (htim->Instance == TIM2)
	{
		//f_timers();
	}*/


}

void f_timers(void)
{
	static int count_1 = 0, count_1_ms = 0;

	if(count_1 < 10-1)
	{
		count_1++;
	}

	else
	{
		if(count_1_ms < 500 - 1)
		{
			count_1_ms++;
		}
		else
		{
			HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
			count_1_ms = 0;
		}
		count_1 = 0;
	}

}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC;
  PeriphClkInit.AdcClockSelection = RCC_ADCPCLK2_DIV6;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();

  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
