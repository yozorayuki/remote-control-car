#include "UsartConfig.h"
#include <stdarg.h>

u8 RcvBuf[256] = {'0'};
u8 RcvStart = 0, RcvEnd = 0, RcvNext = 1;

void USART1_Config(void) {	
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	USART_InitStructure.USART_BaudRate = 115200;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART1, &USART_InitStructure);

	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);

	USART_Cmd(USART1, ENABLE);

	USART_ClearFlag(USART1, USART_FLAG_TC);  
	
	for(u8 i = 0; i != 0; i++) RcvBuf[i] = i;
}

/* NVIC
void NVIC_Config(void)
{
   NVIC_InitTypeDef NVIC_InitStructure;
  
   Set the Vector Table base location at 0x08000000
   NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0);
  
   Configure the NVIC Preemption Priority Bits
   NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
  
   Enable the USART1 Interrupt
   NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;     
   NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;	
   NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;		 
   NVIC_Init(&NVIC_InitStructure); 				
}
*/


int fputc(int ch, FILE *f)
{
    USART_SendData(USART1, (unsigned char) ch);
    // While (!(USART1->SR & USART_FLAG_TXE));
    while( USART_GetFlagStatus(USART1,USART_FLAG_TC)!= SET);	
    return (ch);
}


static char *itoa(int value, char *string, int radix)
{
	int     i, d;
	int     flag = 0;
	char    *ptr = string;
	
	/* This implementation only works for decimal numbers. */
	if (radix != 10)
	{
	    *ptr = 0;
	    return string;
	}
	
	if (!value)
	{
	    *ptr++ = 0x30;
	    *ptr = 0;
	    return string;
	}
	
	/* if this is a negative value insert the minus sign. */
	if (value < 0)
	{
	    *ptr++ = '-';
	
	    /* Make the value positive. */
	    value *= -1;
	}
	
	for (i = 10000; i > 0; i /= 10)
	{
	    d = value / i;
	
	    if (d || flag)
	    {
	        *ptr++ = (char)(d + 0x30);
	        value -= (d * i);
	        flag = 1;
	    }
	}
	
	/* Null terminate the string. */
	*ptr = 0;
	
	return string;

} /* NCL_Itoa */


/*
 * demo: USART_printf( USART1, "\r\n this is a demo \r\n" );
 *       USART_printf( USART1, "\r\n %d \r\n", i );
 *       USART_printf( USART1, "\r\n %s \r\n", j );
 */
void USART_printf(USART_TypeDef* USARTx, uint8_t *Data,...)
{
	const char *s;
	int d;   
	char buf[16];
	
	va_list ap;
	va_start(ap, Data);
	
	while ( *Data != 0)    
	{				                          
		if ( *Data == 0x5c )  //'\'
	{									  
	switch ( *++Data )
	{
		case 'r':							    
			USART_SendData(USARTx, 0x0d);
			Data ++;
		break;
		
		case 'n':							      
			USART_SendData(USARTx, 0x0a);	
			Data ++;
		break;
		
		default:
			Data ++;
		break;
	}			 
	}
	else if ( *Data == '%')
	{							
	switch ( *++Data )
	{
		case 's':								
			s = va_arg(ap, const char *);
	for ( ; *s; s++) 
	{
		USART_SendData(USARTx,*s);
		while( USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET );
	}
		Data++;
		break;
	
	case 'd':						
	d = va_arg(ap, int);
	itoa(d, buf, 10);
	for (s = buf; *s; s++) 
	{
		USART_SendData(USARTx,*s);
		while( USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET );
	}
	Data++;
	break;
		 default:
				Data++;
		    break;
	}		 
	} /* end of else if */
	else USART_SendData(USARTx, *Data++);
	while( USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET );
	}
}


void USART1_put(char _ch) {
  USART_SendData(USART1, (u8) _ch);
  while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
}


void USART1_puts(char _str[]) {
	for(int i = 0; _str[i] != 0; i++) {
		USART_SendData(USART1, _str[i]);
		while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
	}
}


void USART1_IRQHandler(void) {
  if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  
  {
		if(RcvNext == RcvStart) {
			if(RcvStart == RcvBuf[RcvStart]) {
				RcvStart = ++RcvEnd;
				RcvBuf[RcvEnd] = RcvEnd;
			} else {
				for(u8 i = RcvStart; i < RcvBuf[RcvStart]; i++) RcvBuf[i] = i;
				RcvStart = RcvBuf[RcvStart];
			}
		}

		RcvBuf[RcvNext] = USART_ReceiveData(USART1);
		RcvNext++;
		if(RcvBuf[RcvNext-2] == '\r' && RcvBuf[RcvNext-1] == '\n') {
			if(RcvNext == RcvEnd+3) RcvNext -= 2;
			else {
				RcvBuf[RcvEnd] = RcvNext-2;
				RcvEnd = RcvNext-2;
				RcvBuf[RcvEnd] = RcvEnd;
				RcvNext--;
				RcvBuf[RcvNext] = RcvNext;
			}
		}
	}
}


void RcvBufClear(void) {
	RcvStart = 0;
	RcvEnd = 0;
	RcvNext = 1;
	for(u8 i = 0; i != 0; i++) RcvBuf[i] = i;
}
