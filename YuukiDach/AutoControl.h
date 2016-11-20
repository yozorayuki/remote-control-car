#ifndef _AUTOCONTROL_H_
#define _AUTOCONTROL_H_

#include "stm32f10x.h"

#define TRACK_RCC RCC_APB2Periph_GPIOA
#define TRACK_GPIO GPIOD
#define TRACK_FR GPIO_Pin_11
#define TRACK_BR GPIO_Pin_12
#define TRACK_BL GPIO_Pin_13
#define TRACK_FL GPIO_Pin_14

#define ALL 2500
#define SEEKED 3000
#define FINDED 1500

#define DS 5
#define TS1 13
#define TS2 17

void AutoControlConfig(void);
void ReStart(void);
u8 TrackCheck(void);
void Tracking(u8 _toward);
void preDeal(void);
void clearAdVal (void);
void checkDir (uint8_t pos);
void Finding(s8 _nToward, s8 _nTurn);
void Seeking(void);
void AutoControl(void);


#endif
