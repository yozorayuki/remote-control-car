#include "AutoControl.h"

#include "ADCConfig.h"
#include "delay.h"
#include "movement.h"
#include <stdio.h>

#include "sys.h"
#include "mpu6050.h" 
#include "mpuiic.h"
#include "inv_mpu.h"
#include "inv_mpu_dmp_motion_driver.h" 

#define _DEBUG_MODE


float pitch = 0,roll = 0,yaw = 0; 		//欧拉角
float dYaw = 0;


u8 bStrat = 0, bEnd = 0, bSeek = 0, bFind;
u8 nToward = 2, nTurn = 2;
u16 adValue[10];


void AutoControlConfig(void) {
	RCC_APB2PeriphClockCmd(TRACK_RCC, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	
	//配置GPIO为浮空输入
	GPIO_InitStructure.GPIO_Pin = TRACK_FL | TRACK_FR | TRACK_BL | TRACK_BR;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(TRACK_GPIO, &GPIO_InitStructure);
}


u8 TrackCheck(void) {
	u8 re = 0;
	re += GPIO_ReadInputDataBit(TRACK_GPIO, TRACK_FL);
	re += GPIO_ReadInputDataBit(TRACK_GPIO, TRACK_FR)<<1;
	re += GPIO_ReadInputDataBit(TRACK_GPIO, TRACK_BR)<<2;
	re += GPIO_ReadInputDataBit(TRACK_GPIO, TRACK_BL)<<3;
	return re;
}


void LightCheck(void) {
	u16 front = AD_Value[0] + AD_Value[1] + AD_Value[3] + AD_Value[4];
	u16 back = AD_Value[5] + AD_Value[6] + AD_Value[7] + AD_Value[8];
	
	u16 all = front + back;
	
	if(all < ALL && AD_Value[10] > 3000 && AD_Value[11] > 3000) {
		nToward = 2;
		nTurn = 2;
		return;
	}
	
	if(front > back + 300) {
		nToward = 0;
	} else if(back > front + 300) {
		nToward = 1;
	}
	/*
	u16 left = AD_Value[4] + AD_Value[3] + AD_Value[5] + AD_Value[6];
	u16 right = AD_Value[0] + AD_Value[1] + AD_Value[7] + AD_Value[8];
	
	if(left > right + 200) {
		nTurn = 0;
	} else if(right > left + 200){
		nTurn = 1;
	}
	*/
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
			case 0:								// backward
				setSpeed(1, DS2, DS2);
				break;
			
			case 1:								// turn left
				setSpeed(2, TS2, TS1);
				break;
			
			case 4:
				setSpeed(2, TS2, TS1);
				break;
			
			case 2:
				setSpeed(3, TS1, TS2);
				break;
			
			case 8:
				setSpeed(3, TS1, TS2);
				break;
			
			case 5:
				setSpeed(2, TS2, TS2);
				break;
			
			case 10:
				setSpeed(3, TS2, TS2);
				break;
			
			case 9:
				//stopTheCar();
				//while(getYaw(&yaw));
				//dYaw = yaw;
				setSpeed(3, TS2, TS1);
				while((TrackCheck()&4) == 0);
				stopTheCar();
				break;
			
			case 6:
				//stopTheCar();
				//while(getYaw(&yaw));
				//dYaw = yaw;
				setSpeed(2, TS1, TS2);
				while((TrackCheck()&8) == 0);
				stopTheCar();
				break;
			
			case 3:					// 90 degree
			case 12:				// 90 degree	
			case 7:					// 1 2 
									//   4
			
			case 11:				// 1 2
									// 8
			
			case 13:				// 1 
									// 8 4
			
			case 14:				//   2
									// 8 4
			
			case 15:				// all
				stopTheCar();
				break;
			
			default:
				stopTheCar();
				break;
		}
	} else {
		switch(TrackCheck()) {
			case 0:
				setSpeed(0, DS2, DS2);
				break;
			
			case 1:
				setSpeed(2, TS1, TS2);
				break;
			
			case 4:
				setSpeed(2, TS1, TS2);
				break;
			
			case 2:
				setSpeed( 3, TS2, TS1);
				break;
			
			case 8:
				setSpeed(3, TS2, TS1);
				break;
			
			case 5:
				setSpeed(2, TS2, TS2);
				break;
			
			case 10:
				setSpeed(3, TS2, TS2);
				break;
			
			case 9:
				//stopTheCar();
				//while(getYaw(&yaw));
				//dYaw = yaw;
				setSpeed(2, TS1, TS2);
				while((TrackCheck()&2) == 0);
				stopTheCar();
				break;
			
			case 6:
				//stopTheCar();
				//while(getYaw(&yaw));
				//dYaw = yaw;
				setSpeed(3, TS2, TS1);
				while((TrackCheck()&1) == 0);
				stopTheCar();
				break;
			
			case 3:
			case 12:
			case 7:
			case 11:
			case 13:
			case 14:
			case 15:
				if(bSeek) {
					stopTheCar();
					printf(":Puse\n");
				} else {
					setSpeed(0, DS2, DS2);
				}
				break;
			
			default:
				stopTheCar();
				break;
		}
	}
}


void PreDeal(void) {
	setSpeed(0, DS2, DS2);
	
	TaskStart(1800);
	while(Tasking()) {
		Tracking(0);
		LightCheck();
	}
	TaskClose();
	
	stopTheCar();
}


void Finding(u8 _nD, u8 _nT) {
	printf(":light is seeked\n");
	
	stopTheCar();
	delay_ms(50);
	setSpeed(3-_nT, 35, 35);
	
	float target = 0;
	
	switch(_nT) {
		case 1:
			target = dYaw + 90 - 14;
			while(1) {
				getYaw(&yaw);
				if(yaw > target)
					break;
			}
			break;
			
		case 0:
			target = dYaw - 90 + 14;
			while(1) {
				getYaw(&yaw);
				if(yaw < target)
					break;
			}
			break;
	}
	
	while(0) {
		
	}
	setSpeed(1, 12, 12);
	delay_ms(1100);
	
	setSpeed(0, 15, 15);
	while(!TrackCheck());
	/*
	while(1) {
		if(TrackCheck()) {
			delay_ms(1);
			if(TrackCheck())
				break;
		}
	}
	*/
	stopTheCar();
	delay_ms(50);
	setSpeed(2+_nT, 35, 35);
	
	switch(_nT) {
		case 1:
			while(1) {
				getYaw(&yaw);
				if(yaw < dYaw + 5) break;
			}
			break;
		
		case 0:
			while(1) {
				getYaw(&yaw);
				if(yaw > dYaw - 5) break;
			}
			break;
	}
	
	stopTheCar();
	printf("light is finded\n");
	bFind = 0;
}


void Seeking(void) {
	LightCheck();
	
	if(nToward == 2) {
		printf(":NO LIGHT %4d, %4d, %4d, %4d, %4d - %4d, %4d, %4d, %4d\n", AD_Value[0], AD_Value[1], AD_Value[2], AD_Value[3], AD_Value[4], AD_Value[5], AD_Value[6], AD_Value[7], AD_Value[8]);
		printf("          left    %4d, right   %4d\n", AD_Value[10], AD_Value[11]);
		Tracking(0);
		if(AD_Value[9] > 2800) {
			bSeek = 0;
		}
		return;
	}
	
	if(nToward)
		printf(":BACK  ");
	else
		printf(":FRONT ");
	
	Tracking(nToward);
	
	printf("%4d, %4d, %4d, %4d, %4d - %4d, %4d, %4d, %4d\n", AD_Value[0], AD_Value[1], AD_Value[2], AD_Value[3], AD_Value[4], AD_Value[5], AD_Value[6], AD_Value[7], AD_Value[8]);
	printf("       left    %4d, right   %4d\n", AD_Value[10], AD_Value[11]);
	
	if(AD_Value[10] < 1900) {
		bFind = 1;
		nTurn = 0;
	} else if(AD_Value[11] < 1900) {
		bFind = 1;
		nTurn = 1;
	}
	
	if(bFind)
		Finding(nToward, nTurn);
}


void AutoControl(void) {
	if(!bStrat) {
		PreDeal();
		bStrat = 1;
		bSeek = 1;
	}
	if(bSeek) {
		Seeking();
	} else if(!bEnd) {
		printf("do something and end\n");
		bEnd = 1;
	}
}


void Turn(u8 _n, u8 _s) {
	if(_s > 100) _s = 100;
	if(_n <2 || _n > 3) return;
	
	setSpeed(_n, _s, _s);
	
	float target = 0;
	
	switch(_n) {
		case 2:
			target = dYaw + 90;
			while(1) {
				getYaw(&yaw);
				if(yaw > target)
					break;
			}
			break;
			
		case 3:
			target = dYaw - 90;
			while(1) {
				getYaw(&yaw);
				if(yaw < target)
					break;
			}
			break;
	}
	
	stopTheCar();
}


