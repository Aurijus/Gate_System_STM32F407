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
#define LCD_EN_Pin GPIO_PIN_2
#define LCD_EN_GPIO_Port GPIOE
#define LCD_D4_Pin GPIO_PIN_3
#define LCD_D4_GPIO_Port GPIOE
#define LCD_D5_Pin GPIO_PIN_4
#define LCD_D5_GPIO_Port GPIOE
#define LCD_D6_Pin GPIO_PIN_5
#define LCD_D6_GPIO_Port GPIOE
#define LCD_D7_Pin GPIO_PIN_6
#define LCD_D7_GPIO_Port GPIOE
#define MFRC522_SDA_Pin GPIO_PIN_4
#define MFRC522_SDA_GPIO_Port GPIOA
#define MFRC522_SCK_Pin GPIO_PIN_5
#define MFRC522_SCK_GPIO_Port GPIOA
#define MFRC522_MISO_Pin GPIO_PIN_6
#define MFRC522_MISO_GPIO_Port GPIOA
#define MFRC522_MOSI_Pin GPIO_PIN_7
#define MFRC522_MOSI_GPIO_Port GPIOA
#define MFRC522_RST_Pin GPIO_PIN_4
#define MFRC522_RST_GPIO_Port GPIOC
#define DistIn_Trig_Pin GPIO_PIN_8
#define DistIn_Trig_GPIO_Port GPIOE
#define DistIn_Echo_Pin GPIO_PIN_9
#define DistIn_Echo_GPIO_Port GPIOE
#define DistOut_Trig_Pin GPIO_PIN_10
#define DistOut_Trig_GPIO_Port GPIOE
#define DistOut_Echo_Pin GPIO_PIN_11
#define DistOut_Echo_GPIO_Port GPIOE
#define DHT11_DataBus_Pin GPIO_PIN_8
#define DHT11_DataBus_GPIO_Port GPIOD
#define StepMotor_IN1_Pin GPIO_PIN_3
#define StepMotor_IN1_GPIO_Port GPIOD
#define StepMotor_IN2_Pin GPIO_PIN_4
#define StepMotor_IN2_GPIO_Port GPIOD
#define StepMotor_IN3_Pin GPIO_PIN_5
#define StepMotor_IN3_GPIO_Port GPIOD
#define StepMotor_IN4_Pin GPIO_PIN_6
#define StepMotor_IN4_GPIO_Port GPIOD
#define LCD_RS_Pin GPIO_PIN_0
#define LCD_RS_GPIO_Port GPIOE
#define LCD_RW_Pin GPIO_PIN_1
#define LCD_RW_GPIO_Port GPIOE

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
