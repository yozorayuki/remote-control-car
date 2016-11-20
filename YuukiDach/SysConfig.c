#include "SysConfig.h"

void RCC_Config(void)
{
  ErrorStatus HSEStartUpStatus;                  
  RCC_DeInit();                                   
  RCC_HSEConfig(RCC_HSE_ON);                  
  HSEStartUpStatus = RCC_WaitForHSEStartUp();      
  if(HSEStartUpStatus == SUCCESS)                
  {
    FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);
    FLASH_SetLatency(FLASH_Latency_2);             
      	
    RCC_HCLKConfig(RCC_SYSCLK_Div1);            
    RCC_PCLK2Config(RCC_HCLK_Div1);              
    RCC_PCLK1Config(RCC_HCLK_Div2);              
         
    RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_9); 
    RCC_PLLCmd(ENABLE);                                
   
    while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)  
    {
    }
    RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);          
    while(RCC_GetSYSCLKSource() != 0x08)              
    {
    }
  }
	
}

/*******************************************************************************
* Function Name   : NVIC_Configuration
* Description        : Configures NVIC and Vector Table base location.
* Input                    : None
* Output                 : None
* Return                 : None
*******************************************************************************/
void NVIC_Config(void)
{
   NVIC_InitTypeDef NVIC_InitStructure;
  
   /* Set the Vector Table base location at 0x08000000 */
   NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0);
  
   /* Configure the NVIC Preemption Priority Bits */  
   NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
  
   /* Enable the USART1 Interrupt */
   NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;     
   NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;	 
   NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;		  
   NVIC_Init(&NVIC_InitStructure); 						  
}
