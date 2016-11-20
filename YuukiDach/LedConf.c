#include "LedConf.h"

void LED_Config(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE);
	
  GPIO_InitTypeDef GPIO_InitStructure;

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5; 
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 
  GPIO_Init(GPIOE, &GPIO_InitStructure);
	
	GPIO_SetBits(GPIOE, GPIO_Pin_5);
}

void LED_Change(void)
{
	u8 bLed = GPIO_ReadOutputDataBit(GPIOE, GPIO_Pin_5);
	if(bLed)
		GPIO_ResetBits(GPIOE, GPIO_Pin_5);
	else
		GPIO_SetBits(GPIOE, GPIO_Pin_5);
}
