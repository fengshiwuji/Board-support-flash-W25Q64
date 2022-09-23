/*************************************************************
*оƬ��STM32F103RC
*ʵ�ֹ��ܣ�ʹ��Ӳ��SPI���ʰ��ص�flash--W25Q64
*���ڣ�2022-09-22
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
	/*��ʼ��USART*/
	USART_InitConfigure();
	/*��ʼ��SPI*/
	SPI_FLASH_Init();

	/*��ȡ����Flash��ID��*/
	ID = SPI_Read_FlashID();
	/*����һ���ַ���*/
	printf("���Գ���\n����W25Q64��ID��%d\n",ID);
	
	/*����һ������*/
	SPI_FLASH_Erase_Sector(0);
	
	/*�����δʵ�֣���������*/
	//SPI_FLASH_Erase(0,ERASE_FULLBLOCK);
	
	/*д�����ݵ�buff��*/
	for(i=0;i<4096;i++)
	{
		writebuff[i] = i;
	}
	/*������д��Flash�У���ʼ��ַ0���ֽ�����4096*/
	SPI_BuffWrite_Data(0,writebuff,4096);
	
	/*�����ַ����ȡ4096���ֽ�*/
	SPI_Read_Data(0,readbuff,4096);
	
	/*��ӡ���������ݣ����������*/
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
