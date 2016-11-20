#include "TIM.H"

uint8_t u8_tim_sta ;
uint32_t u32_tim_uprising ;
uint32_t * u32p_tim_count ;

void TIM3_Init ( void )
{
    u8_tim_sta = FALSE ;
    u32_tim_uprising = 0 ;
    u32p_tim_count = NULL ;
    
    RCC_APB1PeriphClockCmd ( RCC_APB1Periph_TIM3 , ENABLE ) ;
    
    TIM_TimeBaseInitTypeDef tim_baseinit_structure ;
    
    tim_baseinit_structure.TIM_Period = 0xFFFF ;
    tim_baseinit_structure.TIM_Prescaler = 0 ;
    tim_baseinit_structure.TIM_ClockDivision = 0 ;
    tim_baseinit_structure.TIM_CounterMode = TIM_CounterMode_Up ;
    TIM_TimeBaseInit ( TIM3 , &tim_baseinit_structure ) ;
    TIM_PrescalerConfig ( TIM3 , 18 - 1 ,TIM_PSCReloadMode_Immediate ) ;
    
    TIM_ICInitTypeDef tim_icinit_structure ;
    
    tim_icinit_structure.TIM_Channel = TIM_Channel_2 ;
    tim_icinit_structure.TIM_ICPolarity = TIM_ICPolarity_Rising ;
    tim_icinit_structure.TIM_ICSelection = TIM_ICSelection_DirectTI ;
    tim_icinit_structure.TIM_ICPrescaler = TIM_ICPSC_DIV1 ;
    tim_icinit_structure.TIM_ICFilter = 0x0 ;
    TIM_ICInit ( TIM3 , &tim_icinit_structure ) ;
    
    tim_icinit_structure.TIM_Channel = TIM_Channel_3 ;
    tim_icinit_structure.TIM_ICPolarity = TIM_ICPolarity_Rising ;
    tim_icinit_structure.TIM_ICSelection = TIM_ICSelection_DirectTI ;
    tim_icinit_structure.TIM_ICPrescaler = TIM_ICPSC_DIV1 ;
    tim_icinit_structure.TIM_ICFilter = 0x0 ;
    TIM_ICInit ( TIM3 , &tim_icinit_structure ) ;
    
    tim_icinit_structure.TIM_Channel = TIM_Channel_4 ;
    tim_icinit_structure.TIM_ICPolarity = TIM_ICPolarity_Rising ;
    tim_icinit_structure.TIM_ICSelection = TIM_ICSelection_DirectTI ;
    tim_icinit_structure.TIM_ICPrescaler = TIM_ICPSC_DIV1 ;
    tim_icinit_structure.TIM_ICFilter = 0x0 ;
    TIM_ICInit ( TIM3 , &tim_icinit_structure ) ;
    
    NVIC_InitTypeDef nvic_init_structure ;
    
    NVIC_PriorityGroupConfig ( NVIC_PriorityGroup_0 ) ;
    
    nvic_init_structure.NVIC_IRQChannel = TIM3_IRQn; 
    nvic_init_structure.NVIC_IRQChannelPreemptionPriority = 0 ;
    nvic_init_structure.NVIC_IRQChannelSubPriority = 1 ;
    nvic_init_structure.NVIC_IRQChannelCmd = ENABLE; 
    NVIC_Init ( &nvic_init_structure ) ;
    
    TIM_ClearITPendingBit ( TIM3 , TIM_IT_Update ) ;
    TIM_ARRPreloadConfig ( TIM3 , DISABLE ) ; 
    TIM_ITConfig ( TIM3 , TIM_IT_Update | TIM_IT_CC2 | TIM_IT_CC3 | TIM_IT_CC4 , ENABLE ) ;
    TIM_Cmd ( TIM3 , DISABLE ) ;

    return ;
}

void TIM3_IRQHandler ( void ) 
{
    //USART1_Send_Str ( ( uint8_t * ) "TIM3.\r\n" ) ;
    
    if ( TIM_GetITStatus ( TIM3 , TIM_IT_CC2 ) == SET )
    {
        TIM_ClearITPendingBit ( TIM3 , TIM_IT_CC2 ) ;
        if ( !u8_tim_sta )
        {
            u32_tim_uprising = TIM_GetCapture2 ( TIM3 ) ;
            u8_tim_sta = TRUE ;
            TIM_OC2PolarityConfig ( TIM3 , TIM_ICPolarity_Falling ) ;
        }
        else
        {
            if ( u32p_tim_count )
                * u32p_tim_count = TIM_GetCapture2 ( TIM3 ) - u32_tim_uprising ;
            u8_tim_sta = FALSE ;
            TIM_OC2PolarityConfig ( TIM3 , TIM_ICPolarity_Rising ) ;
            TIM_Cmd ( TIM3 , DISABLE ) ;
        }
    }
    
    if ( TIM_GetITStatus ( TIM3 , TIM_IT_CC3 ) == SET )
    {
        TIM_ClearITPendingBit ( TIM3 , TIM_IT_CC3 ) ;
        if ( !u8_tim_sta )
        {
            u32_tim_uprising = TIM_GetCapture3 ( TIM3 ) ;
            u8_tim_sta = TRUE ;
            TIM_OC3PolarityConfig ( TIM3 , TIM_ICPolarity_Falling ) ;
        }
        else
        {
            if ( u32p_tim_count )
                * u32p_tim_count = TIM_GetCapture3 ( TIM3 ) - u32_tim_uprising ;
            u8_tim_sta = FALSE ;
            TIM_OC3PolarityConfig ( TIM3 , TIM_ICPolarity_Rising ) ;
            TIM_Cmd ( TIM3 , DISABLE ) ;
        }
    }
    
    if ( TIM_GetITStatus ( TIM3 , TIM_IT_CC4 ) == SET )
    {
        TIM_ClearITPendingBit ( TIM3 , TIM_IT_CC4 ) ;
        if ( !u8_tim_sta )
        {
            u32_tim_uprising = TIM_GetCapture4 ( TIM3 ) ;
            u8_tim_sta = TRUE ;
            TIM_OC4PolarityConfig ( TIM3 , TIM_ICPolarity_Falling ) ;
        }
        else
        {
            if ( u32p_tim_count )
                * u32p_tim_count = TIM_GetCapture4 ( TIM3 ) - u32_tim_uprising ;
            u8_tim_sta = FALSE ;
            TIM_OC4PolarityConfig ( TIM3 , TIM_ICPolarity_Rising ) ;
            TIM_Cmd ( TIM3 , DISABLE ) ;
        }
    }
    
    if ( TIM_GetITStatus ( TIM3 , TIM_IT_Update ) == SET )
    {
        TIM_ClearITPendingBit ( TIM3 , TIM_IT_Update ) ;
        if ( u32p_tim_count )
        {
            if ( !u8_tim_sta )
                * u32p_tim_count = 0 ;
            else
                * u32p_tim_count = 1 ;
        }
        u8_tim_sta = FALSE ;
        TIM_OC4PolarityConfig ( TIM2 , TIM_ICPolarity_Rising ) ;
        TIM_OC4PolarityConfig ( TIM3 , TIM_ICPolarity_Rising ) ;
        TIM_OC4PolarityConfig ( TIM4 , TIM_ICPolarity_Rising ) ;
        TIM_Cmd ( TIM3 , DISABLE ) ;
    }
    
    return ;
}
