 /******************** (C) COPYRIGHT 2012 WildFire Team ***************************      
**********************************************************************************/

#include "usart1.h"
#include <stdarg.h>

void USART1_Config(void) {
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
	
    /* config USART1 clock */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA, ENABLE);
	
    /* USART1 GPIO config */
    /* Configure USART1 Tx (PA.09) as alternate function push-pull */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);    
    /* Configure USART1 Rx (PA.10) as input floating */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
	  
    /* USART1 mode config */
    USART_InitStructure.USART_BaudRate = 115200;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No ;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(USART1, &USART_InitStructure); 
    USART_Cmd(USART1, ENABLE);
}


int fputc(int ch, FILE *f)
{
    USART_SendData(USART1, (unsigned char) ch);
    // While (!(USART1->SR & USART_FLAG_TXE));
    while( USART_GetFlagStatus(USART1,USART_FLAG_TC)!= SET);	
    return (ch);
}


static char *itoa(int value, char *string, int radix)
{
    int i, d;
    int flag = 0;
    char *ptr = string;
    
    /* This implementation only works for decimal numbers. */
    if (radix != 10) {
        *ptr = 0;
        return string;
    }
    
    if (!value) {
        *ptr++ = 0x30;
        *ptr = 0;
        return string;
    }
    
    /* if this is a negative value insert the minus sign. */
    if (value < 0) {
        *ptr++ = '-';
    
        /* Make the value positive. */
        value *= -1;
    }
    
    for (i = 10000; i > 0; i /= 10) {
        d = value / i;
    
        if (d || flag) {
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
 * Demo: USART1_printf( USART1, "\r\n this is a demo \r\n" );
 *       USART1_printf( USART1, "\r\n %d \r\n", i );
 *       USART1_printf( USART1, "\r\n %s \r\n", j );
 */
void USART1_printf(USART_TypeDef* USARTx, uint8_t *Data,...)
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
    {									  //
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
/******************* (C) COPYRIGHT 2012 WildFire Team *****END OF FILE************/

