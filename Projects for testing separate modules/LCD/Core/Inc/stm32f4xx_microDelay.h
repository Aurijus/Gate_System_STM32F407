/*
 * stm32f4xx_microDelay.h
 *
 *  Created on: May 13, 2023
 *      Author: Aurijus
 */

#ifndef INC_STM32F4XX_MICRODELAY_H_
#define INC_STM32F4XX_MICRODELAY_H_

#include "stm32f4xx.h"
#include "stm32f407xx.h"

extern TIM_HandleTypeDef htim1;

/**
 * A delay in microseconds.
 */
extern void microDelay(uint16_t us);

#endif /* INC_STM32F4XX_MICRODELAY_H_ */
