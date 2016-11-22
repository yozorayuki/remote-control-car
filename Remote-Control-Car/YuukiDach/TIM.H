#ifndef _TIM_H_
#define _TIM_H_

#include "precompile.h"

extern uint32_t * u32p_tim_count ;

void TIM3_Init ( void ) ;
void TIM3_IRQHandler ( void ) ; 

#endif
