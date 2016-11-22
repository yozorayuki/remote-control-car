/**
  ******************************************************************************
  * @author  	   Yuuki_Dach
  * @version 	   V1.1.0
  * @date          10-November-2016
  * @description   Main.c
  ******************************************************************************
  * @attention
  *
  * THIS IS THE MAIN.C FILE IN THIS PROJECTS. IT CONTIANS MAIN FUNCTION. OUR CAR
	* NEED TO BE CONTROLLED BY OURSELVES AT FIRST. AFTER WE COMPLETE THE TASK
	* WE NEED TO DO. IT WILL BE AUTO-CONTROLLED AND DO THE REST WORK. FOR MORE
	* SPECIFIC DESCRIPTIONS OF THE TASKS, YOU CAN FIND THEM IN "大赛章程.pdf".
  *
  * <h2><center>&copy; COPYRIGHT 2016 Yuuki_Dach</center></h2>
  ******************************************************************************
  */
#include "controller.h"
#include "delay.h"
#include "usartconfig.h"
#include "movement.h"
#include "mechanical_arm.h"
#include "SysConfig.h"
#include "WIFIControl.h"
#include "ADCConfig.h"
#include "AutoControl.h"
#include "precompile.h"

#include "sys.h"
#include "mpu6050.h" 
#include "mpuiic.h"
#include "inv_mpu.h"
#include "inv_mpu_dmp_motion_driver.h" 


uint8_t dir = PART3LEFT;

int main(void) {
    delay_init(72);
	
    Controller_Config();
    Tire_Config();
    Arm_Config();
	ADC_Config();
    AutoControlConfig();
	
    USART1_Config();
    NVIC_Config();
    delay_ms(100);
    //WIFI_ConnectToServer();
    delay_ms(100);
    printf("\nTCP Connected\n");
	
    Ultrasonic_Init();
	printf("Ultrasonic Init OK\n");
	
	MPU_Init();					//初始化MPU6050
	printf("MPU Init OK\n");
	while(mpu_dmp_init());
	printf("DMP Init OK\n");
	
    //putArmHigh();
	
	int str = 0;
	
    while(1){
        if(!isAutoControl()){
            carGo(getButtonData());
            armControl(getButtonData());
            dir = getPart3Direction();
			
			if(getButtonData() == PSB_L2) {
				ADC_PrintValue();
				printf("::%u\n", Ten_Times_Trig(GPIO_Pin_7));
				while(getButtonData() == PSB_L2);
			}
			
			str = 0;
			
        } else if (isAutoControl()){
			if(!str) {
				str = 1;
				/*
				PCA9685_SetOutput(PCA_ADDRESS, LEFT1_TIRE_ENA , 0, 0xfff*20/100);
				PCA9685_SetOutput(PCA_ADDRESS, LEFT2_TIRE_ENA , 0, 0xfff*50/100);
				PCA9685_SetOutput(PCA_ADDRESS, RIGHT2_TIRE_ENA, 0, 0xfff*0/100);
				PCA9685_SetOutput(PCA_ADDRESS, RIGHT1_TIRE_ENA, 0, 0xfff*0/100);
				
				//setForwards() ;
				//setBackwards();
				//setTurnLeft() ;
				setTurnRight();
				*/
				
			}
			Tracking(0);
			//AutoControl();
            //Final_Charge (PART3RIGHT); // dir == PART3LEFT(0) or PART3RIGHT(1)
        } 
    }
}

/******************* (C) COPYRIGHT 2016 Yuuki_Dach *************END OF FILE****/

