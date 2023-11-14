/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
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
#include "stm32f4xx_hal.h"

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
#define KEY_USER_Pin GPIO_PIN_13
#define KEY_USER_GPIO_Port GPIOC
#define KEY_ENC1_Pin GPIO_PIN_0
#define KEY_ENC1_GPIO_Port GPIOC
#define KEY_ENC2_Pin GPIO_PIN_1
#define KEY_ENC2_GPIO_Port GPIOC
#define KEY_ENC3_Pin GPIO_PIN_2
#define KEY_ENC3_GPIO_Port GPIOC
#define TFT_DC_Pin GPIO_PIN_3
#define TFT_DC_GPIO_Port GPIOC
#define ENC2_A_Pin GPIO_PIN_0
#define ENC2_A_GPIO_Port GPIOA
#define ENC2_B_Pin GPIO_PIN_1
#define ENC2_B_GPIO_Port GPIOA
#define PWM_IRON_Pin GPIO_PIN_2
#define PWM_IRON_GPIO_Port GPIOA
#define SW_AIR_Pin GPIO_PIN_3
#define SW_AIR_GPIO_Port GPIOA
#define SW_IRON_Pin GPIO_PIN_4
#define SW_IRON_GPIO_Port GPIOA
#define TFT_SCK_Pin GPIO_PIN_5
#define TFT_SCK_GPIO_Port GPIOA
#define TFT_MISO_Pin GPIO_PIN_6
#define TFT_MISO_GPIO_Port GPIOA
#define TFT_MOSI_Pin GPIO_PIN_7
#define TFT_MOSI_GPIO_Port GPIOA
#define ZERO_CROSS_Pin GPIO_PIN_4
#define ZERO_CROSS_GPIO_Port GPIOC
#define ZERO_CROSS_EXTI_IRQn EXTI4_IRQn
#define CS_IRON_Pin GPIO_PIN_5
#define CS_IRON_GPIO_Port GPIOC
#define CS_GUN_Pin GPIO_PIN_0
#define CS_GUN_GPIO_Port GPIOB
#define LED3_Pin GPIO_PIN_1
#define LED3_GPIO_Port GPIOB
#define LED2_Pin GPIO_PIN_2
#define LED2_GPIO_Port GPIOB
#define RELAY_Pin GPIO_PIN_10
#define RELAY_GPIO_Port GPIOB
#define TFT_CS_Pin GPIO_PIN_11
#define TFT_CS_GPIO_Port GPIOB
#define DIMMER_2_Pin GPIO_PIN_12
#define DIMMER_2_GPIO_Port GPIOB
#define SPI_SCK_Pin GPIO_PIN_13
#define SPI_SCK_GPIO_Port GPIOB
#define SPI_MISO_Pin GPIO_PIN_14
#define SPI_MISO_GPIO_Port GPIOB
#define SPI_MOSI_Pin GPIO_PIN_15
#define SPI_MOSI_GPIO_Port GPIOB
#define ENC1_A_Pin GPIO_PIN_6
#define ENC1_A_GPIO_Port GPIOC
#define ENC1_B_Pin GPIO_PIN_7
#define ENC1_B_GPIO_Port GPIOC
#define SD_DETECT_Pin GPIO_PIN_8
#define SD_DETECT_GPIO_Port GPIOA
#define TEST_120Hz_Pin GPIO_PIN_15
#define TEST_120Hz_GPIO_Port GPIOA
#define DIMMER_1_Pin GPIO_PIN_4
#define DIMMER_1_GPIO_Port GPIOB
#define TFT_RST_Pin GPIO_PIN_5
#define TFT_RST_GPIO_Port GPIOB
#define ENC3_A_Pin GPIO_PIN_6
#define ENC3_A_GPIO_Port GPIOB
#define ENC3_B_Pin GPIO_PIN_7
#define ENC3_B_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

#define DEBOUNCE_SW 		50
#define NUM_DIMMERS 		2
#define DIMMER_VALUE_MAX	100

typedef struct {
	uint32_t cnt;
	uint32_t val;
	uint32_t dir;
} encoder;
/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
