#include "bsp_spi_flash.h"

uint32_t SPI_TimeOut = 0;

/*配置SPI GPIO引脚*/
static void SPI_GPIO_Configure(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	/*开启时钟*/
	FLASH_SPI_APBxCLK_FUN(FLASH_SPI_CLK,ENABLE);
	FLASH_SPI_GPIO_APBxCLK_FUN(FLASH_SPI_GPIO_CLK,ENABLE);
	/*配置SPI时钟引脚，模式为复用推挽输出*/
	GPIO_InitStruct.GPIO_Pin = FLASH_SPI_SCK_PIN;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(FLASH_SPI_SCK_PORT, &GPIO_InitStruct);
	/*配置SPI MISO引脚，模式为上拉或浮空输入模式*/
	GPIO_InitStruct.GPIO_Pin = FLASH_SPI_MISO_PIN;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(FLASH_SPI_MISO_PORT, &GPIO_InitStruct);
	/*配置SPI MOSI引脚，模式为复用推挽输出*/
	GPIO_InitStruct.GPIO_Pin = FLASH_SPI_MOSI_PIN;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(FLASH_SPI_MOSI_PORT, &GPIO_InitStruct);
	/*配置SPI CS片选引脚，因为采用软件模式，故CS模式为通用推挽输出*/
	GPIO_InitStruct.GPIO_Pin = FLASH_SPI_CS_PIN;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(FLASH_SPI_CS_PORT, &GPIO_InitStruct);
}

/*配置SPI工作模式*/
static void SPI_MODE_Configure(void)
{
	SPI_InitTypeDef SPI_InitStruct;
	
	SPI_InitStruct.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;
	SPI_InitStruct.SPI_CPHA = SPI_CPHA_2Edge;	//第二个边沿采样
	SPI_InitStruct.SPI_CPOL = SPI_CPOL_High;	//CLK空闲时为高电平
	SPI_InitStruct.SPI_CRCPolynomial = 0;		//不使用CRC校验
	SPI_InitStruct.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitStruct.SPI_Direction = SPI_Direction_2Lines_FullDuplex;	//双线全双工
	SPI_InitStruct.SPI_FirstBit = SPI_FirstBit_MSB;	//高位先行
	SPI_InitStruct.SPI_Mode = SPI_Mode_Master;		//配置SPI为主机
	SPI_InitStruct.SPI_NSS = SPI_NSS_Soft;			//片选选用软件配置
	/*初始化结构体*/
	SPI_Init(FLASH_SPIx,&SPI_InitStruct);
	
	/*使能SPI*/
	SPI_Cmd(FLASH_SPIx,ENABLE);
}

/*初始化SPI*/
void SPI_FLASH_Init(void)
{
	SPI_GPIO_Configure();
	SPI_MODE_Configure();
}

/*超时检测函数*/
static uint32_t SPI_TimeOut_UserCallBack(uint8_t errorCode)
{
	printf("SPI等待超时！errorCode:%d\n",errorCode);
	return 0;
}

/*FLASH 写入使能*/

void FLASH_Write_Enable(void)
{
	FLASH_SPI_CS_LOW;
	
	SPI_Flash_SendByte(WRITE_ENABLE);
	
	FLASH_SPI_CS_HIGH;
}


/*发送并接收一个字节，发送与接收是同步进行的*/
uint8_t SPI_Flash_SendByte(uint8_t data)
{
	SPI_TimeOut = FLASH_TimeOut;
	/*等待发送缓冲器为空。发送缓冲器非空返回0，为空返回1*/
	while(SPI_I2S_GetFlagStatus(FLASH_SPIx, SPI_I2S_FLAG_TXE) == RESET)
	{
		if(SPI_TimeOut-- == 0)
			return (uint8_t)SPI_TimeOut_UserCallBack(0);
	}
	
	SPI_I2S_SendData(FLASH_SPIx, data);
	
	SPI_TimeOut = FLASH_TimeOut;
	
	/*等待发送缓冲器为非空。表示移位寄存器发送完成。*/
	while(SPI_I2S_GetFlagStatus(FLASH_SPIx, SPI_I2S_FLAG_RXNE) == RESET)
	{
		if(SPI_TimeOut-- == 0)
			return (uint8_t)SPI_TimeOut_UserCallBack(1);
	}
	return SPI_I2S_ReceiveData(FLASH_SPIx);
}

/*接收一个字节*/

uint8_t SPI_Flash_ReciveByte(void)
{
	return SPI_Flash_SendByte(Dummy);
}
/*读取FLASH ID*/
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

/*擦除FLASH指定扇区*/

void SPI_FLASH_Erase_Sector(uint32_t SectorAddr)
{
	/*擦除操作需要使能写寄存器*/
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

/*擦除函数*/
void SPI_FLASH_Erase(uint32_t BlockAddr,uint8_t instruction)
{
	/*擦除操作需要使能写寄存器*/
	FLASH_Write_Enable();
	FLASH_SPI_CS_LOW;
	
	SPI_Flash_SendByte(instruction);
	
	SPI_Flash_SendByte((BlockAddr>>16)&0xff);
	SPI_Flash_SendByte((BlockAddr>>8)&0xff);
	SPI_Flash_SendByte(BlockAddr&0xff);
	
	FLASH_SPI_CS_HIGH;
	SPI_WaiteForWriteEnd();
}

/*等待flash内部操作时序完成*/
void SPI_WaiteForWriteEnd(void)
{
	uint8_t statusReg = 0;
	FLASH_SPI_CS_LOW;
	
	SPI_Flash_SendByte(READ_STATUS);
	do
	{
		statusReg = SPI_Flash_ReciveByte();
	}while((statusReg&0x01)==1); //忙碌
	
	FLASH_SPI_CS_HIGH;
}

/*读取FLASH的内容*/
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

/*页写入：向FLASH中写入。注：每次最多写入256 byte*/
void SPI_PageWrite_Data(uint32_t addr,uint8_t* writeBuff,uint16_t numByte)
{
	FLASH_Write_Enable();
	FLASH_SPI_CS_LOW;
	/*如果要写入的字节数大于页字节数，则抛出异常*/
	if(numByte>PAGESIZE)
	{
		printf("numByte morethan PAGESIZE too much！errorCode:%d\n",2);
		numByte = PAGESIZE;
	}
	/*发送写数据指令*/
	SPI_Flash_SendByte(WRITE_DATA);
	
	/*发送写入数据的地址*/
	SPI_Flash_SendByte((addr>>16)&0xff);
	SPI_Flash_SendByte((addr>>8)&0xff);
	SPI_Flash_SendByte(addr&0xff);
	
	/*数据写入*/
	while(numByte--)
	{
		SPI_Flash_SendByte(*writeBuff++);
	}
	
	FLASH_SPI_CS_HIGH;
	
	/*注：这里一定等待Flash内部时序操作完成*/
	SPI_WaiteForWriteEnd();
}

/*多页写入，不再受限256Byte,但需考虑地址对齐*/
void SPI_BuffWrite_Data(uint32_t addr,uint8_t* writeBuff,uint32_t numByte)
{
	uint32_t CountMoreAddr = 0,CountFullPage = 0,CountMorePage = 0;
	
	CountMoreAddr = addr % PAGESIZE;
	CountFullPage = numByte / PAGESIZE;
	CountMorePage = numByte % PAGESIZE;
	/*地址是否对齐，若对齐执行if,否则执行else。*/
	if(CountMoreAddr==0)
	{
		/*数据是否整页,且至少存在一页，若是执行if 否则执行else。*/
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
			/*数据是否超过一页*/
			if(CountFullPage>0)
			{
				while(CountFullPage--)
				{
					SPI_PageWrite_Data(addr,writeBuff,PAGESIZE);
					addr += PAGESIZE;
					writeBuff+=PAGESIZE;
				}
			}
			/*写入不满一页的剩余的数据*/
			SPI_PageWrite_Data(addr,writeBuff,CountMorePage);
		}
	}
	else
	{
		/*程序执行到这里，意味着地址未对齐，处理方法很简单，先写入（PAGESIZE-CountMoreAddr）个字节的数据，补满这一页，
		*接下来再写入的数据的地址就都对齐了。*/
		SPI_PageWrite_Data(addr,writeBuff,PAGESIZE-CountMorePage);
		addr += PAGESIZE-CountMorePage;
		writeBuff += PAGESIZE-CountMorePage;
		CountFullPage = (numByte-PAGESIZE+CountMorePage) / PAGESIZE;
		CountMorePage = (numByte-PAGESIZE+CountMorePage) % PAGESIZE;
		
		if(CountMorePage==0)
		{
			/*整页写入*/
			while(CountFullPage--)
			{
				SPI_PageWrite_Data(addr,writeBuff,PAGESIZE);
				addr += PAGESIZE;
				writeBuff+=PAGESIZE;
			}
		}
		else
		{
			/*数据是否超过一页*/
			if(CountFullPage>0)
			{
				/*先整页写入*/
				while(CountFullPage--)
				{
					SPI_PageWrite_Data(addr,writeBuff,PAGESIZE);
					addr += PAGESIZE;
					writeBuff+=PAGESIZE;
				}
			}
			/*写入不满一页的剩余的数据*/
			SPI_PageWrite_Data(addr,writeBuff,CountMorePage);
		}
	}
}


