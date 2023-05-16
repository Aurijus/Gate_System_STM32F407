/*
 * stm32f4xx_dht11.h
 *
 *  Created on: May 14, 2023
 *      Author: Aurijus
 */

#ifndef INC_STM32F4XX_DHT11_H_
#define INC_STM32F4XX_DHT11_H_

/**
 * DHT11 Temperature and Humidity sensor module
 * DHT11 Default pinout
 * 		DHT11		STM32F4XX	DESCRIPTION
 *		GND			GND			Ground
 *		5V			5V			5V power
 *		S			PD8			Data bus
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

#define stepsperrev 4096

/* Default pins used */
#define DHT11_PORT GPIOD
#define DHT11_PIN GPIO_PIN_8

/**
 * Public functions
 */

/**
 * Read the values from DHT11 and assign them to global variables for printing.
 */
extern void DHT11_PrintValues(void);

/**
 * Private functions
 */
extern void DHT11_SetPinOutput(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin);
extern void DHT11_SetPinInput(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin);
extern void DHT11_StartRead(void);
extern uint8_t DHT11_CheckResponse(void);
extern uint8_t DHT11_ReadValues(void);

extern void DHT11_PrintTemp(void);

extern void DHT11_PrintHum(void);

#endif /* INC_STM32F4XX_DHT11_H_ */
