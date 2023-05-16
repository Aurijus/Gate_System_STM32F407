/*
 * stm32f4xx_StepperMotor.h
 *
 *  Created on: May 14, 2023
 *      Author: Aurijus
 */

#ifndef INC_STM32F4XX_STEPPERMOTOR_H_
#define INC_STM32F4XX_STEPPERMOTOR_H_

/**
 * 28BYJ-48 Stepper Motor
 * 28BYJ-48 Default pinout
 * 		LCD1602		STM32F4XX	DESCRIPTION
 *		GND			GND			Ground
 *		5V			5V			5V power
 *		IN1			PD3			Motor control module pin 1
 *		IN2			PD4			Motor control module pin 2
 *		IN3			PD5			Motor control module pin 3
 *		IN4			PD6			Motor control module pin 4
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
#define MOTOR_PORT GPIOD
#define MOTOR_IN1 GPIO_PIN_3
#define MOTOR_IN2 GPIO_PIN_4
#define MOTOR_IN3 GPIO_PIN_5
#define MOTOR_IN4 GPIO_PIN_6

/**
 * Public functions
 */

/**
 * Move the Step Motor at an angled step.
 * This implementation uses Half Drive mode.
 */
extern void StepperMotor_AngledStep (float angle, int direction, int rpm);

/**
 * Private functions
 */
extern void StepperMotor_SetRPM (int rpm);
extern void StepperMotor_HalfDrive (int step);
extern void StepperMotor_WaveDrive (int step);


#endif /* INC_STM32F4XX_STEPPERMOTOR_H_ */
