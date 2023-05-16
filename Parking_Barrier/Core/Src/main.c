/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stm32f4xx_StepperMotor.h"
#include "stdio.h"
#include "stm32f4xx_LCD1602.h"
#include "stm32f4_mfrc522.h"
#include "stm32f4xx_dht11.h"
#include "stm32f4xx_hcsr04.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
SPI_HandleTypeDef hspi1;

TIM_HandleTypeDef htim1;
TIM_HandleTypeDef htim2;

/* USER CODE BEGIN PV */

/* For RFID scanner */
extern uint8_t CardID[4];
extern uint8_t CardID_Print[50];
extern uint8_t buffer_Detection[50];
uint8_t CompareID[4] = {0xAA, 0xFE, 0x02, 0x3F};

/* For LCD screen */
int row = 0;
int col = 0;

/* For Distance sensor */
extern char printDistanceIn[20];
extern char printDistanceOut[20];
extern uint8_t DistanceIn;
extern uint8_t DistanceOut;

/* For Main code */
int stepCount = 0;
int distCount = 0;
int isAuthorized = 0;
int totalSpaces = 20;
char spacesPrint[20] = {'\0'};

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_TIM1_Init(void);
static void MX_TIM2_Init(void);
static void MX_SPI1_Init(void);
static void MX_NVIC_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_TIM1_Init();
  MX_TIM2_Init();
  MX_SPI1_Init();

  /* Initialize interrupts */
  MX_NVIC_Init();
  /* USER CODE BEGIN 2 */
  HAL_TIM_IC_Start_IT(&htim1, TIM_CHANNEL_1);
  HAL_TIM_IC_Start_IT(&htim1, TIM_CHANNEL_2);
  HAL_TIM_Base_Start(&htim2);
  LCD1602_Init();
  MFRC522_Init();

  LCD1602_Clear();
  LCD1602_TransferString("Initializing>");

  /* Initialization, getting the values from the sensors for a few iterations so they get synchronized (otherwise some false values are provided to the main code) */
  for (int i = 0 ; i < 5 ; i++)
  	 {
  		 HAL_Delay(500);
  		 DHT11_PrintValues();
  		 HAL_Delay(500);
  		 HCSR04_ReadIn();
  		 HAL_Delay(500);
  		 HCSR04_ReadOut();
  	 }

  sprintf(spacesPrint,"Spaces left: %d", totalSpaces);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	LCD1602_Clear();

	/* Printing Temperature and Humidity values */
	DHT11_PrintTemp();
	DHT11_PrintHum();

	/* If an object approaches any one of the Distance sensors */
	while ((DistanceIn <= 5 || DistanceOut <= 5) && distCount < 5)
	{
		/* If an object wants to enter (Distance value can vary)*/
		while (DistanceIn <= 5)
		{
			LCD1602_Clear();

			/* Indicate that an object is entering and how many spaces are left (if object had bad authorization attempt or no free spaces left, output error message)*/
			if (isAuthorized == -1)
			{
				LCD1602_CursorPosition(0, 0);
				LCD1602_TransferString("No spaces left");
				LCD1602_CursorPosition(1, 0);
				LCD1602_TransferString("or bad auth.");
			}
			else
			{
				LCD1602_CursorPosition(0, 0);
				LCD1602_TransferString("Object entering!");
				LCD1602_CursorPosition(1, 0);
				LCD1602_TransferString(spacesPrint);
				HAL_Delay(2000);
			}


			/* If no free spaces are left, gates do not open */
			if (totalSpaces <= 0)
			{
				LCD1602_Clear();
				LCD1602_CursorPosition(0, 0);
				LCD1602_TransferString("No spaces left.");
				isAuthorized = -1;
			}

			/* If there are free spaces, begin authentication process */
			while(isAuthorized == 0)
			{
				LCD1602_Clear();
				LCD1602_CursorPosition(0, 0);
				LCD1602_TransferString("Authentication?");
				HAL_Delay(2000);

				/* Checks if any RFID cards/tags were placed and if they're authorized */
				if (ReadCardValue() == MI_OK)
				{
					LCD1602_Clear();
					LCD1602_CursorPosition(0, 0);
					LCD1602_TransferString((char*)buffer_Detection);
					LCD1602_CursorPosition(1, 0);
					LCD1602_TransferString((char*)CardID_Print);
					HAL_Delay(1000);

					/* If tag ID passes the authorization check and it matches on the database, grant the pass and decrements free spaces by 1 */
					if(MFRC522_Compare(CardID, CompareID) == MI_OK)
					{
						LCD1602_Clear();
						LCD1602_CursorPosition(0, 0);
						LCD1602_TransferString("Authorized!");
						isAuthorized = 1;
						totalSpaces--;
						HAL_Delay(1000);
					}
					else
					{
						/* If  the tag ID fails to pass the authorization (the data does not match with any on the database), does not grant the pass */
						LCD1602_Clear();
						LCD1602_CursorPosition(0, 0);
						LCD1602_TransferString("Not authorized!");
						isAuthorized = -1;
						HAL_Delay(1000);
					}
				}
				else
				{
					/* Check if a correct card is placed */
					LCD1602_Clear();
					LCD1602_CursorPosition(0, 0);
					LCD1602_TransferString("No card or");
					LCD1602_CursorPosition(1, 0);
					LCD1602_TransferString("wrong card!");
					HAL_Delay(2000);
				}
			}

			/* If granted authorization, open the gates */
			if (isAuthorized == 1)
			{
				if (stepCount == 0)
				{
					StepperMotor_AngledStep(90,0,13);
					HAL_Delay(1000);
					stepCount = 1;
				}
			}

			/* Continue reading values until the object passes, so it doesn't close too early */
			HAL_Delay(1000);
			HCSR04_ReadIn();
			HAL_Delay(250);
		}

		/* If an object wants to exit (Distance value can vary)*/
		while (DistanceOut <= 5)
		{
			LCD1602_Clear();
			LCD1602_CursorPosition(0, 0);
			LCD1602_TransferString("Bye bye!");

			/* Exiting objects do not require any authorization and increments the total spaces by 1 */
			if (stepCount == 0)
			{
				StepperMotor_AngledStep(90,0,13);
				HAL_Delay(1000);
				totalSpaces++;
				stepCount = 1;
			}

			/* Continue reading values until the object passes, so it doesn't close too early */
			HAL_Delay(1000);
			HCSR04_ReadOut();
			HAL_Delay(250);
		}

		/* Close the gate if it was opened */
		if (stepCount == 1)
		{
			StepperMotor_AngledStep(90,1,13);
			stepCount = 0;
			HAL_Delay(1000);
		}

		/* Actively read the Distance sensor values for some time to increase the systems efficiency and responsiveness  */
		sprintf(spacesPrint,"Spaces left: %d", totalSpaces);
		isAuthorized = 0;
		HCSR04_ReadIn();
		HAL_Delay(250);
		HCSR04_ReadOut();
		HAL_Delay(250);
		distCount++;

	}

	/* If no more object are trying to pass the gate, gather values from both the Temperature and Humidity sensor and check the Distance sensors as well */
	distCount = 0;
	HCSR04_ReadIn();
	HAL_Delay(250);
	HCSR04_ReadOut();
	HAL_Delay(250);
	DHT11_PrintValues();
	HAL_Delay(250);

    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 72;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 7;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief NVIC Configuration.
  * @retval None
  */
static void MX_NVIC_Init(void)
{
  /* TIM2_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(TIM2_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(TIM2_IRQn);
  /* SPI1_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(SPI1_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(SPI1_IRQn);
  /* TIM1_CC_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(TIM1_CC_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(TIM1_CC_IRQn);
}

/**
  * @brief SPI1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI1_Init(void)
{

  /* USER CODE BEGIN SPI1_Init 0 */

  /* USER CODE END SPI1_Init 0 */

  /* USER CODE BEGIN SPI1_Init 1 */

  /* USER CODE END SPI1_Init 1 */
  /* SPI1 parameter configuration*/
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_256;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI1_Init 2 */

  /* USER CODE END SPI1_Init 2 */

}

/**
  * @brief TIM1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM1_Init(void)
{

  /* USER CODE BEGIN TIM1_Init 0 */

  /* USER CODE END TIM1_Init 0 */

  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_IC_InitTypeDef sConfigIC = {0};

  /* USER CODE BEGIN TIM1_Init 1 */

  /* USER CODE END TIM1_Init 1 */
  htim1.Instance = TIM1;
  htim1.Init.Prescaler = 71;
  htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim1.Init.Period = 0xffff-1;
  htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim1.Init.RepetitionCounter = 0;
  htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_IC_Init(&htim1) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigIC.ICPolarity = TIM_INPUTCHANNELPOLARITY_RISING;
  sConfigIC.ICSelection = TIM_ICSELECTION_DIRECTTI;
  sConfigIC.ICPrescaler = TIM_ICPSC_DIV1;
  sConfigIC.ICFilter = 0;
  if (HAL_TIM_IC_ConfigChannel(&htim1, &sConfigIC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_IC_ConfigChannel(&htim1, &sConfigIC, TIM_CHANNEL_2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM1_Init 2 */

  /* USER CODE END TIM1_Init 2 */

}

/**
  * @brief TIM2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM2_Init(void)
{

  /* USER CODE BEGIN TIM2_Init 0 */

  /* USER CODE END TIM2_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM2_Init 1 */

  /* USER CODE END TIM2_Init 1 */
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 71;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 0xffff-1;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM2_Init 2 */

  /* USER CODE END TIM2_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOE, LCD_EN_Pin|LCD_D4_Pin|LCD_D5_Pin|LCD_D6_Pin
                          |LCD_D7_Pin|LCD_RS_Pin|LCD_RW_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(MFRC522_SDA_GPIO_Port, MFRC522_SDA_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(MFRC522_RST_GPIO_Port, MFRC522_RST_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOE, DistIn_Trig_Pin|DistOut_Trig_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(DHT11_DataBus_GPIO_Port, DHT11_DataBus_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOD, StepMotor_IN1_Pin|StepMotor_IN2_Pin|StepMotor_IN3_Pin|StepMotor_IN4_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : LCD_EN_Pin LCD_D4_Pin LCD_D5_Pin LCD_D6_Pin
                           LCD_D7_Pin DistIn_Trig_Pin DistOut_Trig_Pin LCD_RS_Pin
                           LCD_RW_Pin */
  GPIO_InitStruct.Pin = LCD_EN_Pin|LCD_D4_Pin|LCD_D5_Pin|LCD_D6_Pin
                          |LCD_D7_Pin|DistIn_Trig_Pin|DistOut_Trig_Pin|LCD_RS_Pin
                          |LCD_RW_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  /*Configure GPIO pin : MFRC522_SDA_Pin */
  GPIO_InitStruct.Pin = MFRC522_SDA_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(MFRC522_SDA_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : MFRC522_RST_Pin */
  GPIO_InitStruct.Pin = MFRC522_RST_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(MFRC522_RST_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : DHT11_DataBus_Pin */
  GPIO_InitStruct.Pin = DHT11_DataBus_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(DHT11_DataBus_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : StepMotor_IN1_Pin StepMotor_IN2_Pin StepMotor_IN3_Pin StepMotor_IN4_Pin */
  GPIO_InitStruct.Pin = StepMotor_IN1_Pin|StepMotor_IN2_Pin|StepMotor_IN3_Pin|StepMotor_IN4_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
	if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1)  // if the interrupt source is channel1
	{
		HCSR04_MeasureIn(htim);
	}
	else if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_2)  // if the interrupt source is channel2
	{
		HCSR04_MeasureOut(htim);
	}
}
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
