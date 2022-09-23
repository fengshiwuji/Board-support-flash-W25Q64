#ifndef __BSP_USART_H__
#define __BSP_USART_H__

#include "stm32f10x.h"
#include <stdio.h>

#define USARTx					USART2
#define USART_APBxCLK_Fun		RCC_APB1PeriphClockCmd
#define USART_CLK				RCC_APB1Periph_USART2
#define USART_APBxCLK_GPIO_Fun	RCC_APB2PeriphClockCmd
#define USART_GPIO_CLK			RCC_APB2Periph_GPIOA

#define USART_Periph_BaudRate	115200
#define USART_TXE_PORT			GPIOA
#define USART_TXE_PIN			GPIO_Pin_2
#define USART_RXNE_PORT			GPIOA
#define USART_RXNE_PIN			GPIO_Pin_3


void USART_InitConfigure(void);
/*发送一个字节*/
void USART_SendByte(uint8_t data);
/*发送一个字符串*/
void USART_SendString(uint8_t* str);

#endif /* __BSP_USART_H__ */
