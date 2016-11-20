/**
  ******************************************************************************
  * @author  			 Yuuki_Dach
  * @version 			 V1.2.0
  * @date          3-November-2016
  * @description   Functions of our car's tires(motors).
  ******************************************************************************
  * @attention
  *
  * OUR CAR USES TIM3 TO OUTPUT PWM WAVE. AND THERE ARE ONLY TWO MOTORS IN OUR
	* CAR. THE CONTROLLERI IS PS2. INTHIS PROGRAM, WE USE IT'S BUTTONS TO CONTROLL
  * ITS DIRECTIONS.
  *
  * <h2><center>&copy; COPYRIGHT 2016 Yuuki_Dach</center></h2>
  ******************************************************************************
  */

#include "movement.h"
#include "controller.h"
#include "usartconf.h"
#include "pca9685.h"
#include "mechanical_arm.h"

void Tire_Config(void){
    GPIO_InitTypeDef GPIO_InitStructure;
    
    RCC_APB2PeriphClockCmd(LEFT1_TIRE_CLK_GPIO  | LEFT2_TIRE_CLK_GPIO | \
                           RIGHT1_TIRE_CLK_GPIO | RIGHT1_TIRE_CLK_GPIO, ENABLE);
    
    GPIO_InitStructure.GPIO_Pin = LEFT1_TIRE_IN1 | LEFT1_TIRE_IN2;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	
    GPIO_Init(LEFT1_TIRE_GPIO, &GPIO_InitStructure);
    
    GPIO_InitStructure.GPIO_Pin = LEFT2_TIRE_IN1 | LEFT2_TIRE_IN2;
    GPIO_Init(LEFT2_TIRE_GPIO, &GPIO_InitStructure);
    
    GPIO_InitStructure.GPIO_Pin = RIGHT1_TIRE_IN1 | RIGHT1_TIRE_IN2;
    GPIO_Init(RIGHT1_TIRE_GPIO, &GPIO_InitStructure);
    
    GPIO_InitStructure.GPIO_Pin = RIGHT2_TIRE_IN1 | RIGHT2_TIRE_IN2;
    GPIO_Init(RIGHT2_TIRE_GPIO, &GPIO_InitStructure);
    
    Arm_Config();
    
    stopTheCar();
}


/**
 * @brief	Sets speed of motors
 * @param	towards: FORWARDS or BACKWARDS
 * @param	PWM_Set: The PWM of motors(from 0 to 100)
 * @retval	None
 */
void setSpeed(int towards, int PWM_Setl, int PWM_Setr){
    PWM_Setl = PWM_Setl < 100 ? PWM_Setl : 100;            
    PWM_Setr = PWM_Setr < 100 ? PWM_Setr : 100;
    
    if (PWM_Setl <= 100 && PWM_Setr <= 100){
        PCA9685_SetOutput(PCA_ADDRESS, LEFT1_TIRE_ENA , 0, 0xfff*PWM_Setl/100);
        PCA9685_SetOutput(PCA_ADDRESS, LEFT2_TIRE_ENA , 0, 0xfff*(PWM_Setl)/100);  
        PCA9685_SetOutput(PCA_ADDRESS, RIGHT1_TIRE_ENA, 0, 0xfff*PWM_Setr/100);
        PCA9685_SetOutput(PCA_ADDRESS, RIGHT2_TIRE_ENA, 0, 0xfff*(PWM_Setr)/100);
    }
    switch (towards) {
        case FORWARDS : setForwards() ; break;
        case BACKWARDS: setBackwards(); break;
        case TURNLEFT : setTurnLeft() ; break;
        case TURNRIGHT: setTurnRight(); break;
    }
}


void setForwards(void) {
    GPIO_Write(GPIOD, GPIO_ReadOutputData(GPIOD) & 0xff55);
    // GPIO_ResetBits(LEFT1_TIRE_GPIO, LEFT1_TIRE_IN2);
    // GPIO_ResetBits(LEFT2_TIRE_GPIO, LEFT2_TIRE_IN2);
    // GPIO_ResetBits(RIGHT1_TIRE_GPIO, RIGHT1_TIRE_IN2);
    // GPIO_ResetBits(RIGHT2_TIRE_GPIO, RIGHT2_TIRE_IN2);
    GPIO_Write(GPIOD, GPIO_ReadOutputData(GPIOD) | 0x0055);
    // GPIO_SetBits(LEFT1_TIRE_GPIO, LEFT1_TIRE_IN1);
    // GPIO_SetBits(LEFT2_TIRE_GPIO, LEFT2_TIRE_IN1);
    // GPIO_SetBits(RIGHT1_TIRE_GPIO, RIGHT1_TIRE_IN1);
    // GPIO_SetBits(RIGHT2_TIRE_GPIO, RIGHT2_TIRE_IN1);  
}


void setBackwards(void) {
    GPIO_Write(GPIOD, GPIO_ReadOutputData(GPIOD) & 0xffaa);
    GPIO_Write(GPIOD, GPIO_ReadOutputData(GPIOD) | 0x00aa);
}


void setTurnLeft(void) {
    GPIO_Write(GPIOD, GPIO_ReadOutputData(GPIOD) & 0xff5a);
    GPIO_Write(GPIOD, GPIO_ReadOutputData(GPIOD) | 0x005a);
}


void setTurnRight(void) {
    GPIO_Write(GPIOD, GPIO_ReadOutputData(GPIOD) & 0xffa5);
    GPIO_Write(GPIOD, GPIO_ReadOutputData(GPIOD) | 0x00a5);
}


void stopTheCar(void) {
    GPIO_Write(GPIOD, GPIO_ReadOutputData(GPIOD) & 0xff00);
    
    PCA9685_SetOutput(PCA_ADDRESS, LEFT1_TIRE_ENA , 0xfff, 0x0);
    PCA9685_SetOutput(PCA_ADDRESS, LEFT2_TIRE_ENA , 0xfff, 0x0);
    PCA9685_SetOutput(PCA_ADDRESS, RIGHT1_TIRE_ENA, 0xfff, 0x0);
    PCA9685_SetOutput(PCA_ADDRESS, RIGHT2_TIRE_ENA, 0xfff, 0x0);
}


void carGo(uint8_t dir){
    switch (dir) {      
        case PSB_PAD_UP   : setSpeed(FORWARDS , 25, 25); break;
        case PSB_PAD_DOWN : setSpeed(BACKWARDS, 25, 25); break;
        case PSB_PAD_RIGHT: setSpeed(TURNRIGHT, 60, 60); break;
        case PSB_PAD_LEFT : setSpeed(TURNLEFT , 60, 60); break;
        case PSB_CIRCLE   : setSpeed(TURNRIGHT, 25, 25); break;
        case PSB_SQUARE   : setSpeed(TURNLEFT , 25, 25); break;
        default: stopTheCar();
    }
}


/* @breif Use left stick to control the car
 *        WARNING!!! Since the sticks of the PS2 are not accurate, I don't recommend users to use this function.
 */
void carGoWithStick(uint8_t lx, uint8_t ly) {
    int slx = ((int)lx-128), sly = ((int)ly-128);
    
    if (sly < -2) {
        if (slx <= 0) {
          
            if (slx <= -100) setSpeed(TURNLEFT , 30, 30);
            else if (slx <= -20) setSpeed(FORWARDS , 30, 40);
            else setSpeed(FORWARDS , 25, 25);
          
        } else {
          
            if (slx >= 100) setSpeed(TURNRIGHT, 30, 30);
            else if (slx >= 20) setSpeed(FORWARDS , 40, 30);
            else setSpeed(FORWARDS , 25, 25);
        }
      
    } else if (sly > 2) {    
        if (slx <= 0) {
          
            if (slx <= -100) setSpeed(TURNRIGHT , 30, 30);
            else if (slx <= -20) setSpeed(BACKWARDS , 30, 40);
            else setSpeed(BACKWARDS , 25, 25);
        
        } else {
        
            if (slx >= 100) setSpeed(TURNLEFT, 30, 30);
            else if (slx >= 20) setSpeed(BACKWARDS , 40, 30);
            else setSpeed(BACKWARDS , 25, 25);
        }
    } else stopTheCar();
}

/******************* (C) COPYRIGHT 2016 Yuuki_Dach *************END OF FILE****/

