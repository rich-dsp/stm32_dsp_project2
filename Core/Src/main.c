/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <math.h>
#include <stdio.h>
#include "utils.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

#define MAX_STIM_LENGTH 1024U
#define DAC_TSEL_TIM6_TRGO 0U

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
DAC_HandleTypeDef hdac1;
DMA_HandleTypeDef hdma_dac1_ch1;
DMA_HandleTypeDef hdma_dac1_ch2;

TIM_HandleTypeDef htim2;

UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */


/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_DAC1_Init(void);
static void MX_TIM2_Init(void);
/* USER CODE BEGIN PFP */


/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
float Stim[MAX_STIM_LENGTH] = {0.0f};


ii_filter_t G_FilterDb;


uint32_t Filt_in = 0;
uint32_t Filt_out = 0;




//uint16_t stim_index = 0;
//uint16_t filt_y_prev = 0;
//uint16_t stim_length = MAX_STIM_LENGTH; // Length of the stimulus array
//float f_norm = 0.05f; // Normalized frequency for the stimulus
//float k = 0.05f; // Filter coefficient for the low-pass filter




uint32_t Stim_Init_sin(float f_norm, uint32_t n_cycles)
{
  const float two_pi = 6.28318530718f;
  uint32_t stim_length;
  uint32_t index = 0;
  float phase = 0.0f;
  uint32_t period = (uint32_t)(1.0f / f_norm);

  stim_length = (uint32_t)(n_cycles * (1.0f / f_norm));
  if (stim_length > MAX_STIM_LENGTH)
  {
    stim_length = MAX_STIM_LENGTH;
  }

  for (index = 0; index < stim_length; index++)
  {
    phase = two_pi * (float)(index % period) / (float)period;
    Stim[index] = sinf(phase);
//    Stim[index] = 0;
  }
  printf("\n\r%ld Stimuli initialized\n\r", stim_length);
  return stim_length;
}

uint32_t Stim_Init_sqr(float f_norm, uint32_t n_cycles)
{
  const float two_pi = 6.28318530718f;
  uint32_t stim_length;
  uint32_t index = 0;
  float phase = 0.0f;
  uint32_t period = (uint32_t)(1.0f / f_norm);

  stim_length = (uint32_t)(n_cycles * (1.0f / f_norm));
  if (stim_length > MAX_STIM_LENGTH)
  {
    stim_length = MAX_STIM_LENGTH;
  }

  for (index = 0; index < stim_length; index++)
  {
    Stim[index] = (index % period) < (period / 2) ? -1.0f : 1.0f; // Square wave
  }
  printf("\n\r%ld Stimuli initialized\n\r", stim_length);
  return stim_length; 
}


float get_filter_value(float x_in, float y_prev)
{
//  const float k = 0.05f; // Filter coefficient
//  float alpha = exp(-k); // Calculate the exponential decay factor
//  float x_f = (float)((x_in) / 2048.0f - 1.0f); // Current input sample
//  float y_prev_float = (float)((y_prev) / 2048.0f - 1.0f); // Previous output sample

  float y = G_FilterDb.alpha * G_FilterDb.filt_y_prev + (1.0f - G_FilterDb.alpha) * x_in; // Apply the low-pass filter
  G_FilterDb.filt_y_prev = y; // Update the previous output sample
  return y;
}


void handle_dacs(void)
{
  
  //HAL_GPIO_WritePin(DEBUG_0_GPIO_Port, DEBUG_0_Pin, GPIO_PIN_SET);
  
  handle_dac1(G_FilterDb.stim_index);
  handle_dac2(G_FilterDb.stim_index);
  G_FilterDb.stim_index = (G_FilterDb.stim_index + 1) % G_FilterDb.stim_length; // Increment stim_index and wrap around if necessary
  //HAL_GPIO_WritePin(DEBUG_0_GPIO_Port, DEBUG_0_Pin, GPIO_PIN_RESET);

}

void handle_dac2(uint32_t stim_index)
{
  // Implementation for handling DAC2

  float y = get_filter_value(Stim[stim_index], G_FilterDb.filt_y_prev);
//  HAL_GPIO_TogglePin(DEBUG_0_GPIO_Port, DEBUG_0_Pin);
  //HAL_DAC_SetValue(&hdac1, DAC_CHANNEL_2, DAC_ALIGN_12B_R, dac_value);
  Filt_out = f_to_dac_value(y);

}


void handle_dac1(uint32_t stim_index)
{
  Filt_in = f_to_dac_value(Stim[stim_index]);
}

uint32_t f_to_dac_value(float f)
{
  // Convert a float value in the range [-1.0, 1.0] to a DAC value in the range [0, 4095]
  if (f < -1.0f) f = -1.0f;
  if (f > 1.0f) f = 1.0f;
  return (uint32_t)((f + 1.0f) * 2047.5f);
}

void set_k(float k_new)
{
  // Implementation for setting the filter coefficient k
  // This function can be used to update the filter coefficient dynamically
  // For example, you can store k in a global variable and use it in get_filter_value
  // Here, we just print the new value of k for demonstration purposes
 // printf("Setting filter coefficient k to: %f\n\r", k_new);
  printf("\n\rSetting k to %lu / 1000", (uint32_t)(k_new * 1000.0));
  G_FilterDb.k = k_new; // Update the global k variable
  G_FilterDb.alpha = exp(-G_FilterDb.k); // Update the alpha value based on the new k
// You can add code here to update the filter coefficient used in get_filter_value
}


void set_stim(uint8_t stim_type, float f_norm_new)
{
  // Implementation for setting the stimulus type and normalized frequency
  // This function can be used to update the stimulus parameters dynamically
  // For example, you can store stim_type and f_norm in global variables and use them in Stim_Init
  // Here, we just print the new values for demonstration purposes
  printf("\n\rSetting stimulus type to: %s", (stim_type == SIN) ? "SIN" : "SQR");
  printf("\n\rSetting normalized frequency to: %f", f_norm_new);
  G_FilterDb.f_norm = f_norm_new; // Update the global f_norm variable
  G_FilterDb.stim_type = stim_type; // Update the global stim_type variable
  HAL_DAC_Stop_DMA(&hdac1, DAC_CHANNEL_1);
  HAL_DAC_Stop_DMA(&hdac1, DAC_CHANNEL_2);
  G_FilterDb.stim_index = 0; // Reset stim_index to start from the beginning of the new stimulus

  if (G_FilterDb.stim_type == SIN)
  {
    G_FilterDb.stim_length = Stim_Init_sin(G_FilterDb.f_norm, 2); // Reinitialize the stimulus with the new frequency
  }
  else if (stim_type == SQR)
  {
    G_FilterDb.stim_length = Stim_Init_sqr(G_FilterDb.f_norm, 2); // Reinitialize the stimulus with the new frequency
  }
  else
  {
    printf("\n\rInvalid stimulus type");
  }
  HAL_DAC_Start_DMA(&hdac1, DAC_CHANNEL_1, &Filt_in, 1, DAC_ALIGN_12B_R);
  HAL_DAC_Start_DMA(&hdac1, DAC_CHANNEL_2, &Filt_out, 1, DAC_ALIGN_12B_R);
}


void Initialize_Filter(void)
{
  G_FilterDb.ptr_stim = Stim;
  G_FilterDb.stim_length = 0;
  G_FilterDb.stim_index = 0;
  G_FilterDb.ptr_filt_in = &Filt_in;
  G_FilterDb.ptr_filt_out = &Filt_out;
  G_FilterDb.stim_type = SIN;
  G_FilterDb.f_norm = 0.05f;
  G_FilterDb.k = 0.05f;
  G_FilterDb.alpha = exp(-G_FilterDb.k);
  G_FilterDb.filt_y_prev = 0.0f;

  set_stim(G_FilterDb.stim_type, G_FilterDb.f_norm); // Initialize the stimulus with the default type and frequency
}


extern CmdArgs CmdStruct;

uint32_t TickZero;
uint32_t TickRun;
uint8_t Count = 0;


/* USER CODE END 0 */

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
  setvbuf(stdout, NULL, _IONBF, 0);
  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */
  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_USART2_UART_Init();
  MX_DAC1_Init();
  MX_TIM2_Init();
  /* USER CODE BEGIN 2 */
	//HAL_TIM_Base_Start(&htim2);
  Initialize_Filter();
  //stim_length = Stim_Init_sin(f_norm, 2);
  printf("Stimulus initialized\n\r");
  //Filt_Init();
  //HAL_DAC_Start_DMA(&hdac1, DAC_CHANNEL_1, Stim, MAX_STIM_LENGTH, DAC_ALIGN_12B_R);
  //HAL_DAC_Stop_DMA(&hdac1, DAC_CHANNEL_1);
  HAL_DAC_Start_DMA(&hdac1, DAC_CHANNEL_1, &Filt_in, 1, DAC_ALIGN_12B_R);
  HAL_DAC_Start_DMA(&hdac1, DAC_CHANNEL_2, &Filt_out, 1, DAC_ALIGN_12B_R);
  handle_dacs();

  HAL_TIM_Base_Start_IT(&htim2);
  extern uint8_t DEBUG_UART_IT_RX;
  //debug_uart_receive_IT();
  PrintBanner();

  TickZero = HAL_GetTick();
  TickRun = TickZero + TICK_WHILE_TIME;

  HAL_UART_Receive_IT(&huart2, (uint8_t *)DEBUG_UART_IT_RX, 1);

  printf("\n\rHello 1");
  printf("It's Rich");
  HAL_UART_Transmit(&huart2, (uint8_t *)"A", 1, HAL_MAX_DELAY);
  //HAL_DAC_Start(&hdac1, DAC_CHANNEL_2);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
    //HAL_GPIO_TogglePin(LED_GREEN_GPIO_Port, LED_GREEN_Pin);
    //HAL_GPIO_TogglePin(DEBUG_0_GPIO_Port, DEBUG_0_Pin);

    while (HAL_GetTick() < TickRun)
    {
    	HAL_UART_Receive_IT(&huart2, (uint8_t *)DEBUG_UART_IT_RX, 1);
      debug_uart_receive_IT();
		  CmdStruct = Handle_UART_Rx();
		  HandleCLI(&CmdStruct);
//      HAL_Delay(500);
    }

	  TickRun = HAL_GetTick() + TICK_WHILE_TIME;

	  Count++;
	  if (Count >= 50)
	  {
		  Count = 0;
		  HAL_GPIO_TogglePin(DEBUG_0_GPIO_Port, DEBUG_0_Pin);
	  }
  }
}
  /* USER CODE END 3 */


/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSIDiv = RCC_HSI_DIV1;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = RCC_PLLM_DIV2;
  RCC_OscInitStruct.PLL.PLLN = 16;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief DAC1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_DAC1_Init(void)
{

  /* USER CODE BEGIN DAC1_Init 0 */

  /* USER CODE END DAC1_Init 0 */

  DAC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN DAC1_Init 1 */

  /* USER CODE END DAC1_Init 1 */

  /** DAC Initialization
  */
  hdac1.Instance = DAC1;
  if (HAL_DAC_Init(&hdac1) != HAL_OK)
  {
    Error_Handler();
  }

  /** DAC channel OUT1 config
  */
  sConfig.DAC_SampleAndHold = DAC_SAMPLEANDHOLD_DISABLE;
  sConfig.DAC_Trigger = DAC_TRIGGER_T2_TRGO;
  sConfig.DAC_OutputBuffer = DAC_OUTPUTBUFFER_ENABLE;
  sConfig.DAC_ConnectOnChipPeripheral = DAC_CHIPCONNECT_DISABLE;
  sConfig.DAC_UserTrimming = DAC_TRIMMING_FACTORY;
  if (HAL_DAC_ConfigChannel(&hdac1, &sConfig, DAC_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }

  /** DAC channel OUT2 config
  */
  if (HAL_DAC_ConfigChannel(&hdac1, &sConfig, DAC_CHANNEL_2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN DAC1_Init 2 */

  /* USER CODE END DAC1_Init 2 */

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
  htim2.Init.Prescaler = 160-1;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 40-1;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_UPDATE;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM2_Init 2 */

  /* USER CODE END TIM2_Init 2 */

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  huart2.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart2.Init.ClockPrescaler = UART_PRESCALER_DIV1;
  huart2.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetTxFifoThreshold(&huart2, UART_TXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetRxFifoThreshold(&huart2, UART_RXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_DisableFifoMode(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

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
  /* DMA1_Channel2_3_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel2_3_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel2_3_IRQn);

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  /* USER CODE BEGIN MX_GPIO_Init_1 */

  /* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(DEBUG_0_GPIO_Port, DEBUG_0_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : B1_Pin */
  GPIO_InitStruct.Pin = B1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : DEBUG_0_Pin */
  GPIO_InitStruct.Pin = DEBUG_0_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(DEBUG_0_GPIO_Port, &GPIO_InitStruct);

  /* USER CODE BEGIN MX_GPIO_Init_2 */

  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

int __io_putchar(int ch)
{
  uint8_t character = (uint8_t)ch;
  HAL_UART_Transmit(&huart2, &character, 1, HAL_MAX_DELAY);
  return ch;
}

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
#ifdef USE_FULL_ASSERT
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
