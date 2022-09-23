#include "bsp_usart.h"

/*时钟、端口及引脚配置*/
static void USART_GPIO_Configure(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	/*打开串口外设的时钟*/
	USART_APBxCLK_Fun(USART_CLK,ENABLE);
	/*打开GPIO端口的时钟*/
	USART_APBxCLK_GPIO_Fun(USART_GPIO_CLK,ENABLE);
	
	/*配置GPIO端口配置寄存器*/
	GPIO_InitStruct.GPIO_Pin = USART_TXE_PIN;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(USART_TXE_PORT,&GPIO_InitStruct);
	
		/*配置GPIO端口配置寄存器*/
	GPIO_InitStruct.GPIO_Pin = USART_RXNE_PIN;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(USART_RXNE_PORT,&GPIO_InitStruct);
}
/*串口外设配置*/
static void USART_MODE_Configure(void)
{
	USART_InitTypeDef USART_InitStruct;
	
	USART_InitStruct.USART_BaudRate = USART_Periph_BaudRate;
	USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStruct.USART_Mode = USART_Mode_Rx|USART_Mode_Tx;
	USART_InitStruct.USART_Parity = USART_Parity_No;
	USART_InitStruct.USART_StopBits = USART_StopBits_1;
	USART_InitStruct.USART_WordLength = USART_WordLength_8b;
	/*把初始化结构体的值写入串口外设相应的寄存器*/
	USART_Init(USARTx,&USART_InitStruct);
	/*使能串口*/
	USART_Cmd(USARTx, ENABLE);
}

/*串口初始化配置寄存器*/
void USART_InitConfigure(void)
{
	USART_GPIO_Configure();
	USART_MODE_Configure();
}
/*发送一个字节*/
void USART_SendByte(uint8_t data)
{
	USART_SendData(USARTx, data);
	while(USART_GetFlagStatus(USARTx, USART_FLAG_TXE)==RESET);
}

/*发送一个字符串*/
void USART_SendString(uint8_t* str)
{
	while(*str)
	{
		USART_SendByte(*str++);
	}
}
	
/*printf重定向*/
int fputc(int data,FILE* pf)
{
	USART_SendData(USARTx, data);
	while(USART_GetFlagStatus(USARTx, USART_FLAG_TXE)==RESET);
	return data;
}
