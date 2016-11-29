#ifndef _AUTOCONTROL_H_
#define _AUTOCONTROL_H_

#include "stm32f10x.h"


#define TRACK_RCC RCC_APB2Periph_GPIOA
#define TRACK_GPIO GPIOD
#define TRACK_L1 GPIO_Pin_11
#define TRACK_L2 GPIO_Pin_12
#define TRACK_L3 GPIO_Pin_13
#define TRACK_L4 GPIO_Pin_14

#define CHECK_RCC RCC_APB2Periph_GPIOE
#define CHECK_GPIO GPIOE
#define CHECK_L1 GPIO_Pin_11
#define CHECK_L2 GPIO_Pin_12
#define CHECK_L3 GPIO_Pin_13
#define CHECK_L4 GPIO_Pin_14


#define ONE 350
#define DISTANCE 2200
#define COLOR_F 1300
#define COLOR_B 1250

#define DS1 20
#define DS2 22
#define TS1 22
#define TS2 90


extern float yaw, dYaw, target;

void AutoControlConfig(void);
u8 TrackCheck(void);
u8 getLeft(void);
u8 getRight(void);
u8 getBottom(void);
void Tracking(u8 _nD);
void PreDeal(void);
void Finding(u8 _nD, u8 _nT);
void Seeking(void);

void TaskStart(u16 nms);
u8 Tasking(void);
void TaskClose(void);

float AutoControl(void);


#endif
