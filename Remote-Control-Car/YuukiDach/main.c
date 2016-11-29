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
  * WE NEED TO DO. IT WILL BE AUTO-CONTROLLED AND DO THE REST WORK.
  *
  * <h2><center>&copy; COPYRIGHT 2016 Yuuki_Dach</center></h2>
  ******************************************************************************
  */

#include "precompile.h"             // Enter this file to turn on or turn off the __DEBUG__ switch.
#include "controller.h"
#include "delay.h"
#include "usartconfig.h"
#include "movement.h"
#include "mechanical_arm.h"
#include "SysConfig.h"
#include "WIFIControl.h"
#include "ADCConfig.h"
#include "mpu6050.h"
#include "mpuiic.h"
#include "inv_mpu.h"
#include "inv_mpu_dmp_motion_driver.h"
#include "autocontrol.h"

uint8_t dir_in3 = PART3LEFT;

#if (__DEBUG__ == __ON__)
    uint8_t cnt_part3 = 0;
#endif 

int main(void) {
    delay_init(72);

    Controller_Config();
    Tire_Config();
    Arm_Config();
    Ultrasonic_Init();
    ADC_Config();
    MPU_Init();
    while(mpu_dmp_init());

#if (__DEBUG__ == __ON__)
    USART1_Config();
    NVIC_Config();
    delay_ms(100);
    WIFI_ConnectToServer();
    delay_ms(100);
    printf("\nThe car is connected!\n");
#endif

    putArmHigh();

    while(1){
        if (!isAutoControl()) {
            carGo(getButtonData());
          
#if (__DEBUG__ == __ON__)          
            //uint8_t mode = getButtonData();
            //if (mode == PSB_START)
            //    printf("7 : %u\r\n", Ultrasonic_Trig(GPIO_Pin_7));
            if (dir_in3) {
                ++cnt_part3;
                if (cnt_part3 > 250) cnt_part3 = 2;
                if (cnt_part3 == 1) printf("dir is RIGHT!!!\n.");
            }
            
            if(getButtonData() == PSB_START) {
				ADC_PrintValue();
				printf("::%u\n", Ten_Times_Trig(GPIO_Pin_7));
				while(getButtonData() == PSB_START);
			}
#endif
  
            armControl(getButtonData());  
            dir_in3 = getPart3Direction();
            
            
        } else if (isAutoControl()) {
            float part2yaw = AutoControl();
            finishPart3(dir_in3, part2yaw);                // dir == PART3LEFT(0) or PART3RIGHT(1)
        } 
    }
}

/******************* (C) COPYRIGHT 2016 Yuuki_Dach *************END OF FILE****/

