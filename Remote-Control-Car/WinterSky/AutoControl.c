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

u8 bStrat = 0, bEnd = 0, bSeek = 0, bFind;
u8 nToward = 2, nTurn = 2;


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
	
	if(front > back + 500) {
		nToward = 0;
	} else if(back > front + 1000) {
		nToward = 1;
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
				//setSpeed(1, 30, 30);
				setSpeed(2, TS2, TS2);
				break;
			
			case 9:
				stopTheCar();
				while(getYaw(&dYaw));
			case 8:
				//setSpeed(1, 30, 30);
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
				//setSpeed(1, 30, 30);
				setSpeed(2, TS2, TS2);
				break;
			
			case 6:
				stopTheCar();
				while(getYaw(&dYaw));
			case 2:
				//setSpeed(1, 30, 30);
				setSpeed(3, TS2, TS2);
				break;
			
			default:
				setSpeed(0, DS2, DS2);
				break;
		}	
	}
}


/*
void Tracking(u8 _nD) {
	while(getYaw(&yaw));
	if(yaw > dYaw - 15 && yaw < dYaw + 15) {
		switch(TrackCheck()) {
			case 0:
				setSpeed(_nD, DS2, DS2);
				break;
			
			case 2:
				setSpeed(2+_nD, TS1, TS2);
				break;
		
			case 4:
				setSpeed(3-_nD, TS2, TS1);
				break;
			
			default:
				setSpeed(_nD, DS1, DS1);
				break;
		}
	} else if(yaw < dYaw - 15) {
		setSpeed(2, TS2, TS2);
		while(yaw < dYaw - 5) {
			getYaw(&yaw);
		}
		stopTheCar();
	} else if(yaw > dYaw + 15) {
		setSpeed(3, TS2, TS2);
		while(yaw > dYaw + 5) {
			getYaw(&yaw);
		}
	}
}
*/


/*
void Tracking(u8 _nD) {
	if(_nD) {
		switch(TrackCheck()) {
			case 0:								// backward
				setSpeed(1, DS2, DS2);
				break;
			
			case 4:
				setSpeed(2, TS1, TS2);
				break;
			
			case 8:
				setSpeed( 3, TS2, TS1);
				break;
			
			case 1:
				stopTheCar();
				while(getYaw(&yaw));
				if(yaw > dYaw + 15) {
					target = dYaw + 5;
					setSpeed(3, TS1, TS2);
					while(yaw > target) {
						getYaw(&yaw);
					}
				} else 
					setSpeed(2, TS2, TS1);
				break;
			
			case 2:
				stopTheCar();
				while(getYaw(&yaw));
				if(yaw < dYaw - 15) {
					target = dYaw - 5;
					setSpeed(2, TS2, TS1);
					while(yaw < target) {
						getYaw(&yaw);
					}
				}
				setSpeed(3, TS1, TS2);
				break;
			
			case 5:
				setSpeed(2, TS2, TS2);
				break;
			
			case 10:
				setSpeed(3, TS2, TS2);
				break;
			
			case 9:
				stopTheCar();
				while(getYaw(&dYaw));
				setSpeed(2, TS1, TS2);
				while((TrackCheck()&2) == 0);
				stopTheCar();
				break;
			
			case 6:
				stopTheCar();
				while(getYaw(&dYaw));
				setSpeed(3, TS2, TS1);
				while((TrackCheck()&1) == 0);
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
				printf(":Puse\n");
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
			
			case 2:
				setSpeed( 3, TS2, TS1);
				break;
			
			case 4:
				stopTheCar();
				while(getYaw(&yaw));
				if(yaw > dYaw + 15) {
					target = dYaw + 5;
					setSpeed(3, TS1, TS2);
					while(yaw > target) {
						getYaw(&yaw);
					}
				} else 
					setSpeed(2, TS2, TS1);
				break;
			
			case 8:
				stopTheCar();
				while(getYaw(&yaw));
				if(yaw < dYaw - 15) {
					target = dYaw - 5;
					setSpeed(2, TS2, TS1);
					while(yaw < target) {
						getYaw(&yaw);
					}
				} else 
					setSpeed(3, TS1, TS2);
				break;
				
			case 5:
				setSpeed(2, TS2, TS2);
				break;
				
			case 10:
				setSpeed(3, TS2, TS2);
				break;
			
			case 9:
				stopTheCar();
				while(getYaw(&dYaw));
				setSpeed(2, TS1, TS2);
				while((TrackCheck()&2) == 0);
				stopTheCar();
				break;
			
			case 6:
				stopTheCar();
				while(getYaw(&dYaw));
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
				stopTheCar();
				printf(":Puse\n");
				break;
			
			default:
				stopTheCar();
				break;
		}
	}
}


*/
void PreDeal(void) {
	/*
	MPU_Init();					//初始化MPU6050
	while(mpu_dmp_init());
	printf("DMP ReInit OK\n");
	*/
	while(getYaw(&dYaw));
	setSpeed(0, DS2, DS2);
	
	TaskStart(1000);
	while(Tasking()) {
		Tracking(0);
		LightCheck();
	}
	TaskClose();
	
	stopTheCar();
	
	delay_ms(200);
	
}


void Finding(u8 _nD, u8 _nT) {
	printf(":light is seeked\n");
	
	stopTheCar();
	delay_ms(50);
	setSpeed(3-_nT, TS2, TS2);
	
	#define dReg 7
	
	switch(_nT) {
		case 1:
			target = dYaw + 90 - dReg;
			while(getYaw(&yaw));
			while(yaw < target) {
				getYaw(&yaw);
			}
			break;
			
		case 0:
			target = dYaw - 90 + dReg;
			while(getYaw(&yaw));
			while(yaw > target) {
				getYaw(&yaw);
			}
			break;
	}
	
	setSpeed(1, DS1, DS1);
	delay_ms(900);
	
	setSpeed(0, DS1, DS1);
	delay_ms(150);
	while(AD_Value[8] < 250);
	
	setSpeed(2+_nT, TS2, TS2);
	
	switch(_nT) {
		case 1:
			target = dYaw + dReg;
			while(getYaw(&yaw));
			while(yaw > target) {
				getYaw(&yaw);
			}
			break;
		
		case 0:
			target = dYaw - dReg;
			while(getYaw(&yaw));
			while(yaw < target) {
				getYaw(&yaw);
			}
			break;
	}
	
	stopTheCar();
	printf("light is finded\n");
	bFind = 0;
	
	delay_ms(50);
}


void Seeking(void) {
	LightCheck();
	
	if(nToward == 2) {
		printf(":NO LIGHT\n");
		ADC_PrintValue();
		Tracking(0);
		
		u32 iDistance = Ten_Times_Trig(GPIO_Pin_7);
		printf("::DISTANCE %u\n", iDistance);
		
		if(Ten_Times_Trig(GPIO_Pin_7) > 1 && Ten_Times_Trig(GPIO_Pin_7) < DISTANCE) {
			LightCheck();
			if(nToward == 2) {
				stopTheCar();
				bSeek = 0;
				printf("distance limit\n");
			}
		}
		
		if(AD_Value[9] > 1250 && iDistance > 1 && iDistance < 10000) {
			bSeek = 0;
			stopTheCar();
			printf("color limit\n");
		}
		
		return;
	}
	
	if(nToward)
		printf(":BACK  ");
	else
		printf(":FRONT ");
	
	Tracking(nToward);
	
	printf("%4d, %4d, %4d, %4d - %4d, %4d, %4d, %4d\n", AD_Value[0], AD_Value[1], AD_Value[2], AD_Value[3], AD_Value[4], AD_Value[5], AD_Value[6], AD_Value[7]);
	printf("       left    %4d, right   %4d\n", AD_Value[10], AD_Value[11]);
	
	if(AD_Value[10] < 1600) {
		bFind = 1;
		nTurn = 0;
	} else if(AD_Value[11] < 1600) {
		bFind = 1;
		nTurn = 1;
	}
	
	if(nToward == 0 && Ten_Times_Trig(GPIO_Pin_7) > 1 && Ten_Times_Trig(GPIO_Pin_7) < DISTANCE) {
		printf("seek distance limit\n");
		if(AD_Value[0] > 1000 && AD_Value[0] > AD_Value[3]) {
			bFind = 1;
			nTurn = 1;
		} else if(AD_Value[3] > 1000 && AD_Value[3] > AD_Value[0]) {
			bFind = 1;
			nTurn = 0;
		}
	} else if(nToward == 1 && AD_Value[9] > 1250) {
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
