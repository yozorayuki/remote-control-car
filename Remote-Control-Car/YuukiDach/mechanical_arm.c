/**
  ******************************************************************************
  * @author  			 Yuuki_Dach
  * @version 			 V1.0.1
  * @date          3-November-2016
  * @description   Functions of the mechanical arm
  ******************************************************************************
  * @attention
  *
  * SERVOS OF THE MECHANICAL ARM ARE CONTROLLED BY PCA9685.
  *
  * <h2><center>&copy; COPYRIGHT 2016 Yuuki_Dach</center></h2>
  ******************************************************************************
  */
	
#include "mechanical_arm.h"

void Arm_Config(void) {
    PCA9685_I2C_Init();
    
    PCA9685_Init_TypeDef PCA_InitStructure;
    
    PCA_InitStructure.Address = PCA_ADDRESS;
    PCA_InitStructure.InvOutputs = PCA9685_NotInvOutputs;
    PCA_InitStructure.OutputDriver = PCA9685_OutputDriver_TotemPole;
    PCA_InitStructure.OutputNotEn = PCA9685_OutputNotEn_0;
    PCA_InitStructure.PWMFrequency = PCA9685_Frequency_50Hz;
    
    PCA9685_Init(&PCA_InitStructure);
}


void putArmHigh(void) {
    PCA9685_SetOutput(PCA_ADDRESS, 0, 0, 130); 
    delay_ms(1000);
    PCA9685_SetOutput(PCA_ADDRESS, 1, 0, 470); 
    PCA9685_SetOutput(PCA_ADDRESS, 2, 0, 505); 
}


void putArmLow(void) {
    PCA9685_SetOutput(PCA_ADDRESS, 1, 0, 300);
    delay_ms(200);
    PCA9685_SetOutput(PCA_ADDRESS, 1, 0, 250);
    delay_ms(200);
    for (int i = 200; i >= 105; i -= 6) {
        PCA9685_SetOutput(PCA_ADDRESS, 1, 0, i);      // In order to put the arm down slowlier
        delay_ms(35);
    }
    
    handGrab();
    PCA9685_SetOutput(PCA_ADDRESS, 0, 0, 460);
    delay_ms(600);
    handOpen();
    delay_ms(300);
    PCA9685_SetOutput(PCA_ADDRESS, 1, 0, 250); //add the pwm will put the arm lower
    PCA9685_SetOutput(PCA_ADDRESS, 2, 0, 495);
}


void handGrab(void) {
    PCA9685_SetOutput(PCA_ADDRESS, 3, 0, 125); 
}


void handOpen(void) {
    PCA9685_SetOutput(PCA_ADDRESS, 3, 0, 270);
}

void armControl(uint8_t cmd) {
    switch (cmd) {
        case PSB_CROSS    : putArmHigh(); break;
        case PSB_TRIANGLE : putArmLow() ; break;
        case PSB_R1       : handGrab()  ; break;
        case PSB_R2       : handOpen()  ; break;
    }
}
/******************* (C) COPYRIGHT 2016 Yuuki_Dach *************END OF FILE****/
