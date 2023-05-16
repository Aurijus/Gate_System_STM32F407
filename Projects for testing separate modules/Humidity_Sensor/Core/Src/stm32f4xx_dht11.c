/*
 * stm32f4xx_dht11.c
 *
 *  Created on: May 14, 2023
 *      Author: Aurijus
 */

#include "stm32f4xx_LCD1602.h"
#include "stm32f4xx_dht11.h"
#include "stm32f4xx_microDelay.h"
#include "stdio.h"


uint8_t Rh_Integral, Rh_Decimal, Temp_Integral, Temp_Decimal;
uint16_t SUM, RH, TEMP;

float Temperature = 0;
float Humidity = 0;
uint8_t Presence;



void DHT11_SetPinOutput(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	GPIO_InitStruct.Pin = GPIO_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOx, &GPIO_InitStruct);
}

void DHT11_SetPinInput(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	GPIO_InitStruct.Pin = GPIO_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOx, &GPIO_InitStruct);
}

void DHT11_StartRead(void)
{
	DHT11_SetPinOutput(DHT11_PORT, DHT11_PIN);  // set the pin as output

	HAL_GPIO_WritePin(DHT11_PORT, DHT11_PIN, 0);   // pull the pin low
	HAL_Delay(18);   // wait for 18 ms

	HAL_GPIO_WritePin(DHT11_PORT, DHT11_PIN, 1); // pull the pin high
	microDelay(20); // wait for 20 us

	DHT11_SetPinInput(DHT11_PORT, DHT11_PIN);    // set as input
}

uint8_t DHT11_CheckResponse(void)
{
	uint8_t Response = 0;

	microDelay(40);

	if (!(HAL_GPIO_ReadPin(DHT11_PORT, DHT11_PIN)))
	{
		microDelay(80);

		if((HAL_GPIO_ReadPin (DHT11_PORT, DHT11_PIN)))
			Response = 1;
		else
			Response = -1;
	}

	while((HAL_GPIO_ReadPin(DHT11_PORT, DHT11_PIN)));   // wait for the pin to go low

	return Response;
}

uint8_t DHT11_ReadValues(void)
{
	uint8_t i,  j;

	for (j = 0; j < 8; j++)
	{
		while (!(HAL_GPIO_ReadPin(DHT11_PORT, DHT11_PIN)));   // wait for the pin to go high

		microDelay(40);   // wait for 40 us

		if (!(HAL_GPIO_ReadPin(DHT11_PORT, DHT11_PIN)))   // if the pin is low
			i&= ~(1<<(7-j));   // write 0
		else
			i|= (1<<(7-j));  // if the pin is high, write 1

		while ((HAL_GPIO_ReadPin(DHT11_PORT, DHT11_PIN)));  // wait for the pin to go low
	}
	return i;
}


void DHT11_PrintValues(void)
{
	DHT11_StartRead();
	Presence = DHT11_CheckResponse();
	Rh_Integral = DHT11_ReadValues();
	Rh_Decimal = DHT11_ReadValues();
	Temp_Integral = DHT11_ReadValues();
	Temp_Decimal  = DHT11_ReadValues();
	SUM = DHT11_ReadValues();

	if(SUM == Rh_Integral + Rh_Decimal + Temp_Integral + Temp_Decimal)
	{
		Temperature = (float)Temp_Integral + (float)Temp_Decimal/10;
		Humidity = (float)Rh_Integral + (float)Rh_Decimal/10;
	}
}


void DHT11_PrintTemp(void)
{
	char printTemp[20] = {'\0'};
	LCD1602_CursorPosition(0, 0);

	sprintf(printTemp, "Temp: %.1f", Temperature);
	LCD1602_TransferString(printTemp);

}

void DHT11_PrintHum(void)
{
	char printHum[20] = {'\0'};
	LCD1602_CursorPosition(1, 0);


	sprintf(printHum,"RH: %.1f", Humidity);
	LCD1602_TransferString(printHum);
}
