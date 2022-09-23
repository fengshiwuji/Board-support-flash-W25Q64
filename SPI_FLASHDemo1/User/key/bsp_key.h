#ifndef __BSP_KEY_H__
#define __BSP_KEY_H__

#include "stm32f10x.h"

#define KEY_ON				1
#define KEY_OFF				0

#define KEY1_GPIO_PIN 	 	GPIO_Pin_1
#define KEY1_GPIO_PORT  	GPIOB
#define KEY2_GPIO_PIN 	 	GPIO_Pin_13
#define KEY2_GPIO_PORT  	GPIOB
#define KEY_GPIO_CLK		RCC_APB2Periph_GPIOB

void KEY_GPIO_Config(void);
uint8_t Key_Scan(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);

#endif

