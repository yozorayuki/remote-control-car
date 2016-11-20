#ifndef _ADCCONFIG_H_
#define _ADCCONFIG_H_

#include "stm32f10x.h"

extern u16 AD_Value[12];

void ADC_Config(void);
void ADC_PrintValue(void);

#endif
