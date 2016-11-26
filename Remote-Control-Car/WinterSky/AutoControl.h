#ifndef _AUTOCONTROL_H_
#define _AUTOCONTROL_H_

#include "stm32f10x.h"


#define TRACK_RCC RCC_APB2Periph_GPIOA
#define TRACK_GPIO GPIOD
#define TRACK_FR GPIO_Pin_11
#define TRACK_BR GPIO_Pin_12
#define TRACK_BL GPIO_Pin_13
#define TRACK_FL GPIO_Pin_14

#define TRACK_L1 GPIO_Pin_14
#define TRACK_L2 GPIO_Pin_13
#define TRACK_L3 GPIO_Pin_12
#define TRACK_L4 GPIO_Pin_11


#define ONE 250
#define DISTANCE 2000

#define DS1 15
#define DS2 20
#define TS1 22
#define TS2 80


extern float yaw, dYaw, target;

void AutoControlConfig(void);
u8 TrackCheck(void);
void Tracking(u8 _nD);
void PreDeal(void);
void Finding(u8 _nD, u8 _nT);
void Seeking(void);

void TaskStart(u16 nms);
u8 Tasking(void);
void TaskClose(void);

float AutoControl(void);


#endif
