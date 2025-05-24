/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
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
#define ButtonRL_Pin GPIO_PIN_0
#define ButtonRL_GPIO_Port GPIOI
#define POWER1_Pin GPIO_PIN_2
#define POWER1_GPIO_Port GPIOH
#define POWER2_Pin GPIO_PIN_3
#define POWER2_GPIO_Port GPIOH
#define POWER3_Pin GPIO_PIN_4
#define POWER3_GPIO_Port GPIOH
#define POWER4_Pin GPIO_PIN_5
#define POWER4_GPIO_Port GPIOH
#define LED6_Pin GPIO_PIN_6
#define LED6_GPIO_Port GPIOG
#define ButtonRH_Pin GPIO_PIN_12
#define ButtonRH_GPIO_Port GPIOH
#define LED5_Pin GPIO_PIN_5
#define LED5_GPIO_Port GPIOG
#define LED4_Pin GPIO_PIN_4
#define LED4_GPIO_Port GPIOG
#define LED3_Pin GPIO_PIN_3
#define LED3_GPIO_Port GPIOG
#define ButtonLL_Pin GPIO_PIN_11
#define ButtonLL_GPIO_Port GPIOH
#define ButtonLH_Pin GPIO_PIN_10
#define ButtonLH_GPIO_Port GPIOH
#define LED2_Pin GPIO_PIN_2
#define LED2_GPIO_Port GPIOG
#define LED1_Pin GPIO_PIN_1
#define LED1_GPIO_Port GPIOG
#define AS69_MD1_Pin GPIO_PIN_14
#define AS69_MD1_GPIO_Port GPIOD
#define AS69_MD0_Pin GPIO_PIN_13
#define AS69_MD0_GPIO_Port GPIOD
#define RS485_RE_Pin GPIO_PIN_1
#define RS485_RE_GPIO_Port GPIOA
#define AS69_AUX_Pin GPIO_PIN_12
#define AS69_AUX_GPIO_Port GPIOD
#define RS485_DE_Pin GPIO_PIN_2
#define RS485_DE_GPIO_Port GPIOA
#define LED_Green_Pin GPIO_PIN_11
#define LED_Green_GPIO_Port GPIOE
#define LED_Red_Pin GPIO_PIN_14
#define LED_Red_GPIO_Port GPIOF

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
