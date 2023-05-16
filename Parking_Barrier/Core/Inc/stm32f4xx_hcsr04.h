/*
 * stm32f4xx_hcse04.h
 *
 *  Created on: May 14, 2023
 *      Author: Aurijus
 */

#ifndef INC_STM32F4XX_HCSR04_H_
#define INC_STM32F4XX_HCSR04_H_

/**
 * HC-SR04 LCD screen module
 * HC-SR04 Default pinout
 * 		LCD1602		STM32F4XX	DESCRIPTION
 *		GND			GND			Ground
 *		VCC			5V			5V power
 *		Trig		PE8			Trig - Send read signal
 *		Echo		PE9			Echo - Receive values
 */

/**
 * Library dependencies
 * - STM32F4xx
 * - STM32F407xx
 * - STM32F4xx HAL GPIO
 */

/* Includes */
#include "stm32f4xx.h"
#include "stm32f407xx.h"
#include "stm32f4xx_hal_gpio.h"

/* Default Trig pin used */
#define TRIG_PIN_IN GPIO_PIN_8
#define TRIG_PIN_OUT GPIO_PIN_10
#define TRIG_PORT GPIOE

/**
 * Public functions
 */

/**
 * Read values from HC-SR04
 * (Entering object)
 */
extern void HCSR04_ReadIn (void);

/**
 * Read values from HC-SR04
 * (Exiting object)
 * */
extern void HCSR04_ReadOut (void);

/**
 * Private functions
 */
extern void HCSR04_MeasureIn (TIM_HandleTypeDef *htim);
extern void HCSR04_MeasureOut (TIM_HandleTypeDef *htim);

#endif /* INC_STM32F4XX_HCSR04_H_ */
