/**
  ******************************************************************************
  * @author  			 Yuuki_Dach
  * @version 			 V1.0.0
  * @date          3-November-2016
  * @description   Functions of the mechanical arm
  ******************************************************************************
  * @attention
  *
  * SERVOS OF THE MECHANICAL ARM ARE CONTROLLED BY PCA9685. SO REMEMBER TO ADD
  * FILES OF PCA9685(pca9685.h & pca9685.c).
  *
  * <h2><center>&copy; COPYRIGHT 2016 Yuuki_Dach</center></h2>
  ******************************************************************************
  */
	

#ifndef __MECHANICAL_ARM_H
#define __MECHANICAL_ARM_H

#include "pca9685.h"
#include "controller.h"
#include "delay.h"

void Arm_Config(void);
void putArmHigh(void);
void putArmLow(void);
void handGrab(void);
void handOpen(void);
void armControl(uint8_t cmd);

#endif /* __MECHANICAL_ARM_H */

/******************* (C) COPYRIGHT 2016 Yuuki_Dach *************END OF FILE****/

