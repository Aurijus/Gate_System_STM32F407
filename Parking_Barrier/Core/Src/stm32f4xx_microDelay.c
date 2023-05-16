/*
 * stm32f4xx_microDelay.c
 *
 *  Created on: May 13, 2023
 *      Author: Aurijus
 */

#include "stm32f4xx_microDelay.h"

void microDelay(uint16_t us)
{
    __HAL_TIM_SET_COUNTER(&htim2, 0);
    while (__HAL_TIM_GET_COUNTER(&htim2) < us);
}
