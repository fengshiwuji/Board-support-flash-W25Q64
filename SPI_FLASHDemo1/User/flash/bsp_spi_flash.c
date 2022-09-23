#include "bsp_spi_flash.h"

uint32_t SPI_TimeOut = 0;

/*����SPI GPIO����*/
static void SPI_GPIO_Configure(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	/*����ʱ��*/
	FLASH_SPI_APBxCLK_FUN(FLASH_SPI_CLK,ENABLE);
	FLASH_SPI_GPIO_APBxCLK_FUN(FLASH_SPI_GPIO_CLK,ENABLE);
	/*����SPIʱ�����ţ�ģʽΪ�����������*/
	GPIO_InitStruct.GPIO_Pin = FLASH_SPI_SCK_PIN;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(FLASH_SPI_SCK_PORT, &GPIO_InitStruct);
	/*����SPI MISO���ţ�ģʽΪ�����򸡿�����ģʽ*/
	GPIO_InitStruct.GPIO_Pin = FLASH_SPI_MISO_PIN;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(FLASH_SPI_MISO_PORT, &GPIO_InitStruct);
	/*����SPI MOSI���ţ�ģʽΪ�����������*/
	GPIO_InitStruct.GPIO_Pin = FLASH_SPI_MOSI_PIN;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(FLASH_SPI_MOSI_PORT, &GPIO_InitStruct);
	/*����SPI CSƬѡ���ţ���Ϊ�������ģʽ����CSģʽΪͨ���������*/
	GPIO_InitStruct.GPIO_Pin = FLASH_SPI_CS_PIN;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(FLASH_SPI_CS_PORT, &GPIO_InitStruct);
}

/*����SPI����ģʽ*/
static void SPI_MODE_Configure(void)
{
	SPI_InitTypeDef SPI_InitStruct;
	
	SPI_InitStruct.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;
	SPI_InitStruct.SPI_CPHA = SPI_CPHA_2Edge;	//�ڶ������ز���
	SPI_InitStruct.SPI_CPOL = SPI_CPOL_High;	//CLK����ʱΪ�ߵ�ƽ
	SPI_InitStruct.SPI_CRCPolynomial = 0;		//��ʹ��CRCУ��
	SPI_InitStruct.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitStruct.SPI_Direction = SPI_Direction_2Lines_FullDuplex;	//˫��ȫ˫��
	SPI_InitStruct.SPI_FirstBit = SPI_FirstBit_MSB;	//��λ����
	SPI_InitStruct.SPI_Mode = SPI_Mode_Master;		//����SPIΪ����
	SPI_InitStruct.SPI_NSS = SPI_NSS_Soft;			//Ƭѡѡ���������
	/*��ʼ���ṹ��*/
	SPI_Init(FLASH_SPIx,&SPI_InitStruct);
	
	/*ʹ��SPI*/
	SPI_Cmd(FLASH_SPIx,ENABLE);
}

/*��ʼ��SPI*/
void SPI_FLASH_Init(void)
{
	SPI_GPIO_Configure();
	SPI_MODE_Configure();
}

/*��ʱ��⺯��*/
static uint32_t SPI_TimeOut_UserCallBack(uint8_t errorCode)
{
	printf("SPI�ȴ���ʱ��errorCode:%d\n",errorCode);
	return 0;
}

/*FLASH д��ʹ��*/

void FLASH_Write_Enable(void)
{
	FLASH_SPI_CS_LOW;
	
	SPI_Flash_SendByte(WRITE_ENABLE);
	
	FLASH_SPI_CS_HIGH;
}


/*���Ͳ�����һ���ֽڣ������������ͬ�����е�*/
uint8_t SPI_Flash_SendByte(uint8_t data)
{
	SPI_TimeOut = FLASH_TimeOut;
	/*�ȴ����ͻ�����Ϊ�ա����ͻ������ǿշ���0��Ϊ�շ���1*/
	while(SPI_I2S_GetFlagStatus(FLASH_SPIx, SPI_I2S_FLAG_TXE) == RESET)
	{
		if(SPI_TimeOut-- == 0)
			return (uint8_t)SPI_TimeOut_UserCallBack(0);
	}
	
	SPI_I2S_SendData(FLASH_SPIx, data);
	
	SPI_TimeOut = FLASH_TimeOut;
	
	/*�ȴ����ͻ�����Ϊ�ǿա���ʾ��λ�Ĵ���������ɡ�*/
	while(SPI_I2S_GetFlagStatus(FLASH_SPIx, SPI_I2S_FLAG_RXNE) == RESET)
	{
		if(SPI_TimeOut-- == 0)
			return (uint8_t)SPI_TimeOut_UserCallBack(1);
	}
	return SPI_I2S_ReceiveData(FLASH_SPIx);
}

/*����һ���ֽ�*/

uint8_t SPI_Flash_ReciveByte(void)
{
	return SPI_Flash_SendByte(Dummy);
}
/*��ȡFLASH ID*/
uint32_t SPI_Read_FlashID(void)
{
	uint32_t FlashID = 0;
	FLASH_SPI_CS_LOW;
	
	SPI_Flash_SendByte(READ_JEDEC_ID);
	
	FlashID = SPI_Flash_ReciveByte();
	FlashID<<=8;
	FlashID |= SPI_Flash_ReciveByte();
	FlashID<<=8;
	FlashID |= SPI_Flash_ReciveByte();
	
	FLASH_SPI_CS_HIGH;
	
	return FlashID;
}

/*����FLASHָ������*/

void SPI_FLASH_Erase_Sector(uint32_t SectorAddr)
{
	/*����������Ҫʹ��д�Ĵ���*/
	FLASH_Write_Enable();
	FLASH_SPI_CS_LOW;
	
	SectorAddr *= 4096;
	
	SPI_Flash_SendByte(ERASE_SECTOR);
	
	SPI_Flash_SendByte((SectorAddr>>16)&0xff);
	SPI_Flash_SendByte((SectorAddr>>8)&0xff);
	SPI_Flash_SendByte(SectorAddr&0xff);
	
	FLASH_SPI_CS_HIGH;
	SPI_WaiteForWriteEnd();
}

/*��������*/
void SPI_FLASH_Erase(uint32_t BlockAddr,uint8_t instruction)
{
	/*����������Ҫʹ��д�Ĵ���*/
	FLASH_Write_Enable();
	FLASH_SPI_CS_LOW;
	
	SPI_Flash_SendByte(instruction);
	
	SPI_Flash_SendByte((BlockAddr>>16)&0xff);
	SPI_Flash_SendByte((BlockAddr>>8)&0xff);
	SPI_Flash_SendByte(BlockAddr&0xff);
	
	FLASH_SPI_CS_HIGH;
	SPI_WaiteForWriteEnd();
}

/*�ȴ�flash�ڲ�����ʱ�����*/
void SPI_WaiteForWriteEnd(void)
{
	uint8_t statusReg = 0;
	FLASH_SPI_CS_LOW;
	
	SPI_Flash_SendByte(READ_STATUS);
	do
	{
		statusReg = SPI_Flash_ReciveByte();
	}while((statusReg&0x01)==1); //æµ
	
	FLASH_SPI_CS_HIGH;
}

/*��ȡFLASH������*/
void SPI_Read_Data(uint32_t addr,uint8_t* readBuff,uint32_t numByte)
{
	FLASH_SPI_CS_LOW;
	
	SPI_Flash_SendByte(READ_DATA);
	
	SPI_Flash_SendByte((addr>>16)&0xff);
	SPI_Flash_SendByte((addr>>8)&0xff);
	SPI_Flash_SendByte(addr&0xff);
	
	while(numByte--)
	{
		*readBuff++ = SPI_Flash_ReciveByte();
	}
	
	FLASH_SPI_CS_HIGH;
	
}

/*ҳд�룺��FLASH��д�롣ע��ÿ�����д��256 byte*/
void SPI_PageWrite_Data(uint32_t addr,uint8_t* writeBuff,uint16_t numByte)
{
	FLASH_Write_Enable();
	FLASH_SPI_CS_LOW;
	/*���Ҫд����ֽ�������ҳ�ֽ��������׳��쳣*/
	if(numByte>PAGESIZE)
	{
		printf("numByte morethan PAGESIZE too much��errorCode:%d\n",2);
		numByte = PAGESIZE;
	}
	/*����д����ָ��*/
	SPI_Flash_SendByte(WRITE_DATA);
	
	/*����д�����ݵĵ�ַ*/
	SPI_Flash_SendByte((addr>>16)&0xff);
	SPI_Flash_SendByte((addr>>8)&0xff);
	SPI_Flash_SendByte(addr&0xff);
	
	/*����д��*/
	while(numByte--)
	{
		SPI_Flash_SendByte(*writeBuff++);
	}
	
	FLASH_SPI_CS_HIGH;
	
	/*ע������һ���ȴ�Flash�ڲ�ʱ��������*/
	SPI_WaiteForWriteEnd();
}

/*��ҳд�룬��������256Byte,���迼�ǵ�ַ����*/
void SPI_BuffWrite_Data(uint32_t addr,uint8_t* writeBuff,uint32_t numByte)
{
	uint32_t CountMoreAddr = 0,CountFullPage = 0,CountMorePage = 0;
	
	CountMoreAddr = addr % PAGESIZE;
	CountFullPage = numByte / PAGESIZE;
	CountMorePage = numByte % PAGESIZE;
	/*��ַ�Ƿ���룬������ִ��if,����ִ��else��*/
	if(CountMoreAddr==0)
	{
		/*�����Ƿ���ҳ,�����ٴ���һҳ������ִ��if ����ִ��else��*/
		if(CountMorePage==0)
		{
			while(CountFullPage--)
			{
				SPI_PageWrite_Data(addr,writeBuff,PAGESIZE);
				addr += PAGESIZE;
				writeBuff+=PAGESIZE;
			}
		}
		else
		{
			/*�����Ƿ񳬹�һҳ*/
			if(CountFullPage>0)
			{
				while(CountFullPage--)
				{
					SPI_PageWrite_Data(addr,writeBuff,PAGESIZE);
					addr += PAGESIZE;
					writeBuff+=PAGESIZE;
				}
			}
			/*д�벻��һҳ��ʣ�������*/
			SPI_PageWrite_Data(addr,writeBuff,CountMorePage);
		}
	}
	else
	{
		/*����ִ�е������ζ�ŵ�ַδ���룬�������ܼ򵥣���д�루PAGESIZE-CountMoreAddr�����ֽڵ����ݣ�������һҳ��
		*��������д������ݵĵ�ַ�Ͷ������ˡ�*/
		SPI_PageWrite_Data(addr,writeBuff,PAGESIZE-CountMorePage);
		addr += PAGESIZE-CountMorePage;
		writeBuff += PAGESIZE-CountMorePage;
		CountFullPage = (numByte-PAGESIZE+CountMorePage) / PAGESIZE;
		CountMorePage = (numByte-PAGESIZE+CountMorePage) % PAGESIZE;
		
		if(CountMorePage==0)
		{
			/*��ҳд��*/
			while(CountFullPage--)
			{
				SPI_PageWrite_Data(addr,writeBuff,PAGESIZE);
				addr += PAGESIZE;
				writeBuff+=PAGESIZE;
			}
		}
		else
		{
			/*�����Ƿ񳬹�һҳ*/
			if(CountFullPage>0)
			{
				/*����ҳд��*/
				while(CountFullPage--)
				{
					SPI_PageWrite_Data(addr,writeBuff,PAGESIZE);
					addr += PAGESIZE;
					writeBuff+=PAGESIZE;
				}
			}
			/*д�벻��һҳ��ʣ�������*/
			SPI_PageWrite_Data(addr,writeBuff,CountMorePage);
		}
	}
}


