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
#include "usartconf.h"
#include "movement.h"
#include "mechanical_arm.h"
#include "SysConfig.h"
#include "WIFIControl.h"
#include "ADCConfig.h"
#include "mpu6050.h"
//#include "AutoControl.h"
#include "precompile.h"

uint8_t dir = PART3LEFT;
float lastYaw, yaw;

int main(void) {
    delay_init(72);

    Controller_Config();
    Tire_Config();
    Arm_Config();
    ADC_Config();
    MPU_Init();

#if (__DEBUG__ == __ON__)
    USART1_Config();
    NVIC_Config();
    delay_ms(100);
    WIFI_ConnectToServer();
    delay_ms(100);
    printf("\nis connected!\n");
#endif

    Ultrasonic_Init();

    putArmHigh();

    while(1){
        if (!isAutoControl()) {
            carGo (getButtonData ());
          
#if (__DEBUG__ == __ON__)          
            uint8_t mode = getButtonData () ;
            printf("sonic: %5d\n", Ultrasonic_Trig(GPIO_Pin_7));
            delay_ms(100);
            if ( mode == PSB_START )
                printf ( "7 : %u\r\n" , Ultrasonic_Trig ( GPIO_Pin_7 ) ) ;
            else if ( mode == PSB_L2 )
                printf ( "8 : %u\r\n" , Ultrasonic_Trig ( GPIO_Pin_8 ) ) ;
            else if ( mode == PSB_R2 )
                printf ( "9 : %u\r\n" , Ultrasonic_Trig ( GPIO_Pin_9 ) ) ;
#endif
  
            //armControl(getButtonData());  
            dir = getPart3Direction();
            
        } else if (isAutoControl()) {
            Final_Charge (dir); // dir == PART3LEFT(0) or PART3RIGHT(1)
        } 
    }
}

/******************* (C) COPYRIGHT 2016 Yuuki_Dach *************END OF FILE****/

