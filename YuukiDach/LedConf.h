#ifndef __LEDCONF_H
#define __LEDCONF_H

#include <stm32f10x.h>

void LED_Config(void);
void LED_Change(void);

#define LED_ON	GPIO_ResetBits(GPIOE, GPIO_Pin_5)
#define LED_OFF	GPIO_SetBits(GPIOE, GPIO_Pin_5)

#endif
