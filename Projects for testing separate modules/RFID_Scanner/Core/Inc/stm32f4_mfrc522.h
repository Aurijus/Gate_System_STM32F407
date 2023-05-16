/**
 * MFRC522 RFID Card reader
 * MF RC522 Default pinout
 * 		MFRC522		STM32F4XX	DESCRIPTION
 *		CS (SDA)	PA4			Chip select for SPI
 *		SCK			PA5			Serial Clock for SPI
 *		MISO		PA6			Master In Slave Out for SPI
 *		MOSI		PA7			Master Out Slave In for SPI
 *		GND			GND			Ground
 *		VCC			3.3V		3.3V power
 *		RST			PC4			Reset pin
 */

#ifndef MFRC522_H
#define MFRC522_H 100
/**
 * Library dependencies
 * - STM32F4xx
 * - STM32F407xx
 * - STM32F4xx HAL GPIO
 * - STM32F4xx HAL SPI
 */

/* Includes */
#include "stm32f4xx.h"
#include "stm32f407xx.h"
#include "stm32f4xx_hal_gpio.h"
#include "stm32f4xx_hal_spi.h"

/* Default CS pin used */
#define MFRC522_CS_PORT					GPIOA
#define MFRC522_CS_PIN					GPIO_PIN_4

/* Default RST pin used */
#define MFRC522_RESET_PORT				GPIOC
#define MFRC522_RESET_PIN				GPIO_PIN_4

/* Interfaces used */
extern SPI_HandleTypeDef hspi1;
extern UART_HandleTypeDef huart2;

/**
 * Status enumeration
 *
 * Used with most functions
 */
typedef enum {
	MI_OK = 0,
	MI_NOTAGERR = 1,
	MI_ERR = 2
} MFRC522_Status_t;

/* MFRC522 Commands */
#define PCD_IDLE						0x00   //NO action; Cancel the current command
#define PCD_AUTHENT						0x0E   //Authentication Key
#define PCD_RECEIVE						0x08   //Receive Data
#define PCD_TRANSMIT					0x04   //Transmit data
#define PCD_TRANSCEIVE					0x0C   //Transmit and receive data,
#define PCD_RESETPHASE					0x0F   //Reset
#define PCD_CALCCRC						0x03   //CRC Calculate

/* Mifare_One card command word */
#define PICC_REQIDL						0x26   // find the antenna area does not enter hibernation
#define PICC_REQALL						0x52   // find all the cards antenna area
#define PICC_ANTICOLL					0x93   // anti-collision
#define PICC_SELECTTAG					0x93   // election card
#define PICC_AUTHENT1A					0x60   // authentication key A
#define PICC_AUTHENT1B					0x61   // authentication key B
#define PICC_READ						0x30   // Read Block
#define PICC_WRITE						0xA0   // write block
#define PICC_DECREMENT					0xC0   // debit
#define PICC_INCREMENT					0xC1   // recharge
#define PICC_RESTORE					0xC2   // transfer block data to the buffer
#define PICC_TRANSFER					0xB0   // save the data in the buffer
#define PICC_HALT						0x50   // Sleep

/* MFRC522 Registers */
//Page 0: Command and Status
#define MFRC522_REG_RESERVED00			0x00    
#define MFRC522_REG_COMMAND				0x01    
#define MFRC522_REG_COMM_IE_N			0x02    
#define MFRC522_REG_DIV1_EN				0x03    
#define MFRC522_REG_COMM_IRQ			0x04    
#define MFRC522_REG_DIV_IRQ				0x05
#define MFRC522_REG_ERROR				0x06    
#define MFRC522_REG_STATUS1				0x07    
#define MFRC522_REG_STATUS2				0x08    
#define MFRC522_REG_FIFO_DATA			0x09
#define MFRC522_REG_FIFO_LEVEL			0x0A
#define MFRC522_REG_WATER_LEVEL			0x0B
#define MFRC522_REG_CONTROL				0x0C
#define MFRC522_REG_BIT_FRAMING			0x0D
#define MFRC522_REG_COLL				0x0E
#define MFRC522_REG_RESERVED01			0x0F
//Page 1: Command 
#define MFRC522_REG_RESERVED10			0x10
#define MFRC522_REG_MODE				0x11
#define MFRC522_REG_TX_MODE				0x12
#define MFRC522_REG_RX_MODE				0x13
#define MFRC522_REG_TX_CONTROL			0x14
#define MFRC522_REG_TX_AUTO				0x15
#define MFRC522_REG_TX_SELL				0x16
#define MFRC522_REG_RX_SELL				0x17
#define MFRC522_REG_RX_THRESHOLD		0x18
#define MFRC522_REG_DEMOD				0x19
#define MFRC522_REG_RESERVED11			0x1A
#define MFRC522_REG_RESERVED12			0x1B
#define MFRC522_REG_MIFARE				0x1C
#define MFRC522_REG_RESERVED13			0x1D
#define MFRC522_REG_RESERVED14			0x1E
#define MFRC522_REG_SERIALSPEED			0x1F
//Page 2: CFG    
#define MFRC522_REG_RESERVED20			0x20  
#define MFRC522_REG_CRC_RESULT_M		0x21
#define MFRC522_REG_CRC_RESULT_L		0x22
#define MFRC522_REG_RESERVED21			0x23
#define MFRC522_REG_MOD_WIDTH			0x24
#define MFRC522_REG_RESERVED22			0x25
#define MFRC522_REG_RF_CFG				0x26
#define MFRC522_REG_GS_N				0x27
#define MFRC522_REG_CWGS_PREG			0x28
#define MFRC522_REG__MODGS_PREG			0x29
#define MFRC522_REG_T_MODE				0x2A
#define MFRC522_REG_T_PRESCALER			0x2B
#define MFRC522_REG_T_RELOAD_H			0x2C
#define MFRC522_REG_T_RELOAD_L			0x2D
#define MFRC522_REG_T_COUNTER_VALUE_H	0x2E
#define MFRC522_REG_T_COUNTER_VALUE_L	0x2F
//Page 3:TestRegister 
#define MFRC522_REG_RESERVED30			0x30
#define MFRC522_REG_TEST_SEL1			0x31
#define MFRC522_REG_TEST_SEL2			0x32
#define MFRC522_REG_TEST_PIN_EN			0x33
#define MFRC522_REG_TEST_PIN_VALUE		0x34
#define MFRC522_REG_TEST_BUS			0x35
#define MFRC522_REG_AUTO_TEST			0x36
#define MFRC522_REG_VERSION				0x37
#define MFRC522_REG_ANALOG_TEST			0x38
#define MFRC522_REG_TEST_ADC1			0x39  
#define MFRC522_REG_TEST_ADC2			0x3A   
#define MFRC522_REG_TEST_ADC0			0x3B   
#define MFRC522_REG_RESERVED31			0x3C   
#define MFRC522_REG_RESERVED32			0x3D
#define MFRC522_REG_RESERVED33			0x3E   
#define MFRC522_REG_RESERVED34			0x3F
//Dummy byte
#define MFRC522_DUMMY					0x00
#define MFRC522_STATUS_OK 				0x00
#define MFRC522_STATUS_ERROR			0x01
#define MFRC522_REG_BLOCK_ADDR    		0x01

#define MFRC522_MIFARE_KEY_SIZE 6
#define MFRC522_MAX_LEN					16

/**
 * Public functions
 */

/**
 * Initialize MFRC522 RFID reader
 *
 * Prepare MFRC522 to work with RFIDs
 *
 */
extern void MFRC522_Init(void);

/**
 * Check for RFID card existence
 *
 * Parameters:
 * 	- uint8_t* id:
 * 		Pointer to 4bytes long memory to store valid card id in.
 * 		ID is valid only if card is detected, so when function returns MI_OK
 *
 * Returns MI_OK if card is detected
 */
extern MFRC522_Status_t ReadCardValue(void);

/**
 * Compare 2 RFID ID's
 * Useful if you have known ID (database with allowed IDs), to compare detected card with with your ID
 *
 * Parameters:
 * 	- uint8_t* CardID:
 * 		Pointer to 4bytes detected card ID
 * 	- uint8_t* CompareID:
 * 		Pointer to 4bytes your ID
 *
 * Returns MI_OK if IDs are the same, or MI_ERR if not
 */
extern MFRC522_Status_t MFRC522_Compare(uint8_t* CardID, uint8_t* CompareID);

/**
 * Private functions
 */
extern void MFRC522_WriteRegister(uint8_t addr, uint8_t val);
extern uint8_t MFRC522_ReadRegister(uint8_t addr);
extern void MFRC522_SetBitMask(uint8_t reg, uint8_t mask);
extern void MFRC522_ClearBitMask(uint8_t reg, uint8_t mask);
extern void MFRC522_AntennaOn(void);
extern void MFRC522_AntennaOff(void);
extern void MFRC522_Reset(void);
extern MFRC522_Status_t MFRC522_Request(uint8_t reqMode, uint8_t* TagType);
extern MFRC522_Status_t MFRC522_ToCard(uint8_t command, uint8_t* sendData, uint8_t sendLen, uint8_t* backData, uint16_t* backLen);
extern MFRC522_Status_t MFRC522_Anticoll(uint8_t* serNum);
extern void MFRC522_CalculateCRC(uint8_t* pIndata, uint8_t len, uint8_t* pOutData);
extern uint8_t MFRC522_SelectTag(uint8_t* serNum);
extern MFRC522_Status_t MFRC522_Auth(uint8_t authMode, uint8_t BlockAddr, uint8_t* Sectorkey, uint8_t* serNum);
extern MFRC522_Status_t MFRC522_Read(uint8_t blockAddr, uint8_t* recvData);
extern MFRC522_Status_t MFRC522_Write(uint8_t blockAddr, uint8_t* writeData);
extern void MFRC522_Halt(void);

#endif
