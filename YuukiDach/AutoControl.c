#include "AutoControl.h"
#include "ADCConfig.h"
#include "precompile.h"

#if (__DEBUG__ == __ON__)
#include "WifiControl.h"
#endif 

#include "delay.h"
#include "movement.h"
#include "ultrasonic.h"
#include "movement.h"
#include "final_charge.h"
#include "controller.h"
#include <stdio.h>


u8 bStrat = 0, bEnd = 0, bSeek = 0, bFind;
u8 nLocation = 0;
u8 nToward = 2, nTurn = 2;
u8 nAddCount = 0;
u16 adVal[10];


void AutoControlConfig(void) {
	RCC_APB2PeriphClockCmd(TRACK_RCC, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	
	GPIO_InitStructure.GPIO_Pin = TRACK_FL | TRACK_FR | TRACK_BL | TRACK_BR;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(TRACK_GPIO, &GPIO_InitStructure);
}


void ReStart(void) {
	bStrat = bEnd = bSeek = bFind = 0;
	nAddCount = 0;
	for(int i = 0; i < 10; i++) {
		adVal[i] = 0;
	}
}


u8 TrackCheck(void) {
	u8 re = 0;
	re += GPIO_ReadInputDataBit(TRACK_GPIO, TRACK_FL);
	re += GPIO_ReadInputDataBit(TRACK_GPIO, TRACK_FR)<<1;
	re += GPIO_ReadInputDataBit(TRACK_GPIO, TRACK_BR)<<2;
	re += GPIO_ReadInputDataBit(TRACK_GPIO, TRACK_BL)<<3;
	return re;
}


void Tracking(u8 _toward) {
	if(_toward) {
		switch(TrackCheck()) {
			case 0:
				setSpeed(1, DS, DS);
				break;
			
			case 1:
				setSpeed(2, TS2, TS1);
				delay_ms(10);
				break;
			
			case 4:
				setSpeed(2, TS2, TS1);
				delay_ms(10);
				break;
			
			case 2:
				setSpeed( 3, TS1, TS2);
				delay_ms(10);
				break;
			
			case 8:
				setSpeed(3, TS1, TS2);
				delay_ms(10);
				break;
			
			case 5:
				setSpeed(2, TS2, TS2);
				delay_ms(10);
				break;
			
			case 10:
				setSpeed(3, TS2, TS2);
				delay_ms(10);
				break;
			
			case 9:
				setSpeed(3, TS2, TS1);
				while((TrackCheck()&4) == 0);
				stopTheCar();
				break;
			
			case 6:
				setSpeed(2, TS1, TS2);
				while((TrackCheck()&8) == 0);
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
					setSpeed(0, DS, DS);
					delay_ms(10);
				}
				break;
			
			default:
				stopTheCar();
				break;
		}
	} else {
		switch(TrackCheck()) {
			case 0:
				setSpeed(0, DS, DS);
				break;
			
			case 1:
				setSpeed(2, TS1, TS2);
				delay_ms(10);
				break;
			
			case 4:
				setSpeed(2, TS1, TS2);
				delay_ms(10);
				break;
			
			case 2:
				setSpeed( 3, TS2, TS1);
				delay_ms(10);
				break;
			
			case 8:
				setSpeed(3, TS2, TS1);
				delay_ms(10);
				break;
			
			case 5:
				setSpeed(2, TS2, TS2);
				delay_ms(10);
				break;
			
			case 10:
				setSpeed(3, TS2, TS2);
				delay_ms(10);
				break;
			
			case 9:
				setSpeed(2, TS1, TS2);
				while((TrackCheck()&2) == 0);
				stopTheCar();
				break;
			
			case 6:
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
				/*
				if(bSeek) {
					stopTheCar();
					printf(":Puse\n");
				} else {
					setSpeed(0, DS, DS);
					delay_ms(10);
				}
			*/
				break;
			
			default:
				stopTheCar();
				break;
		}
	}
}


void preDeal(void) {
	setSpeed(FORWARDS, DS, DS);
	while (!(TrackCheck() & 0x3)) {}
    while (Ten_Times_Trig (GPIO_Pin_7) > 50000) Tracking(FORWARDS);    // Set a good distance to insure the car can stop at the middle part.
}



void clearAdVal (void) {
    for (uint8_t i = 0; i < 10; ++i) adVal[i] = 0;
}


/**
  * @param pos : Only FORWARDS(0) and BACKWARDS(1) are allowed to be input
  */
/*void checkDir (uint8_t pos) {
    uint32_t leftVal = 0,  rightVal = 0;
    if (pos == FORWARDS) {
        leftVal  = adVal[3] + adVal[4];
        rightVal = adVal[0] + adVal[1];
        if (leftVal > rightVal) {
            while()
        }
    } else {
    
    }


void findOutDir (void) {
    uint32_t frontVal = 0, backVal = 0;
    cleanAdVal();
    
    for (uint8_t i = 1; i <= 0x8; i << 1) {
        for (uint8_t j = 0; j < 10; ++j) adVal[j] += AD_Value[j];

 #if (__DEBUG__ == __ON__) 
        printf ("%7d", adVal[i]);
 #endif
        
    }

 #if (__DEBUG__ == __ON__) 
        printf ("\r\n");
 #endif
    
    for (uint8_t i = 0; i < 5 ; ++i) frontVal += adVal[i];
    for (uint8_t i = 5; i < 10; ++i) backVal  += adVal[i];
    if (frontVal > backVal + 4000) {
        checkDir (FORWARDS);
    } else if (backVal > frontVal + 4000) {
        checkDir (BACKWARDS);
    }
} */


//void seekLight (void) {
//    findOutDir();
//}


void Finding(s8 _nToward, s8 _nTurn) {
#ifdef _DEBUG_MODE
	printf("pre seek\n");
	ADC_PrintValue();
#endif
	if(_nTurn < 0) {
#ifdef _DEBUG_MODE
		printf("seek left\n");
#endif
		setSpeed(2, 25, 25);
		while(AD_Value[4+5*_nToward]+AD_Value[3+5*_nToward] > AD_Value[1+5*_nToward]+AD_Value[0+5*_nToward]);
	} else {
#ifdef _DEBUG_MODE
		printf("seek right\n");
#endif
		setSpeed(3, 25, 25);
		while(AD_Value[4+5*_nToward]+AD_Value[3+5*_nToward] < AD_Value[1+5*_nToward]+AD_Value[0+5*_nToward]);
	}
	setSpeed(_nToward, 15, 15);
	
#ifdef _DEBUG_MODE
	printf("\nSEEKING\n");
#endif
	while(AD_Value[2+5*_nToward] > 500) {
#ifdef _DEBUG_MODE
		ADC_PrintValue();
#endif
		u8 bFlag = AD_Value[4+5*_nToward]+AD_Value[3+5*_nToward] > AD_Value[1+5*_nToward]+AD_Value[0+5*_nToward];
		if(_nToward) {
			setSpeed(1, 15+2*bFlag, 15-2*bFlag);
		} else {
			setSpeed(0, 15-2*bFlag, 15+2*bFlag);
		}
	}
#ifdef _DEBUG_MODE
	printf("\nSEEK OK\n");
#endif
	ADC_PrintValue();
	setSpeed(1-_nToward, 15, 15);
	delay_ms(800);
	setSpeed(2+(_nTurn>0), 25, 25);
	delay_ms(700);
}


/*void Seeking(void) {
	for(int i = 0; i < 10; i++)
		adValue[i] += AD_Value[i];
	if(nAddCount++ < 16) return;
	
	for(int i = 0; i < 5; i++) {
		adValue[i] = adValue[i]>>4;
		front += adValue[i];
	}
	for(int i = 5; i < 10; i++) {
		adValue[i] = adValue[i]>>4;
		back += adValue[i];
	}
	all = front + back;
	
	if(all < 1000) {	
		stopTheCar();
		toward = 2;
		turn = 0;
		
#ifdef _DEBUG_MODE
		printf("fuck\n");
#endif
	} else {
		if(front > back) {	
			setSpeed(0, 10, 10);
			delay_ms(10);
			toward = 0;
			
#ifdef _DEBUG_MODE
			printf("front light\n");
			ADC_PrintValue();
#endif
			
		} else {					
			setSpeed(1, 10, 10);
			delay_ms(10);
			toward = 1;
			
#ifdef _DEBUG_MODE
			printf("back light\n");
			ADC_PrintValue();
#endif
			
		}
		if(adValue[4+5*toward] > 800) {
			Finding(toward, -1);
		} else if(adValue[0+5*toward] > 800) {
			Finding(toward, 1);
		}
	}
	
	for(int i = 0; i < 10; i++) {
		adValue[i] = 0;
	}
	front = back = 0;
	nAddCount = 0;
}


void AutoControl(void) {
	preDeal();
}*/


