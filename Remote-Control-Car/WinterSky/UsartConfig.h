#ifndef __USARTCONF_H
#define __USARTCONF_H

#include <stm32f10x.h>
#include <stdio.h>

extern u8 RcvBuf[256];
extern u8 RcvStart, RcvEnd;

void USART1_Config(void);
void USART_printf(USART_TypeDef* USARTx, uint8_t *Data,...);
void USART1_put(char ch);
void USART1_puts(char _str[]);
void RcvBufClear(void);

#endif
