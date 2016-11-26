#include "ultrasonic.h"

uint32_t u32_ultrasonic_count ;

void Ultrasonic_Init ( void )
{
    GPIO_InitTypeDef gpio_init_structure ;
    
    RCC_APB2PeriphClockCmd ( RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC | RCC_APB2Periph_AFIO , ENABLE ) ;
    
    /*
    gpio_init_structure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 ;
    gpio_init_structure.GPIO_Mode = GPIO_Mode_IPD ;
    gpio_init_structure.GPIO_Speed = GPIO_Speed_50MHz ;
    GPIO_Init ( GPIOB , &gpio_init_structure ) ;
    GPIO_ResetBits ( GPIOB , GPIO_Pin_0 ) ;
    GPIO_ResetBits ( GPIOB , GPIO_Pin_1 ) ;
    */
    
    gpio_init_structure.GPIO_Pin = GPIO_Pin_7 ;
    gpio_init_structure.GPIO_Mode = GPIO_Mode_IPD ;
    gpio_init_structure.GPIO_Speed = GPIO_Speed_50MHz ;
    GPIO_Init (GPIOA , &gpio_init_structure ) ;
    
    GPIO_ResetBits (GPIOA, GPIO_Pin_7) ;
    
    gpio_init_structure.GPIO_Pin = GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 ;
    gpio_init_structure.GPIO_Mode = GPIO_Mode_Out_PP ;
    gpio_init_structure.GPIO_Speed = GPIO_Speed_50MHz ;
    GPIO_Init ( GPIOC , &gpio_init_structure ) ;
    
    GPIO_ResetBits ( GPIOC , GPIO_Pin_7 ) ;
    GPIO_ResetBits ( GPIOC , GPIO_Pin_8 ) ;
    GPIO_ResetBits ( GPIOC , GPIO_Pin_9 ) ;

    TIM3_Init ( ) ;
    
    return ;
}

uint32_t Ultrasonic_Trig ( uint32_t _line )
{
    uint32_t retry = 0 ;
    u32_ultrasonic_count = 0 ;
    u32p_tim_count = &u32_ultrasonic_count ;
    
    GPIO_SetBits ( GPIOC , _line ) ;
    delay_us ( 20 ) ;
    TIM_SetCounter ( TIM3 , 0 ) ;
    TIM_Cmd ( TIM3 , ENABLE ) ;
    GPIO_ResetBits ( GPIOC , _line ) ;
    
    
    while ( retry < ULTRASONIC_MAX_RETRY && u32_ultrasonic_count == 0 )
        retry ++ ;
    
    return u32_ultrasonic_count ;
    /* 0 : Error 
       1 : Dis > 200.com
       default : Normal 
    */
}

uint32_t Ten_Times_Trig ( uint32_t _line )
{
    uint32_t sum = 0 , now ;
    for ( uint8_t i = 0 ; i < 10 ; i ++ )
    {
        now = Ultrasonic_Trig ( _line ) ;
        if ( now == 1 || now == 0 )
            return 0xFFFF ;
        sum += now ;
    }
    return ( sum / 10 ) ;
}
