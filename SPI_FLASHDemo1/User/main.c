/*************************************************************
*芯片：STM32F103RC
*实现功能：使用硬件SPI访问板载的flash--W25Q64
*日期：2022-09-22
*************************************************************/

#include "stm32f10x.h"
#include "bsp_usart.h"
#include "bsp_spi_flash.h"

uint8_t readbuff[4096];
uint8_t writebuff[4096];

int main(void)
{
	uint32_t ID = 0;
	uint32_t i=0;
	/*初始化USART*/
	USART_InitConfigure();
	/*初始化SPI*/
	SPI_FLASH_Init();

	/*读取板载Flash的ID号*/
	ID = SPI_Read_FlashID();
	/*发送一个字符串*/
	printf("测试程序：\n板载W25Q64的ID：%d\n",ID);
	
	/*擦除一个扇区*/
	SPI_FLASH_Erase_Sector(0);
	
	/*块擦除未实现，存在问题*/
	//SPI_FLASH_Erase(0,ERASE_FULLBLOCK);
	
	/*写入数据到buff中*/
	for(i=0;i<4096;i++)
	{
		writebuff[i] = i;
	}
	/*把数据写入Flash中，起始地址0，字节数：4096*/
	SPI_BuffWrite_Data(0,writebuff,4096);
	
	/*从零地址处读取4096个字节*/
	SPI_Read_Data(0,readbuff,4096);
	
	/*打印读到的数据，输出到串口*/
	for(i=0;i<4096;i++)
	{
		if((i%10)==0)
			printf("\n");
		printf("0x%x ",readbuff[i]);
	}
	
	while(1)
	{
		
	}
}
