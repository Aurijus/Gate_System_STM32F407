/*
 * stm32f4xx_StepperMotor.c
 *
 *  Created on: May 14, 2023
 *      Author: Aurijus
 */

#include "stm32f4xx_StepperMotor.h"
#include "stm32f4xx_microDelay.h"


void StepperMotor_SetRPM (int rpm)  // Max. RPM = 14 (Recommended 13)
{
	microDelay(60000000/stepsperrev/rpm);
}

void StepperMotor_HalfDrive (int step)
{
	switch (step){
		case 0:
			  HAL_GPIO_WritePin(MOTOR_PORT, MOTOR_IN1, GPIO_PIN_SET);
			  HAL_GPIO_WritePin(MOTOR_PORT, MOTOR_IN2, GPIO_PIN_RESET);
			  HAL_GPIO_WritePin(MOTOR_PORT, MOTOR_IN3, GPIO_PIN_RESET);
			  HAL_GPIO_WritePin(MOTOR_PORT, MOTOR_IN4, GPIO_PIN_RESET);
			  break;

		case 1:
			  HAL_GPIO_WritePin(MOTOR_PORT, MOTOR_IN1, GPIO_PIN_SET);
			  HAL_GPIO_WritePin(MOTOR_PORT, MOTOR_IN2, GPIO_PIN_SET);
			  HAL_GPIO_WritePin(MOTOR_PORT, MOTOR_IN3, GPIO_PIN_RESET);
			  HAL_GPIO_WritePin(MOTOR_PORT, MOTOR_IN4, GPIO_PIN_RESET);
			  break;

		case 2:
			  HAL_GPIO_WritePin(MOTOR_PORT, MOTOR_IN1, GPIO_PIN_RESET);
			  HAL_GPIO_WritePin(MOTOR_PORT, MOTOR_IN2, GPIO_PIN_SET);
			  HAL_GPIO_WritePin(MOTOR_PORT, MOTOR_IN3, GPIO_PIN_RESET);
			  HAL_GPIO_WritePin(MOTOR_PORT, MOTOR_IN4, GPIO_PIN_RESET);
			  break;

		case 3:
			  HAL_GPIO_WritePin(MOTOR_PORT, MOTOR_IN1, GPIO_PIN_RESET);
			  HAL_GPIO_WritePin(MOTOR_PORT, MOTOR_IN2, GPIO_PIN_SET);
			  HAL_GPIO_WritePin(MOTOR_PORT, MOTOR_IN3, GPIO_PIN_SET);
			  HAL_GPIO_WritePin(MOTOR_PORT, MOTOR_IN4, GPIO_PIN_RESET);
			  break;

		case 4:
			  HAL_GPIO_WritePin(MOTOR_PORT, MOTOR_IN1, GPIO_PIN_RESET);
			  HAL_GPIO_WritePin(MOTOR_PORT, MOTOR_IN2, GPIO_PIN_RESET);
			  HAL_GPIO_WritePin(MOTOR_PORT, MOTOR_IN3, GPIO_PIN_SET);
			  HAL_GPIO_WritePin(MOTOR_PORT, MOTOR_IN4, GPIO_PIN_RESET);
			  break;

		case 5:
			  HAL_GPIO_WritePin(MOTOR_PORT, MOTOR_IN1, GPIO_PIN_RESET);
			  HAL_GPIO_WritePin(MOTOR_PORT, MOTOR_IN2, GPIO_PIN_RESET);
			  HAL_GPIO_WritePin(MOTOR_PORT, MOTOR_IN3, GPIO_PIN_SET);
			  HAL_GPIO_WritePin(MOTOR_PORT, MOTOR_IN4, GPIO_PIN_SET);
			  break;

		case 6:
			  HAL_GPIO_WritePin(MOTOR_PORT, MOTOR_IN1, GPIO_PIN_RESET);
			  HAL_GPIO_WritePin(MOTOR_PORT, MOTOR_IN2, GPIO_PIN_RESET);
			  HAL_GPIO_WritePin(MOTOR_PORT, MOTOR_IN3, GPIO_PIN_RESET);
			  HAL_GPIO_WritePin(MOTOR_PORT, MOTOR_IN4, GPIO_PIN_SET);
			  break;

		case 7:
			  HAL_GPIO_WritePin(MOTOR_PORT, MOTOR_IN1, GPIO_PIN_SET);
			  HAL_GPIO_WritePin(MOTOR_PORT, MOTOR_IN2, GPIO_PIN_RESET);
			  HAL_GPIO_WritePin(MOTOR_PORT, MOTOR_IN3, GPIO_PIN_RESET);
			  HAL_GPIO_WritePin(MOTOR_PORT, MOTOR_IN4, GPIO_PIN_SET);
			  break;

		}
}

void StepperMotor_WaveDrive (int step)
{
	switch (step){
		case 0:
			  HAL_GPIO_WritePin(MOTOR_PORT, MOTOR_IN1, GPIO_PIN_SET);
			  HAL_GPIO_WritePin(MOTOR_PORT, MOTOR_IN2, GPIO_PIN_RESET);
			  HAL_GPIO_WritePin(MOTOR_PORT, MOTOR_IN3, GPIO_PIN_RESET);
			  HAL_GPIO_WritePin(MOTOR_PORT, MOTOR_IN4, GPIO_PIN_RESET);
			  break;

		case 1:
			  HAL_GPIO_WritePin(MOTOR_PORT, MOTOR_IN1, GPIO_PIN_RESET);
			  HAL_GPIO_WritePin(MOTOR_PORT, MOTOR_IN2, GPIO_PIN_SET);
			  HAL_GPIO_WritePin(MOTOR_PORT, MOTOR_IN3, GPIO_PIN_RESET);
			  HAL_GPIO_WritePin(MOTOR_PORT, MOTOR_IN4, GPIO_PIN_RESET);
			  break;

		case 2:
			  HAL_GPIO_WritePin(MOTOR_PORT, MOTOR_IN1, GPIO_PIN_RESET);
			  HAL_GPIO_WritePin(MOTOR_PORT, MOTOR_IN2, GPIO_PIN_RESET);
			  HAL_GPIO_WritePin(MOTOR_PORT, MOTOR_IN3, GPIO_PIN_SET);
			  HAL_GPIO_WritePin(MOTOR_PORT, MOTOR_IN4, GPIO_PIN_RESET);
			  break;

		case 3:
			  HAL_GPIO_WritePin(MOTOR_PORT, MOTOR_IN1, GPIO_PIN_RESET);
			  HAL_GPIO_WritePin(MOTOR_PORT, MOTOR_IN2, GPIO_PIN_RESET);
			  HAL_GPIO_WritePin(MOTOR_PORT, MOTOR_IN3, GPIO_PIN_RESET);
			  HAL_GPIO_WritePin(MOTOR_PORT, MOTOR_IN4, GPIO_PIN_SET);
			  break;
	}
}

void StepperMotor_AngledStep (float angle, int direction, int rpm)
{
	/* 360 degrees = 512 sequences */
	float AnglePerSeq = 0.703125;
	int NumberOfSequences = (int) (angle/AnglePerSeq);

	for (int seq = 0; seq < NumberOfSequences ; seq++)
	{
		if (direction == 0)  // Clockwise rotation
		{
			for (int step = 7; step >= 0; step--)
			{
				StepperMotor_HalfDrive(step);
				StepperMotor_SetRPM(rpm);
			}

		}
		else if (direction == 1)  // Anti-Clockwise rotation
		{
			for (int step = 0; step < 8; step++)
			{
				StepperMotor_HalfDrive(step);
				StepperMotor_SetRPM(rpm);
			}
		}
	}
}
