/*
 * stm32f4xx_hcse04.c
 *
 *  Created on: May 14, 2023
 *      Author: Aurijus
 */

#include "stm32f4xx_hcsr04.h"
#include "stm32f4xx_microDelay.h"
#include "stdio.h"

uint32_t IC_Val1 = 0;
uint32_t IC_Val2 = 0;
uint32_t Difference = 0;
uint8_t Is_First_Captured = 0;  // is the first value captured ?
uint8_t Distance  = 0;


uint8_t printDistance[35] = {'\0'};

void HCSR04_Read (void)
{
	HAL_GPIO_WritePin(TRIG_PORT, TRIG_PIN, GPIO_PIN_SET);  // pull the TRIG pin HIGH
	microDelay(10);  // wait for 10 us
	HAL_GPIO_WritePin(TRIG_PORT, TRIG_PIN, GPIO_PIN_RESET);  // pull the TRIG pin low

	__HAL_TIM_ENABLE_IT(&htim1, TIM_IT_CC1);
}

void HCSR04_Measure (TIM_HandleTypeDef *htim)
{
	if (Is_First_Captured==0) // if the first value is not captured
	{
		IC_Val1 = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1); // read the first value
		Is_First_Captured = 1;  // set the first captured as true
		// Now change the polarity to falling edge
		__HAL_TIM_SET_CAPTUREPOLARITY(htim, TIM_CHANNEL_1, TIM_INPUTCHANNELPOLARITY_FALLING);
	}
	else if (Is_First_Captured==1)   // if the first is already captured
	{
		IC_Val2 = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1);  // read second value
		__HAL_TIM_SET_COUNTER(htim, 0);  // reset the counter

		if (IC_Val2 > IC_Val1)
			Difference = IC_Val2-IC_Val1;
		else if (IC_Val1 > IC_Val2)
			Difference = (0xffff - IC_Val1) + IC_Val2;

		Distance = Difference * .034/2;

		sprintf(printDistance, "Distance: %d\r\n", Distance);

		Is_First_Captured = 0; // set it back to false

		// set polarity to rising edge
		__HAL_TIM_SET_CAPTUREPOLARITY(htim, TIM_CHANNEL_1, TIM_INPUTCHANNELPOLARITY_RISING);
		__HAL_TIM_DISABLE_IT(&htim1, TIM_IT_CC1);
	}
}
