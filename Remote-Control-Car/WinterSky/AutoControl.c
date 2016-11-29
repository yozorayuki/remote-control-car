#include "AutoControl.h"

#include "ADCConfig.h"
#include "delay.h"
#include "movement.h"
#include "ultrasonic.h"
#include <stdio.h>

#include "mpu6050.h" 
#include "mpuiic.h"
#include "inv_mpu.h"
#include "inv_mpu_dmp_motion_driver.h"


float yaw = 0, dYaw = 0, target;	//欧拉角

u8 bStrat = 0, bSeek = 0, bFind;
u8 nToward = 2, nTurn = 2;


void AutoControlConfig(void) {
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(TRACK_RCC, ENABLE);
	//配置GPIO为浮空输入
	GPIO_InitStructure.GPIO_Pin = TRACK_L1 | TRACK_L2 | TRACK_L3 | TRACK_L4;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(TRACK_GPIO, &GPIO_InitStructure);
	
	RCC_APB2PeriphClockCmd(CHECK_RCC, ENABLE);
	
	GPIO_InitStructure.GPIO_Pin = CHECK_L1 | CHECK_L2 | CHECK_L3 | CHECK_L4;
	GPIO_Init(CHECK_GPIO, &GPIO_InitStructure);
}


u8 TrackCheck(void) {
	u8 re = 0;
	re += GPIO_ReadInputDataBit(TRACK_GPIO, TRACK_L1);
	re += GPIO_ReadInputDataBit(TRACK_GPIO, TRACK_L2)<<1;
	re += GPIO_ReadInputDataBit(TRACK_GPIO, TRACK_L3)<<2;
	re += GPIO_ReadInputDataBit(TRACK_GPIO, TRACK_L4)<<3;
	return re;
}


u8 getLeft(void) {
	return 1 - GPIO_ReadInputDataBit(CHECK_GPIO, CHECK_L1);
}


u8 getRight(void) {
	return 1 - GPIO_ReadInputDataBit(CHECK_GPIO, CHECK_L4);
}


u8 getBottom(void) {
	return GPIO_ReadInputDataBit(CHECK_GPIO, CHECK_L2);
}


void LightCheck(void) {
	int flag = 0;
	for(int i = 0; i < 8; i++) {
		if(AD_Value[i] > ONE) flag++;
	}
	if(!flag) {
		nToward = 2;
		nTurn = 2;
		return;
	}
	
	u16 front = AD_Value[0] + AD_Value[1] + AD_Value[2] + AD_Value[3];
	u16 back = AD_Value[4] + AD_Value[5] + AD_Value[6] + AD_Value[7];
	
	if(AD_Value[0] + AD_Value[3] + AD_Value[4] + AD_Value[7] > 1000) {
		if(front > back + 500) {
			nToward = 0;
		} else if(back > front + 1000) {
			nToward = 1;
		}
	} else {
		if(front > back) {
			nToward = 0;
		} else if(back > front) {
			nToward = 1;
		}
	}
}


u8 bTaskOn = 0;

void TaskStart(u16 nms) {
	if(nms > 1800) nms = 1800;
	
	SysTick->LOAD=(u32)nms*9000;//时间加载(SysTick->LOAD为24bit)
	SysTick->VAL =0x00;           //清空计数器
	SysTick->CTRL=0x01 ;          //开始倒数
	
	bTaskOn = 1;
}


u8 Tasking(void) {
	u32 temp=SysTick->CTRL;
	u8 re = temp&0x01&&!(temp&(1<<16));
	if(!re) {
		SysTick->CTRL=0x00;       //关闭计数器
		SysTick->VAL =0X00;       //清空计数器
		bTaskOn = 0;
	}
	
	return re;
}

void TaskClose(void) {
	if(bTaskOn) {
		SysTick->CTRL=0x00;       //关闭计数器
		SysTick->VAL =0X00;       //清空计数器
		bTaskOn = 0;
	}
}

void Tracking(u8 _nD) {
	if(_nD) {
		switch(TrackCheck()) {
			case 0:
				setSpeed(1, DS2, DS2);
				break;
			
			case 6:
				stopTheCar();
				while(getYaw(&dYaw));
			case 4:
				setSpeed(2, TS2, TS2);
				break;
			
			case 9:
				stopTheCar();
				while(getYaw(&dYaw));
			case 8:
				setSpeed(3, TS2, TS2);
				break;
			
			default:
				setSpeed(1, DS2, DS2);
				break;
		}
	} else {
		switch(TrackCheck()) {
			case 0:
				setSpeed(0, DS2, DS2);
				break;
			
			case 9:
				stopTheCar();
				while(getYaw(&dYaw));
			case 1:
				setSpeed(2, TS2, TS2);
				break;
			
			case 6:
				stopTheCar();
				while(getYaw(&dYaw));
			case 2:
				setSpeed(3, TS2, TS2);
				break;
			
			default:
				setSpeed(0, DS2, DS2);
				break;
		}	
	}
}


void PreDeal(void) {
	/*
	stopTheCar();
	while(getYaw(&dYaw));
	setSpeed(0, DS2, DS2);
	*/
	
	TaskStart(800);
	while(Tasking()) {
		Tracking(0);
		LightCheck();
	}
	TaskClose();
	
	stopTheCar();
	delay_ms(50);
}


void Finding(u8 _nD, u8 _nT) {
#ifdef _DEBUG_MODE
	printf(":light is seeked\n");
#endif
	
	stopTheCar();
	delay_ms(50);
	setSpeed(3-_nT, TS2, TS2);
	
	#define dAngle 7
	
	switch(_nT) {
		case 1:
			target = dYaw + 90 - dAngle;
			while(getYaw(&yaw));
			while(yaw < target) {
				getYaw(&yaw);
			}
			break;
			
		case 0:
			target = dYaw - 90 + dAngle;
			while(getYaw(&yaw));
			while(yaw > target) {
				getYaw(&yaw);
			}
			break;
	}
	
	setSpeed(1, DS1, DS1);
	delay_ms(600);
	
	setSpeed(0, DS1, DS1);
	delay_ms(150);
	while(!getBottom());
	
	setSpeed(2+_nT, TS2, TS2);
	
	switch(_nT) {
		case 1:
			target = dYaw + dAngle;
			while(getYaw(&yaw));
			while(yaw > target) {
				getYaw(&yaw);
			}
			break;
		
		case 0:
			target = dYaw - dAngle;
			while(getYaw(&yaw));
			while(yaw < target) {
				getYaw(&yaw);
			}
			break;
	}
	
	stopTheCar();
	bFind = 0;
	
#ifdef _DEBUG_MODE
	printf("light is finded\n");
#endif
	
	delay_ms(50);
}


void Seeking(void) {
	LightCheck();
	
	if(nToward == 2) {
		Tracking(0);
		u32 iDistance = Ten_Times_Trig(GPIO_Pin_7);
		
#ifdef _DEBUG_MODE
		printf(":NO LIGHT\n");
		ADC_PrintValue();
		printf("::DISTANCE %u\n", iDistance);
#endif
		
		if(AD_Value[8] > COLOR_F) {
            while(iDistance > 65530 && iDistance < 2) {
                iDistance = Ten_Times_Trig(GPIO_Pin_7);
            }
			if(iDistance < 3000) {
				stopTheCar();
#ifdef _DEBUG_MODE
				printf("door is not open\n");
#endif
			}
		}
		
		if(AD_Value[9] > COLOR_B) {
            while(iDistance > 65530 && iDistance < 2) {
                iDistance = Ten_Times_Trig(GPIO_Pin_7);
            }
            if(iDistance < 10000) {
                bSeek = 0;
                stopTheCar();
            }
		}
		
		return;
	}
	
	Tracking(nToward);
	
#ifdef _DEBUG_MODE
	if(nToward)
		printf(":BACK\n");
	else
		printf(":FRONT\n");
    ADC_PrintValue();
#endif
	
	if(getLeft()) {
		bFind = 1;
		nTurn = 0;
	} else if(getRight()) {
		bFind = 1;
		nTurn = 1;
	}
	
	if(nToward == 0 && AD_Value[8] > COLOR_F) {
		if(AD_Value[0] > 1000 && AD_Value[0] > AD_Value[3]) {
			bFind = 1;
			nTurn = 1;
		} else if(AD_Value[3] > 1000 && AD_Value[3] > AD_Value[0]) {
			bFind = 1;
			nTurn = 0;
		}
	} else if(nToward == 1 && AD_Value[9] > COLOR_B) {
		if(AD_Value[4] > 1000 && AD_Value[4] > AD_Value[7]) {
			bFind = 1;
			nTurn = 0;
		} else if(AD_Value[7] > 1000 && AD_Value[7] > AD_Value[4]) {
			bFind = 1;
			nTurn = 1;
		}
	}
	
	if(bFind)
		Finding(nToward, nTurn);
}


float AutoControl(void) {
	if(!bStrat) {
		PreDeal();
		bStrat = 1;
		bSeek = 1;
	}
	while(bSeek) {
		Seeking();
	}
	
#ifdef _DEBUG_MODE
	printf("part two is ended\n");
#endif
	
	return dYaw;
}
