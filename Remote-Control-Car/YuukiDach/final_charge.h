/**
  ******************************************************************************
  * @author  	   Yuuki_Dach
  * @version       V1.0.0
  * @date          23-November-2016
  * @description   Header file of final_charge.c 
  ******************************************************************************
  * @attention
  *
  * THERE ARE DEFINITIONS OF THE VALUES THAT THE FINAL_CHARGE.C WILL NEED.
  *
  * <h2><center>&copy; COPYRIGHT 2016 Yuuki_Dach</center></h2>
  ******************************************************************************
  */
  
#ifndef _FINAL_CHARGE_H_
#define _FINAL_CHARGE_H_

#include "precompile.h"

#define MIDDLE_TRIGGER GPIO_Pin_7

#define TURN_PWM 90

#define INNER_TIME 100
#define FORWARD_I_TIME 700
#define OUTER_TIME_I 800
#define FORWARD_II_TIME 870

#define CORRECT_DIS 30000
#define INNER_DIS 15000
#define FORWARD_I_DIS 2500
#define FORWARD_II_DIS 4100

#define INTERVAL 85

void correctDir(float part2yaw);
void dirDef(uint8_t dir);
void turnInner(void);
void turnOuter(void);
void upwardSlope(void);
void finishPart3(uint8_t _dir, float part2yaw);

#endif /* __FINAL_CHARGE_H */

/******************* (C) COPYRIGHT 2016 Yuuki_Dach *************END OF FILE****/
