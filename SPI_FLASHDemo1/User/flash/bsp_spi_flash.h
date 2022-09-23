#ifndef __BSP_SPI_FLASH_H__
#define __BSP_SPI_FLASH_H__

#include "stm32f10x.h"
#include "bsp_usart.h"


/*SPI外设宏定义*/
#define FLASH_SPIx						SPI2			
/*外设时钟函数宏定义*/
#define FLASH_SPI_APBxCLK_FUN			RCC_APB1PeriphClockCmd
/*SPI外设时钟*/
#define FLASH_SPI_CLK					RCC_APB1Periph_SPI2
/*端口时钟函数*/
#define FLASH_SPI_GPIO_APBxCLK_FUN		RCC_APB2PeriphClockCmd

/*端口时钟*/
#define FLASH_SPI_GPIO_CLK				RCC_APB2Periph_GPIOB

/*端口及引脚*/
#define FLASH_SPI_SCK_PORT				GPIOB	
#define FLASH_SPI_SCK_PIN				GPIO_Pin_13

#define FLASH_SPI_MISO_PORT				GPIOB
#define FLASH_SPI_MISO_PIN				GPIO_Pin_14

#define FLASH_SPI_MOSI_PORT				GPIOB
#define FLASH_SPI_MOSI_PIN				GPIO_Pin_15

#define FLASH_SPI_CS_PORT				GPIOB
#define FLASH_SPI_CS_PIN				GPIO_Pin_12

#define FLASH_TimeOut			(uint32_t)10000
#define Dummy					(uint8_t)0x00
#define PAGESIZE				(uint16_t)256

#define FLASH_SPI_CS_HIGH		GPIO_SetBits(FLASH_SPI_CS_PORT,FLASH_SPI_CS_PIN);
#define FLASH_SPI_CS_LOW		GPIO_ResetBits(FLASH_SPI_CS_PORT,FLASH_SPI_CS_PIN);

#define READ_JEDEC_ID			(uint8_t)0x9F
#define ERASE_SECTOR			(uint8_t)0x20
#define ERASE_FULLBLOCK			(uint8_t)0xD8
#define ERASE_HALFBLOCK			(uint8_t)0x52
//#define ERASE_CHIP				(uint8_t)0xC7
#define ERASE_CHIP				(uint8_t)0x60
#define ERASE_SECTOR			(uint8_t)0x20
#define READ_STATUS				(uint8_t)0x05
#define READ_DATA				(uint8_t)0x03
#define WRITE_ENABLE			(uint8_t)0x06
#define WRITE_DATA				(uint8_t)0x02


void SPI_FLASH_Init(void);
uint32_t SPI_Read_FlashID(void);
uint8_t SPI_Flash_SendByte(uint8_t data);
uint8_t SPI_Flash_ReciveByte(void);
void SPI_WaiteForWriteEnd(void);
void SPI_FLASH_Erase_Sector(uint32_t SectorAddr);
void SPI_FLASH_Erase(uint32_t BlockAddr,uint8_t instruction);
void SPI_Read_Data(uint32_t addr,uint8_t* readBuff,uint32_t numByte);
void SPI_PageWrite_Data(uint32_t addr,uint8_t* writeBuff,uint16_t numByte);
void SPI_BuffWrite_Data(uint32_t addr,uint8_t* writeBuff,uint32_t numByte);

#endif /* __BSP_SPI_FLASH_H__ */

