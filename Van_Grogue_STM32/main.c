/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "fatfs.h"
#include <stdio.h>
#include <string.h>
#include "PID.h"
//#include "fatfs_sd.h"	//biblioteca de manipulação do cartão sd
#include "Driver_motor.h"

/* Private variables ---------------------------------------------------------*/

SPI_HandleTypeDef hspi1;

TIM_HandleTypeDef htim1;
TIM_HandleTypeDef htim2;

UART_HandleTypeDef huart1;
DMA_HandleTypeDef hdma_usart1_rx;
DMA_HandleTypeDef hdma_usart1_tx;

ADC_HandleTypeDef hadc1;
DMA_HandleTypeDef hdma_adc1;


/*Variáveis globais*/
unsigned char curva1 = 0, curva2;	//variáveis usadas no cálculo do raio de cada curva
int erro = 0;      //variável para cáculo do erro da direção do robô em cima da linha
unsigned int PWMA = 0, PWMB = 0; // Modulação de largura de pulso enviada pelo PID
unsigned int PWMA_C = 0, PWMB_C = 0; //PWM de curva com ajuste do PID;
uint32_t sensores_frontais[6];
uint32_t sensores_laterais[2];


//variáveis de controle
char f_parada= 0;   //variável que comanda quando o robô deve parar e não realizar mais sua rotina
char f_calibra = 0; //variável que indica o fim da calibração dos sensores e inicio da estratégia
char flag = 0;      //variável de controle para identificar o momento de parada

unsigned char pulse_numberD = 0, pulse_numberE = 0; //variáveis para contagem dos pulsos dos encoders

/*Vairáveis da UART*/
uint8_t rx_data[1] = {0}; //UART de 8 bits
uint8_t tx_data [100] = {0};

int f_button = 0;//botão de inicialização da rotina, ainda a ser implementado

/*Variáveis de manipulação de arquivos*/
/*FATFS 		 fs;  //file system
FIL 		fil;  //file
FRESULT fresult;  //to store the result
char buffer[1024];//to store data

UINT br, bw;	  //file read/write count*/

/*Capacity related variables*/
/*FATFS 		 *pfs;
DWORD	fre_clust;
uint32_t total, free_space;*/

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_SPI1_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_TIM1_Init(void);
static void MX_TIM2_Init(void);
static void MX_ADC1_Init(void);
/***********Protótipo das funções************/
void parada();              //Leitura dos sensores laterais
void setup();
void setup_Hardware();      //define os registradores
void setup_logica();
void loop();
void sentido_de_giro();     //lê os sensores frontais e determina o sentido de giro dos motores com o PID
void PWM_limit();           //limita o PWM em 1000 caso a variável passe de 1023
void estrategia();          //estrategia do robô
void calibration();         //contêm toda a rotina de calibração
void fim_de_pista();        //verifica se é o fim da psita
void f_timers (void);       //função de temporização das rotinas
void frente();
void tras();
void motor_off();
void freio();
void count_pulsesD();
void count_pulsesE();
float calculo_do_raio();
void entrou_na_curva();

/*===========================================================================*/

int main(void)
{
	setup();
	while (1) loop();

}

void setup(void)
{
	setup_Hardware();   //setup das IO's e das interrupções
	calibration();      //rotina de calibração
	setup_logica();     //definição das variáveis lógicas(vazio por enquanto)
}//end setup

void setup_Hardware()
{
	/* MCU Configuration--------------------------------------------------------*/

	/* Reset of all peripherals, Initializes the Flash interface and the Systick. */
	HAL_Init();

	/* Configure the system clock */
	SystemClock_Config();

	/* Initialize all configured peripherals */
	MX_GPIO_Init();
	MX_DMA_Init();
	//MX_SPI1_Init();
	MX_USART1_UART_Init();
	MX_FATFS_Init();
	MX_TIM1_Init();
	MX_TIM2_Init();
	MX_ADC1_Init();
	HAL_TIM_Base_Start_IT(&htim2);
	HAL_TIM_PWM_Start(&htim1, PWMA_Pin);
	HAL_TIM_PWM_Start(&htim1, PWMB_Pin);
	//esse valor é o buffer, onde os valores do AD são armazenados
	//2 são os valores que serão armazenados == número de canais do AD em uso ou mútiplos
	HAL_UART_Receive_DMA(&huart1, rx_data, 1);//inicializo o RX em modo DMA
	HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, 1);//inicializo o led desligado
	//GPIOA->ODR = 0x00000000;
}

void calibration()
{
     //----> Calibração dos Sensores frontais <----//
    HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, 0);

    HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, 1);
    HAL_Delay(500);
    HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, 0); //subrotina de acender e apagar o LED 13
    HAL_Delay(1000);
    HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, 1);
    HAL_Delay(500);
    HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, 0);
    HAL_Delay(500);
    HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, 1);
    HAL_Delay(2000);
    f_calibra = 1;  //flag para indicar fim da calibração
}//end calibration

void setup_logica()
{

}//end setup_logica


void loop()//loop vazio
{

}

void parada()
{
    //leitura de marcador de curva

    //cruzamento
    //branco = 0, preto = 1
    if (sensores_laterais[0] < 800 && sensores_laterais[1] < 800) //verifica se é cruzamento
    {

        flag = 0;
    }

    //leitura de marcador de parada
    else if (sensores_laterais[0] > 1000 && sensores_laterais[1] < 800)	//precisa de testes por ser analógico
    {
        flag = 1;
    }

}


void sentido_de_giro()
{
    //-----> Área do senstido de giro
    unsigned int u = 0; //valor de retorno do PID
    static unsigned int PWMR = 400; // valor da força do motor em linha reta
    unsigned int u_curva = 0; //valor de retorno do PID numa curva
    static unsigned int PWM_Curva = 350; //PWM ao entrar na curva

    if ((sensores_frontais[0] < 800 && sensores_frontais[5] > 3600) || (sensores_frontais[0]  > 225 && sensores_frontais[5] < 3600))
        //se o primeiro sensor ou o último sensor estiverem lendo branco...
        //necessário teste com monitor serial
        //estudar a melhor quantidade de sensores e seu espaçamento
    {
        u_curva = PID(erro);
        PWMA_C = PWM_Curva - u_curva;
        PWMB_C = PWM_Curva + u_curva;
        frente();
        PWM_limit();
        __HAL_TIM_SET_COMPARE(&htim1, PWMA_Pin, PWMA_C);	//PWM de 100Hz, variável varia de 0 a 10000
        __HAL_TIM_SET_COMPARE(&htim1, PWMB_Pin, PWMB_C);
    } //em cima da linha

    else
    {
        //pra frente - reta
        //flag1 = 0x00;
        //--------------->AREA DO PID<---------------

        u = PID(erro);

        PWMA = PWMR - u;
        PWMB = PWMR + u;

        frente();
        PWM_limit();
        __HAL_TIM_SET_COMPARE(&htim1, PWMA_Pin, PWMA);	//PWM de 100Hz, variável varia de 0 a 10000
        __HAL_TIM_SET_COMPARE(&htim1, PWMB_Pin, PWMB);
    }

    //A função que fazia o robô rodar em seu próprio eixo foi removida

}

void PWM_limit() {
    //------> Limitando PWM

    if (PWMA > 9999)//PWM de 16 bits limitado até 10000
    {
        PWMA = 9800;
    }

    if (PWMA_C > 9999)
    {
        PWMA_C = 9800;
    }

    if (PWMB_C > 9999)
    {
        PWMB_C = 9800;
    }
    if (PWMB > 9999)
    {
        PWMB = 9800;
    }
}

void calculo_do_erro()
{
    unsigned int soma_direito = 0, denominador_direito = 6, denominador_esquerdo = 6;
    int soma_esquerdo = 0;
    int soma_total = 0;   //caso aumente o peso da média_ponderada, tomar cuidado com a variável char

    static char peso [] = {-3, -2, -1, 1, 2, 3}; //utilizando um prescale de 2000
    //os pesos precisarão ser corrigidos pois os sensores do Van Grogue estão um pouco assimétricos

    for (int j = 0; j < 3; j++)
    {
        soma_esquerdo += (sensores_frontais[j] * peso[j]);
        soma_direito += (sensores_frontais[5-j] * peso[5 - j]);
    }

    soma_total = (soma_esquerdo + soma_direito) / (denominador_esquerdo + denominador_direito);

    erro = 0 - soma_total;   //valor esperado(estar sempre em cima da linha) - valor medido
}



void count_pulsesD()
{
    static int Encoder_C1Last = 0, direction_m;

    int Lstate = HAL_GPIO_ReadPin(Encoder_D1_GPIO_Port, Encoder_D1_Pin);//variável de leitura de um dos pinos do encoderD

    if (!Encoder_C1Last && Lstate) { //Verifica se Encoder_C1Last é falso e Lstate é verdadeiro
        int val = HAL_GPIO_ReadPin(Encoder_D2_GPIO_Port, Encoder_D2_Pin);; //Variável de leitura do segundo pino do encoderD

        if (!val && direction_m) direction_m = 0; //sentido horário

        else if (val && !direction_m) direction_m = 1; //sentido anti-horário
    }

    Encoder_C1Last = Lstate;

    if (!direction_m) pulse_numberD++; //sentido horário
    else pulse_numberD--;



}

void count_pulsesE()
{
    static int Encoder_C1Last = 0, direction_m;

    int Lstate = HAL_GPIO_ReadPin(Encoder_E1_GPIO_Port, Encoder_E1_Pin);; //variável de leitura de um dos pinos do encoderD

    if (!Encoder_C1Last && Lstate)
    { //Verifica se Encoder_C1Last é falso e Lstate é verdadeiro
        int val = HAL_GPIO_ReadPin(Encoder_E2_GPIO_Port, Encoder_E2_Pin);//Variável de leitura do segundo pino do encoderD

        if (!val && direction_m) direction_m = 0; //sentido horário

        else if (val && !direction_m) direction_m = 1; //sentido anti-horário
    }

    Encoder_C1Last = Lstate;

    if (!direction_m) pulse_numberD++; //sentido horário
    else pulse_numberD--; //sentido anti-horário


}

void estrategia()
{

    if (!f_parada)  //se f_parada for 0...
    {
        calculo_do_erro();      //faz a média ponderada e calcula o erro
        entrou_na_curva();		//verifica o fim de uma reta ou de uma curva
        sentido_de_giro();      //Verifica se precisa fazer uma curva e o cálculo do PID
    }

}

void fim_de_pista()
{
    static char parada = 0;

    if(flag)
    {
       parada++;
       flag = 0;
    }

    if(parada > 1)  //dois marcadores de parada
    {
        freio();
        f_parada = 1;
        parada = 0;
    }
}

void f_timers (void) {

    static char c_timer1 = 0;
    static unsigned char c_timer2 = 0;
    if(f_calibra)
    {

        //funções a cada 100us
        if(c_timer1 < 1-1)
        {
            c_timer1++;
        }

        else
        {
            parada();
            fim_de_pista();         //Verifica se é o fim da pista
            c_timer1 = 0;
        }

        if (c_timer2 < 4-1)   //o 0 conta na contagem -> 4-1
        {
            c_timer2++; //100us -1; 200us-2;300 us-3;
        }

        else    //a cada 400us
        {
            estrategia();
            c_timer2=0;
        }
    }
}//fim do programa


//=========Funções visíveis ao usuário===========//
//reavaliar esta função devido à pista possuir um "S" e somente três marcadores de curva

void entrou_na_curva()
{
    static float dist_reta1 = 0, dist_reta2 = 0;
    static unsigned int entrou = 0;
    static float reta_med = 0;

    if (sensores_laterais[0] < 800 && sensores_laterais[1] > 1000)
        //li branco no sensor de curva e li preto no sensor de parada
    {
        switch (entrou) {
            case 0: //entrou na curva
				dist_reta1 = pulse_numberD * 0.812;
				dist_reta2 = pulse_numberE * 0.812;
				if (!dist_reta1 && !dist_reta2); //nao printa;
				else
				{
					reta_med = (dist_reta1+dist_reta2)/2;
					//sprintf((char *)tx_data, "RetaD %.2f\n", reta_med); //Converte para string
					//HAL_UART_Transmit(&huart1, tx_data, strlen((const char *)tx_data), 500);
					reta_med = 0x00;
				} //printa na serial a distancia;
				pulse_numberD = 0x00;
				dist_reta1 = 0x00;
				pulse_numberE = 0x00;
				dist_reta2 = 0x00; //zera as variáveis de cálculo da distância
                entrou = 1;
                break;

            case 1:
				curva1 = pulse_numberD * 0.812; //converte o número de pulsos em mm
				curva2 = pulse_numberE * 0.812;
				calculo_do_raio();
				curva1 = 0x00;
				pulse_numberD = 0x00;
				curva2 = 0x00;
				pulse_numberE = 0x00; //zera as variáveis de cálculo do raio da curva

				if ((sensores_frontais[0] < 200 && sensores_frontais[5] > 900) || (sensores_frontais[0] > 900 && sensores_frontais[5] < 200))
				{//se o primeiro sensor ou o último sensor estiverem lendo branco...

					entrou = 1;
				}
				else{
					entrou = 0;
				}

				break;
        }
    }
}

//-u _printf_float

float calculo_do_raio() //esta função calcula o raio a partir da disância percorrida pelas duas rodas do robô
{
    static float raio = 0;
    unsigned int diametro = 126; //126mm, diâmetro sas rodas
    if (!(curva1) && !(curva2)); //não calcula o raio se não mediu a curva
    else if (curva1 != curva2)
    {
        raio = (diametro / 2) * ((curva1 + curva2) / (curva1 - curva2)); //Cálculo do raio em módulo
        if(raio < 0)	raio *= -1;	//normaliza para números positivos
        //sprintf((char*) tx_data, "Raio %.2f\n", raio); //Converte para string	//se o float não funcionar, usar tec. de cent, dez e uni
        //HAL_UART_Transmit(&huart1, tx_data, strlen((const char *)tx_data), 500);

    }
    return raio;
}

/*Atrelar às funções quando guardar e imprimir o valor da distância das retas e a distância das curvas*/


/*******************Funções das interrupções*******************/

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	//esse if entra no timer2
    if (htim == &htim2)
    {
        f_timers();	//função com os timers de cada rotina
    }

}

//botões por interrupção
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {

    if(GPIO_Pin == Encoder_D1_Pin)
    {
    	count_pulsesD();
    }

    else if(GPIO_Pin == Encoder_E1_Pin)
    {
    	count_pulsesE();
    }

    else;
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

/**
  * @brief ADC1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADC1_Init(void)
{

  /* USER CODE BEGIN ADC1_Init 0 */

  /* USER CODE END ADC1_Init 0 */

  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC1_Init 1 */

  /* USER CODE END ADC1_Init 1 */
  /** Common config
  */
  hadc1.Instance = ADC1;
  hadc1.Init.ScanConvMode = ADC_SCAN_DISABLE;
  hadc1.Init.ContinuousConvMode = ENABLE;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.NbrOfConversion = 1;
  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_7;
  sConfig.Rank = ADC_REGULAR_RANK_1;
  sConfig.SamplingTime = ADC_SAMPLETIME_55CYCLES_5;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC1_Init 2 */

  /* USER CODE END ADC1_Init 2 */

}

/**
  * @brief SPI1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI1_Init(void)
{

  /* USER CODE BEGIN SPI1_Init 0 */

  /* USER CODE END SPI1_Init 0 */

  /* USER CODE BEGIN SPI1_Init 1 */

  /* USER CODE END SPI1_Init 1 */
  /* SPI1 parameter configuration*/
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_8;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI1_Init 2 */

  /* USER CODE END SPI1_Init 2 */

}

/**
  * @brief TIM1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM1_Init(void)
{

  /* USER CODE BEGIN TIM1_Init 0 */

  /* USER CODE END TIM1_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};
  TIM_BreakDeadTimeConfigTypeDef sBreakDeadTimeConfig = {0};

  /* USER CODE BEGIN TIM1_Init 1 */

  /* USER CODE END TIM1_Init 1 */
  htim1.Instance = TIM1;
  htim1.Init.Prescaler = 72-1;
  htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim1.Init.Period = 10000-1;
  htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim1.Init.RepetitionCounter = 0;
  htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
  if (HAL_TIM_Base_Init(&htim1) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim1, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_Init(&htim1) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCNPolarity = TIM_OCNPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_ENABLE;
  sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
  sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;
  if (HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_2) != HAL_OK)
  {
    Error_Handler();
  }
  sBreakDeadTimeConfig.OffStateRunMode = TIM_OSSR_DISABLE;
  sBreakDeadTimeConfig.OffStateIDLEMode = TIM_OSSI_DISABLE;
  sBreakDeadTimeConfig.LockLevel = TIM_LOCKLEVEL_OFF;
  sBreakDeadTimeConfig.DeadTime = 0;
  sBreakDeadTimeConfig.BreakState = TIM_BREAK_DISABLE;
  sBreakDeadTimeConfig.BreakPolarity = TIM_BREAKPOLARITY_HIGH;
  sBreakDeadTimeConfig.AutomaticOutput = TIM_AUTOMATICOUTPUT_DISABLE;
  if (HAL_TIMEx_ConfigBreakDeadTime(&htim1, &sBreakDeadTimeConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM1_Init 2 */

  /* USER CODE END TIM1_Init 2 */
  HAL_TIM_MspPostInit(&htim1);

}

/**
  * @brief TIM2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM2_Init(void)
{

  /* USER CODE BEGIN TIM2_Init 0 */

  /* USER CODE END TIM2_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM2_Init 1 */

  /* USER CODE END TIM2_Init 1 */
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 72-1;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 100-1;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
  if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM2_Init 2 */

  /* USER CODE END TIM2_Init 2 */

}

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 57600;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

}

/**
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void)
{

  /* DMA controller clock enable */
  __HAL_RCC_DMA1_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA1_Channel1_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel1_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel1_IRQn);
  /* DMA1_Channel4_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel4_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel4_IRQn);
  /* DMA1_Channel5_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel5_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel5_IRQn);

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, CS_Pin|AIN2_Pin|AIN1_Pin|BIN2_Pin
                          |BIN1_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(Buzzer_GPIO_Port, Buzzer_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : LED_Pin */
  GPIO_InitStruct.Pin = LED_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LED_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : QTR0_Pin QTR1_Pin QTR2_Pin QTR3_Pin
                           QTR4_Pin QTR5_Pin QTR6_Pin Curva_Pin
                           Parada_Pin */
  GPIO_InitStruct.Pin = QTR0_Pin|QTR1_Pin|QTR2_Pin|QTR3_Pin
                          |QTR4_Pin|QTR5_Pin|QTR6_Pin|Curva_Pin
                          |Parada_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : CS_Pin AIN2_Pin AIN1_Pin BIN2_Pin
                           BIN1_Pin */
  GPIO_InitStruct.Pin = CS_Pin|AIN2_Pin|AIN1_Pin|BIN2_Pin
                          |BIN1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : SW_Pin Encoder_D2_Pin Encoder_E2_Pin */
  GPIO_InitStruct.Pin = SW_Pin|Encoder_D2_Pin|Encoder_E2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : Encoder_D1_Pin Encoder_E1_Pin */
  GPIO_InitStruct.Pin = Encoder_D1_Pin|Encoder_E1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : Buzzer_Pin */
  GPIO_InitStruct.Pin = Buzzer_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(Buzzer_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : Button_Pin */
  GPIO_InitStruct.Pin = Button_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(Button_GPIO_Port, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI9_5_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);

  HAL_NVIC_SetPriority(EXTI15_10_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);

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
