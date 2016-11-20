#ifndef _ULTRASONIC_H_
#define _ULTRASONIC_H_

#include "precompile.h"

#define ULTRASONIC_MAX_RETRY 0x0008FFFF

extern uint32_t u32_ultrasonic_count ;
extern uint8_t u8_tim_sta ;
extern uint32_t u32_tim_uprising ;

void Ultrasonic_Init(void) ;
uint32_t Ultrasonic_Trig(uint32_t _line);
uint32_t Ten_Times_Trig(uint32_t _line);

#endif
