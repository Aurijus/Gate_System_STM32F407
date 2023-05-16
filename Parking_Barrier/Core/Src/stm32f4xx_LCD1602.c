/*
 * stm32f4xx_LCD1602.c
 *
 *  Created on: May 13, 2023
 *      Author: Aurijus
 */

#include "stm32f4xx_LCD1602.h"
#include "stm32f4xx_microDelay.h"

  void LCD1602_TransferToLCD(char data, int rs)
  {
	  /* rs = 1 for data, rs=0 for command */
      HAL_GPIO_WritePin(RS_PORT, RS_PIN, rs);

      /* write the data to the respective pin */
      HAL_GPIO_WritePin(D7_PORT, D7_PIN, ((data>>3)&0x01));
      HAL_GPIO_WritePin(D6_PORT, D6_PIN, ((data>>2)&0x01));
      HAL_GPIO_WritePin(D5_PORT, D5_PIN, ((data>>1)&0x01));
      HAL_GPIO_WritePin(D4_PORT, D4_PIN, ((data>>0)&0x01));

      /* Toggle EN PIN to send the data
       * if the HCLK > 100 MHz, use the  20 us delay.
       * Increase if malfunctioning.
       */
      HAL_GPIO_WritePin(EN_PORT, EN_PIN, 1);
      microDelay(100);
      HAL_GPIO_WritePin(EN_PORT, EN_PIN, 0);
      microDelay(100);
  }

  void LCD1602_SendCommand(char cmd)
  {
      char command;
      
      /* Send upper bit */
      command = ((cmd>>4)&0x0f);
      
      /* RS must be 0 while sending a command */
      LCD1602_TransferToLCD(command,0);
      
      /* Send lower bit */
      command = ((cmd)&0x0f);
      LCD1602_TransferToLCD(command, 0);
  }

  void LCD1602_SendData(char data)
  {
      char sendData;

      /* Send upper bit */
      sendData = ((data>>4)&0x0f);

      /* RS must be 1 while sending data */
      LCD1602_TransferToLCD(sendData, 1);

      /* Send lower bit */
      sendData = ((data)&0x0f);
      LCD1602_TransferToLCD(sendData, 1);
  }

  void LCD1602_CursorPosition(int row, int col)
  {
      switch (row)
      {
          case 0:
              col |= 0x80;
              break;
          case 1:
              col |= 0xC0;
              break;
      }
      LCD1602_SendCommand(col);
  }

  void LCD1602_Clear(void)
  {
	  LCD1602_SendCommand(0x01);
	  HAL_Delay(2);
  }

  void LCD1602_Init(void)
  {
      // 4 bit initialization
      HAL_Delay(50);  // wait for >40ms
      LCD1602_SendCommand (0x30);
      HAL_Delay(5);  // wait for >4.1ms
      LCD1602_SendCommand (0x30);
      HAL_Delay(1);  // wait for >100us
      LCD1602_SendCommand (0x30);
      HAL_Delay(10);
      LCD1602_SendCommand (0x20);  // 4bit mode
      HAL_Delay(10);

    // Display initialization
      LCD1602_SendCommand (0x28); // Function set --> DL=0 (4 bit mode), N = 1 (2 line display) F = 0 (5x8 characters)
      HAL_Delay(1);
      LCD1602_SendCommand (0x08); //Display on/off control --> D=0,C=0, B=0  ---> display off
      HAL_Delay(1);
      LCD1602_SendCommand (0x01);  // clear display
      HAL_Delay(1);
      LCD1602_SendCommand (0x06); //Entry mode set --> I/D = 1 (increment cursor) & S = 0 (no shift)
      HAL_Delay(1);
      LCD1602_SendCommand (0x0C); //Display on/off control --> D = 1, C and B = 0. (Cursor and blink, last two bits)
  }

  void LCD1602_TransferString(char *str)
  {
	  while(*str)
		  LCD1602_SendData(*str++);
  }
