#include "bsp_usart.h"

/*ʱ�ӡ��˿ڼ���������*/
static void USART_GPIO_Configure(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	/*�򿪴��������ʱ��*/
	USART_APBxCLK_Fun(USART_CLK,ENABLE);
	/*��GPIO�˿ڵ�ʱ��*/
	USART_APBxCLK_GPIO_Fun(USART_GPIO_CLK,ENABLE);
	
	/*����GPIO�˿����üĴ���*/
	GPIO_InitStruct.GPIO_Pin = USART_TXE_PIN;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(USART_TXE_PORT,&GPIO_InitStruct);
	
		/*����GPIO�˿����üĴ���*/
	GPIO_InitStruct.GPIO_Pin = USART_RXNE_PIN;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(USART_RXNE_PORT,&GPIO_InitStruct);
}
/*������������*/
static void USART_MODE_Configure(void)
{
	USART_InitTypeDef USART_InitStruct;
	
	USART_InitStruct.USART_BaudRate = USART_Periph_BaudRate;
	USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStruct.USART_Mode = USART_Mode_Rx|USART_Mode_Tx;
	USART_InitStruct.USART_Parity = USART_Parity_No;
	USART_InitStruct.USART_StopBits = USART_StopBits_1;
	USART_InitStruct.USART_WordLength = USART_WordLength_8b;
	/*�ѳ�ʼ���ṹ���ֵд�봮��������Ӧ�ļĴ���*/
	USART_Init(USARTx,&USART_InitStruct);
	/*ʹ�ܴ���*/
	USART_Cmd(USARTx, ENABLE);
}

/*���ڳ�ʼ�����üĴ���*/
void USART_InitConfigure(void)
{
	USART_GPIO_Configure();
	USART_MODE_Configure();
}
/*����һ���ֽ�*/
void USART_SendByte(uint8_t data)
{
	USART_SendData(USARTx, data);
	while(USART_GetFlagStatus(USARTx, USART_FLAG_TXE)==RESET);
}

/*����һ���ַ���*/
void USART_SendString(uint8_t* str)
{
	while(*str)
	{
		USART_SendByte(*str++);
	}
}
	
/*printf�ض���*/
int fputc(int data,FILE* pf)
{
	USART_SendData(USARTx, data);
	while(USART_GetFlagStatus(USARTx, USART_FLAG_TXE)==RESET);
	return data;
}
