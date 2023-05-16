/*
 * stm32f4xx_mfrc522.c
 *
 *  Created on: May 12, 2023
 *      Author: Aurijus
 */

#include "stm32f4_mfrc522.h"
#include "stdio.h"

uint8_t CardID[4];
uint8_t CardID_Print[50] = {'\0'};
uint8_t buffer_Detection[50] = {'\0'};


  MFRC522_Status_t MFRC522_Compare(uint8_t* CardID, uint8_t* CompareID)
  {
  	uint8_t i;

  	for (i = 0; i < 4; i++)
  		if (CardID[i] != CompareID[i])
  			return MI_ERR;

  	return MI_OK;
  }

  void MFRC522_WriteRegister(uint8_t addr, uint8_t val)
  {
      uint8_t data[2];

      data[0] = (addr << 1) & 0x7E;  // Set address and clear write bit
      data[1] = val;

      HAL_GPIO_WritePin(MFRC522_CS_PORT, MFRC522_CS_PIN, GPIO_PIN_RESET);
      HAL_SPI_Transmit(&hspi1, data, 2, HAL_MAX_DELAY);
      HAL_GPIO_WritePin(MFRC522_CS_PORT, MFRC522_CS_PIN, GPIO_PIN_SET);
  }

  uint8_t MFRC522_ReadRegister(uint8_t reg)
  {
      // Create the command byte by setting the MSB to 0 for a read operation
      uint8_t command = ((reg << 1) & 0x7E) | 0x80;
      uint8_t buf[2] = {command, 0};

      // Transmit the command byte to read the register value
      HAL_GPIO_WritePin(MFRC522_CS_PORT, MFRC522_CS_PIN, GPIO_PIN_RESET);
      HAL_SPI_TransmitReceive(&hspi1, buf, buf, 2, HAL_MAX_DELAY);
      HAL_GPIO_WritePin(MFRC522_CS_PORT, MFRC522_CS_PIN, GPIO_PIN_SET);

      return buf[1];
  }

  void MFRC522_SetBitMask(uint8_t reg, uint8_t mask)
  {
  	MFRC522_WriteRegister(reg, MFRC522_ReadRegister(reg) | mask);
  }

  void MFRC522_ClearBitMask(uint8_t reg, uint8_t mask)
  {
  	MFRC522_WriteRegister(reg, MFRC522_ReadRegister(reg) & (~mask));
  }

  void MFRC522_AntennaOn(void)
  {
  	uint8_t temp;

  	temp = MFRC522_ReadRegister(MFRC522_REG_TX_CONTROL);

  	if (!(temp & 0x03))
  		MFRC522_WriteRegister(MFRC522_REG_TX_CONTROL, temp | 0x03);

  }

  void MFRC522_AntennaOff(void)
  {
  	MFRC522_ClearBitMask(MFRC522_REG_TX_CONTROL, 0x03);
  }

  void MFRC522_Reset(void)
  {
  	MFRC522_WriteRegister(MFRC522_REG_COMMAND, PCD_RESETPHASE);
  }


  MFRC522_Status_t MFRC522_ToCard(uint8_t command, uint8_t* sendData, uint8_t sendLen, uint8_t* backData, uint16_t* backLen)
  {
  	MFRC522_Status_t status = MI_ERR;
  	uint8_t irqEn = 0x00;
  	uint8_t waitIRq = 0x00;
  	uint8_t lastBits;
  	uint8_t n;
  	uint16_t i;

  	switch (command)
  	{
  		case PCD_AUTHENT:
  		{
  			irqEn = 0x12;
  			waitIRq = 0x10;
  			break;
  		}
  		case PCD_TRANSCEIVE:
  		{
  			irqEn = 0x77;
  			waitIRq = 0x30;
  			break;
  		}
  		default:
  			break;
  	}

  	MFRC522_WriteRegister(MFRC522_REG_COMM_IE_N, irqEn | 0x80);
  	MFRC522_ClearBitMask(MFRC522_REG_COMM_IRQ, 0x80);
  	MFRC522_SetBitMask(MFRC522_REG_FIFO_LEVEL, 0x80);

  	MFRC522_WriteRegister(MFRC522_REG_COMMAND, PCD_IDLE);

  	//Writing data to the FIFO
  	for (i = 0; i < sendLen; i++)
  		MFRC522_WriteRegister(MFRC522_REG_FIFO_DATA, sendData[i]);

  	//Execute the command
  	MFRC522_WriteRegister(MFRC522_REG_COMMAND, command);

  	if (command == PCD_TRANSCEIVE)
  		MFRC522_SetBitMask(MFRC522_REG_BIT_FRAMING, 0x80);		//StartSend=1,transmission of data starts

  	//Waiting to receive data to complete
  	i = 2000;	//i according to the clock frequency adjustment, the operator M1 card maximum waiting time 25ms???
  	do {
  		//CommIrqReg[7..0]
  		//Set1 TxIRq RxIRq IdleIRq HiAlerIRq LoAlertIRq ErrIRq TimerIRq
  		n = MFRC522_ReadRegister(MFRC522_REG_COMM_IRQ);
  		i--;
  	} while ((i!=0) && !(n&0x01) && !(n&waitIRq));

  	MFRC522_ClearBitMask(MFRC522_REG_BIT_FRAMING, 0x80);			//StartSend=0

  	if (i != 0)  {
  		if (!(MFRC522_ReadRegister(MFRC522_REG_ERROR) & 0x1B)) {

  			status = MI_OK;

  			if (n & irqEn & 0x01)
  				status = MI_NOTAGERR;

  			if (command == PCD_TRANSCEIVE) {
  				n = MFRC522_ReadRegister(MFRC522_REG_FIFO_LEVEL);
  				lastBits = MFRC522_ReadRegister(MFRC522_REG_CONTROL) & 0x07;

  				if (lastBits)
  					*backLen = (n - 1) * 8 + lastBits;
  				else
  					*backLen = n * 8;


  				if (n == 0)
  					n = 1;

  				if (n > MFRC522_MAX_LEN)
  					n = MFRC522_MAX_LEN;

  				//Reading the received data in FIFO
  				for (i = 0; i < n; i++)
  					backData[i] = MFRC522_ReadRegister(MFRC522_REG_FIFO_DATA);
  			}
  		}
  		else
  			status = MI_ERR;

  	}

  	return status;
  }

  MFRC522_Status_t MFRC522_Anticoll(uint8_t *serNum)
  {
      MFRC522_Status_t status;
      uint8_t i, check = 0x00;
      uint8_t size = sizeof(serNum);

      //Clear interrupts
      MFRC522_WriteRegister(MFRC522_REG_BIT_FRAMING, 0x00);

      //Send Request command
      serNum[0] = PICC_ANTICOLL;
      serNum[1] = 0x20;
      status = MFRC522_ToCard(PCD_TRANSCEIVE, serNum, 2, serNum, &size);

      //If status is OK
      if (status == MI_OK) {
          //Check received data with check byte
          for (i = 0; i < 4; i++)
              check ^= serNum[i];

          if (check != serNum[i])
              status = MI_ERR;

      }

      return status;
  }

  void MFRC522_CalculateCRC(uint8_t *data, uint8_t length, uint8_t *result)
  {
	  uint8_t i, n;

	  	MFRC522_ClearBitMask(MFRC522_REG_DIV_IRQ, 0x04);			//CRCIrq = 0
	  	MFRC522_SetBitMask(MFRC522_REG_FIFO_LEVEL, 0x80);			//Clear the FIFO pointer
	  	//Write_MFRC522(CommandReg, PCD_IDLE);

	  	//Writing data to the FIFO
	  	for (i = 0; i < length; i++)
	  		MFRC522_WriteRegister(MFRC522_REG_FIFO_DATA, *(data+i));

	  	MFRC522_WriteRegister(MFRC522_REG_COMMAND, PCD_CALCCRC);

	  	//Wait CRC calculation is complete
	  	i = 0xFF;

	  	do
	  	{
	  		n = MFRC522_ReadRegister(MFRC522_REG_DIV_IRQ);
	  		i--;
	  	} while ((i!=0) && !(n&0x04));			//CRCIrq = 1

	  	//Read CRC calculation result
	  	result[0] = MFRC522_ReadRegister(MFRC522_REG_CRC_RESULT_L);
	  	result[1] = MFRC522_ReadRegister(MFRC522_REG_CRC_RESULT_M);
  }

  MFRC522_Status_t MFRC522_SelectTag(uint8_t *uid)
  {
      uint8_t status, i;
      uint8_t buffer[9];
      uint8_t size;

      buffer[0] = PICC_SELECTTAG;
      buffer[1] = 0x70;

      for (i = 0; i < 5; i++)
          buffer[i+2] = *(uid+i);

      MFRC522_CalculateCRC(buffer, 7, &buffer[7]);

      status = MFRC522_ToCard(PCD_TRANSCEIVE, buffer, 9, buffer, &size);

      if ((status == MI_OK) && (size == 0x18))
          return MI_OK;
      else
          return MI_ERR;
  }

  MFRC522_Status_t MFRC522_Auth(uint8_t authMode, uint8_t blockAddr, uint8_t *sectorKey, uint8_t *uid)
  {
	  MFRC522_Status_t status;
	  uint16_t recvBits;
	  uint8_t i;
	  uint8_t buff[12];

      //Verify the command block address + sector + password + card serial number
      buff[0] = authMode;
      buff[1] = blockAddr;

      for (i = 0; i < 6; i++)
      	buff[i+2] = *(sectorKey+i);

      for (i = 0; i < 4; i++)
      	buff[i+8] = *(uid+i);

      status = MFRC522_ToCard(PCD_AUTHENT, buff, 12, buff, &recvBits);

      if ((status != MI_OK) || (!(MFRC522_ReadRegister(MFRC522_REG_STATUS2) & 0x08)))
      {
        	status = MI_ERR;
      }


      	return status;
  }

  MFRC522_Status_t MFRC522_Read(uint8_t blockAddr, uint8_t *recvData)
  {
	  MFRC522_Status_t status;
	  	uint16_t unLen;

	  	recvData[0] = PICC_READ;
	  	recvData[1] = blockAddr;
	  	MFRC522_CalculateCRC(recvData,2, &recvData[2]);
	  	status = MFRC522_ToCard(PCD_TRANSCEIVE, recvData, 4, recvData, &unLen);

	  	if ((status != MI_OK) || (unLen != 0x90))
	  		status = MI_ERR;

	  	return status;
  }


  MFRC522_Status_t MFRC522_Write(uint8_t blockAddr, uint8_t* writeData)
  {
  	MFRC522_Status_t status;
  	uint16_t recvBits;
  	uint8_t i;
  	uint8_t buff[18];

  	buff[0] = PICC_WRITE;
  	buff[1] = blockAddr;
  	MFRC522_CalculateCRC(buff, 2, &buff[2]);
  	status = MFRC522_ToCard(PCD_TRANSCEIVE, buff, 4, buff, &recvBits);

  	if ((status != MI_OK) || (recvBits != 4) || ((buff[0] & 0x0F) != 0x0A))
  		status = MI_ERR;

  	if (status == MI_OK)
  	{
  		//Data to the FIFO write 16Byte
  		for (i = 0; i < 16; i++)
  			buff[i] = *(writeData+i);

  		MFRC522_CalculateCRC(buff, 16, &buff[16]);
  		status = MFRC522_ToCard(PCD_TRANSCEIVE, buff, 18, buff, &recvBits);

  		if ((status != MI_OK) || (recvBits != 4) || ((buff[0] & 0x0F) != 0x0A))
  			status = MI_ERR;
  	}
  	return status;
  }

  void MFRC522_Halt(void)
  {
  	uint16_t unLen;
  	uint8_t buff[4];

  	buff[0] = PICC_HALT;
  	buff[1] = 0;
  	MFRC522_CalculateCRC(buff, 2, &buff[2]);

  	MFRC522_ToCard(PCD_TRANSCEIVE, buff, 4, buff, &unLen);
  }

  void MFRC522_Init(void)
  {
	  // Set RST pin high to bring MFRC522 out of reset
	  HAL_GPIO_WritePin(MFRC522_RESET_PORT, MFRC522_RESET_PIN, GPIO_PIN_SET);
	  // Set CS pin low to enable communication with MFRC522
	  HAL_GPIO_WritePin(MFRC522_CS_PORT, MFRC522_CS_PIN, GPIO_PIN_RESET);

	  MFRC522_Reset();

	  // Initialize MFRC522
	  MFRC522_WriteRegister(MFRC522_REG_T_MODE, 0x8D);
	  MFRC522_WriteRegister(MFRC522_REG_T_PRESCALER, 0x3E);
	  MFRC522_WriteRegister(MFRC522_REG_T_RELOAD_L, 30);
	  MFRC522_WriteRegister(MFRC522_REG_T_RELOAD_H, 0);
	  MFRC522_WriteRegister(MFRC522_REG_TX_AUTO, 0x40);
	  MFRC522_WriteRegister(MFRC522_REG_MODE, 0x3D);

	  // Enable antenna
	  MFRC522_AntennaOn();
	  // Set CS pin high to disable MFRC522
	  HAL_GPIO_WritePin(MFRC522_CS_PORT, MFRC522_CS_PIN, GPIO_PIN_SET);
    }

  MFRC522_Status_t MFRC522_Request(uint8_t reqMode, uint8_t* TagType) {
      MFRC522_Status_t status;
      uint16_t backBits; // The received data bits

      MFRC522_WriteRegister(MFRC522_REG_BIT_FRAMING, 0x07);

      TagType[0] = reqMode;
      status = MFRC522_ToCard(PCD_TRANSCEIVE, TagType, 1, TagType, &backBits);

      if (status != MI_OK || backBits != 0x10)
          status = MI_ERR;

      return status;
  }

  MFRC522_Status_t ReadCardValue(void)
  {
	  MFRC522_Init();
      MFRC522_Status_t status = MI_ERR;
      uint8_t buffer[18] = {0};
      uint8_t serial[5] = {0};
      uint8_t key[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
      uint8_t sector = 3;
      uint8_t block = 0;

      // Check for card
      status = MFRC522_Request(PICC_REQIDL, buffer);
      if (status != MI_OK)
      {
    	  sprintf((char*)buffer_Detection,"No card found");
          return status;
      }

      // Get card serial number
      status = MFRC522_Anticoll(serial);
      if (status != MI_OK)
      {
    	  sprintf((char*)buffer_Detection,"Error reading card serial number\r\n");
          return status;
      }

      // Select card
      status = MFRC522_SelectTag(serial);
      if (status != MI_OK)
      {
    	  sprintf((char*)buffer_Detection,"Error selecting card\r\n");
          return status;
      }

      // Authenticate with card
      status = MFRC522_Auth(PICC_AUTHENT1A, sector, key, serial);
      if (status != MI_OK)
      {
    	  sprintf((char*)buffer_Detection,"Error authenticating with card\r\n");
          return status;
      }

      // Read value from card
      status = MFRC522_Read(block, buffer);
      if (status != MI_OK)
      {
    	  sprintf((char*)buffer_Detection,"Error reading card value\r\n");
          return status;
      }

      // Process value and output
      for (int  i = 0 ; i < 4 ; i++)
    	  CardID[i] = buffer[i]; // Assigning the UID to a global variable

      sprintf(CardID_Print, "%02x\t%02x\t%02x\t%02x", (uint32_t)buffer[0], (uint32_t)buffer[1], (uint32_t)buffer[2], (uint32_t)buffer[3]);
      status = MI_OK;
      sprintf((char*)buffer_Detection,"Card Detected!");
      return status;
  }
