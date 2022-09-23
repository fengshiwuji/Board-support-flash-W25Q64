#ifndef __BSP_LED_H__
#define __BSP_LED_H__

#include "stm32f10x.h"

#define LED1_GPIO_PIN 	 	GPIO_Pin_5
#define LED1_GPIO_PORT  	GPIOB
#define LED_GPIO_CLK		RCC_APB2Periph_GPIOB
#define LED2_GPIO_PIN 	 	GPIO_Pin_6
#define LED2_GPIO_PORT  	GPIOB

#define ON 		1
#define OFF		0
#define LED1(t)	if(t)\
					GPIO_ResetBits(LED1_GPIO_PORT, LED1_GPIO_PIN);\
				else \
					GPIO_SetBits(LED1_GPIO_PORT, LED1_GPIO_PIN);
#define LED2(t)	if(t)\
					GPIO_ResetBits(LED2_GPIO_PORT, LED2_GPIO_PIN);\
				else \
					GPIO_SetBits(LED2_GPIO_PORT, LED2_GPIO_PIN);

#define LED1_TOGGLE	{LED1_GPIO_PORT->ODR ^= GPIO_Pin_5;}
#define	LED2_TOGGLE	{LED2_GPIO_PORT->ODR ^= GPIO_Pin_6;}
				
				
void LED_GPIO_Config(void);


#endif /*__BSP_LED_H__*/
