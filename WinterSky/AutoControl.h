#ifndef _AUTOCONTROL_H_
#define _AUTOCONTROL_H_

#include "stm32f10x.h"


#define TRACK_RCC RCC_APB2Periph_GPIOA
#define TRACK_GPIO GPIOD
#define TRACK_FR GPIO_Pin_11
#define TRACK_BR GPIO_Pin_12
#define TRACK_BL GPIO_Pin_13
#define TRACK_FL GPIO_Pin_14


#define ALL 2000
#define SEEKED 3000
#define FINDED 1500


#define DS1 9
#define DS2 13
#define TS1 25
#define TS2 32

void AutoControlConfig(void);
u8 TrackCheck(void);
void Tracking(u8 _nD);
void PreDeal(void);
void Finding(u8 _nD, u8 _nT);
void Seeking(void);
void Turn(u8 _n, u8 _s);
void TaskStart(u16 nms);
u8 Tasking(void);
void TaskClose(void);


void AutoControl(void);


#endif
