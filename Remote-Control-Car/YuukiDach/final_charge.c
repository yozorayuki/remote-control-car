/**
  ******************************************************************************
  * @author        Yuuki_Dach
  * @version       V1.0.0
  * @date          23-November-2016
  * @description   Functions to finish part3
  ******************************************************************************
  * @attention
  *
  * THIS IS THE FINAL PART OF OUR COMPETITION. ALL WE NEED TO DO IS GOING UP A 
  * SLOPE WHICH HAS A INCLINATION OF 30 DEGREE.
  *
  * <h2><center>&copy; COPYRIGHT 2016 Yuuki_Dach</center></h2>
  ******************************************************************************
  */

#include "final_charge.h"

#include "mechanical_arm.h"
#include "mpu6050.h"
#include "mpuiic.h"
#include "inv_mpu.h"
#include "inv_mpu_dmp_motion_driver.h"

uint32_t last_sonic, now_sonic;
float last_yaw, now_yaw;
uint8_t inner_turn, outer_turn;

void dirDef(uint8_t dir) {
    if (dir) {
        inner_turn = TURNLEFT;
        outer_turn = TURNRIGHT;
    } else {
        inner_turn = TURNRIGHT;
        outer_turn = TURNLEFT;
    }
}

void correctDir(float part2yaw) {
    if (part2yaw > 0) {
        setSpeed(TURNRIGHT, TURN_PWM, TURN_PWM);
        while (part2yaw > 0) getYaw(&part2yaw);
    } else if (part2yaw < 0) {
        setSpeed(TURNLEFT , TURN_PWM, TURN_PWM);
        while (part2yaw < 0) getYaw(&part2yaw);
    }
    stopTheCar();
    
    setSpeed(FORWARDS, 20, 20);
    delay_ms(200);
    stopTheCar();
}


void turnInner(void) {
    while(getYaw(&last_yaw));
    setSpeed(inner_turn, TURN_PWM, TURN_PWM);
    now_yaw = 0;
    while (now_yaw < 82) {
        while(getYaw(&now_yaw));
        now_yaw = now_yaw > last_yaw ? now_yaw-last_yaw : last_yaw - now_yaw;

#if (__DEBUG__ == __ON__)
        printf("In turnInner()\n");
#endif
        
    } 
    stopTheCar();
}


void turnOuter(void) {
    while(getYaw(&last_yaw));
    setSpeed(outer_turn, TURN_PWM, TURN_PWM);
    now_yaw = 0;
    while (now_yaw < 80) {
        while(getYaw(&now_yaw));
        now_yaw = now_yaw > last_yaw ? now_yaw-last_yaw : last_yaw - now_yaw;
    } 
    stopTheCar();
}


void upwardSlope(void) {             
    setSpeed(FORWARDS, 80, 80);
    delay_ms(1200);
    stopTheCar();
    now_sonic = Ten_Times_Trig(MIDDLE_TRIGGER);

#if (__DEBUG__ == __ON__)
    printf("sonic: %5d\r\n", now_sonic);
#endif
    
    while (now_sonic < 2050 || now_sonic > 2300) {
        if (now_sonic < 2050) {
            setSpeed(BACKWARDS, 40, 40);
            delay_ms(10);
            now_sonic = Ten_Times_Trig(MIDDLE_TRIGGER);

#if (__DEBUG__ == __ON__)
            printf("sonic: %5d\r\n", now_sonic);
#endif
            
        } else if (now_sonic > 2300) {
            setSpeed(FORWARDS, 40, 40);
            delay_ms(10);
            now_sonic = Ten_Times_Trig(MIDDLE_TRIGGER);

#if (__DEBUG__ == __ON__)
            printf("sonic: %5d\r\n", now_sonic);
#endif
            
        }
    }
}


void finishPart3(uint8_t _dir, float part2yaw) { 
    correctDir(part2yaw);
    dirDef(_dir);
    turnInner();

    setSpeed(FORWARDS, 22, 22);
    delay_ms(700);
    stopTheCar();
    
#if (__DEBUG__ == __ON__)
    printf("Forward 1 was over!\n");
#endif
    
    turnOuter();
    
    do {
        setSpeed(FORWARDS, 22, 22);
    } while (Ten_Times_Trig(MIDDLE_TRIGGER) > 1400);
    
    turnOuter();   
    upwardSlope();    
    stopTheCar();
    putArmLow();
    while(1);
}

/******************* (C) COPYRIGHT 2016 Yuuki_Dach *************END OF FILE****/
