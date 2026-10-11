/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32g0xx_hal.h"

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

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define B1_Pin GPIO_PIN_13
#define B1_GPIO_Port GPIOC
#define USART2_TX_Pin GPIO_PIN_2
#define USART2_TX_GPIO_Port GPIOA
#define USART2_RX_Pin GPIO_PIN_3
#define USART2_RX_GPIO_Port GPIOA
#define TMS_Pin GPIO_PIN_13
#define TMS_GPIO_Port GPIOA
#define TCK_Pin GPIO_PIN_14
#define TCK_GPIO_Port GPIOA
#define DEBUG_0_Pin GPIO_PIN_8
#define DEBUG_0_GPIO_Port GPIOC

/* USER CODE BEGIN Private defines */

uint32_t Stim_Init_sin(float f_norm, uint32_t n_cycles);
uint32_t Stim_Init_sqr(float f_norm, uint32_t n_cycles);

float get_filter_value(float x_in, float y_prev);
void handle_dac2(uint32_t stim_index);
void handle_dacs(void);
void handle_dac1(uint32_t stim_index);
uint32_t f_to_dac_value(float f);


void set_k(float k);
void set_stim(uint8_t stim_type, float f_norm_new);


#define FALSE 0
#define TRUE 1

#define TICK_WHILE_TIME 20

#define SIN 0
#define SQR 1

typedef struct {
  float * ptr_stim;
  uint32_t stim_length;
  uint32_t stim_index;
  uint32_t * ptr_filt_in;
  uint32_t * ptr_filt_out;
  uint8_t stim_type;
  float f_norm;
  float k;
  float alpha;
  float filt_y_prev;
} ii_filter_t;

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
