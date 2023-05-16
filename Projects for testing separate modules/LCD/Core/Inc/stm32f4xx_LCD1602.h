/*
 * stm32f4xx_LCD1602.h
 *
 *  Created on: May 13, 2023
 *      Author: Aurijus
 */

#ifndef INC_STM32F4XX_LCD1602_H_
#define INC_STM32F4XX_LCD1602_H_

/**
 * LCD1602 LCD screen module
 * LCD1602 Default pinout
 * 		LCD1602		STM32F4XX	DESCRIPTION
 *		VSS			GND			Ground
 *		VDD			5V			5V power
 *		V0			GND			Variable voltage for contrast (via potentiometer)
 *		RS			PE0			Register Select (H: Data, L: Instruction)
 *		RW			PE1			Read/Write (H: Read, L: Write)
 *		EN			PE2			Enable signal
 *		D4			PE3			Data bit 4
 *		D5			PE4			Data bit 5
 *		D6			PE5			Data bit 6
 *		D7			PE6			Data bit 7
 *		A			PA5			5V power for LED backlight
 *		K			PA5			GND for LED backlight
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

/* Default RS pin used */
#define RS_PORT GPIOE
#define RS_PIN GPIO_PIN_0
/* Default RW pin used */
#define RW_PORT GPIOE
#define RW_PIN GPIO_PIN_1
/* Default EN pin used */
#define EN_PORT GPIOE
#define EN_PIN GPIO_PIN_2
/* Default 4-bit mode pins used */
#define D4_PORT GPIOE
#define D4_PIN GPIO_PIN_3
#define D5_PORT GPIOE
#define D5_PIN GPIO_PIN_4
#define D6_PORT GPIOE
#define D6_PIN GPIO_PIN_5
#define D7_PORT GPIOE
#define D7_PIN GPIO_PIN_6


/**
 * Public functions
 */

/**
 * Initialize LCD1602 LCD screen
 */
extern void LCD1602_Init(void);

/**
 * Transfer a string to the LCD screen
 */
extern void LCD1602_TransferString(char *str);

/**
 * Change the cursors position in the LCD screen
 */
extern void LCD1602_CursorPosition(int row, int col);

/**
 * Private functions
 */
extern void LCD1602_TransferToLCD(char data, int rs);
extern void LCD1602_SendCommand(char cmd);
extern void LCD1602_SendData(char data);

#endif /* INC_STM32F4XX_LCD1602_H_ */
