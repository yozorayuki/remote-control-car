/**
  ******************************************************************************
  * @author  			 Yuuki_Dach
  * @version 			 V1.0.0
  * @date          16-October-2016
  * @description   Header of mechanical_arm.c
  ******************************************************************************
  * @attention
  *
  * HERE ARE SOME BASIC DEFINITONS AND FUNCTION DECLARATIONS OF THE PCA9685
  *
  * <h2><center>&copy; COPYRIGHT 2016 Yuuki_Dach</center></h2>
  ******************************************************************************
  */
	

#ifndef __PCA9685_H
#define __PCA9685_H

#include "stm32f10x.h"

#define PCA_ADDRESS     0x40

#define PCA9685_I2C     I2C1
#define I2C_GPIO        GPIOB
#define I2C_SCL         GPIO_Pin_6
#define I2C_SDA         GPIO_Pin_7
#define I2C_CLK         RCC_APB1Periph_I2C1
#define I2C_GPIO_CLK    RCC_APB2Periph_GPIOB
#define I2C_CLOCK_SPEED	200000

#define MODE1         0x00
#define MODE1_ALLCALL	0
#define MODE1_SUB3		1
#define MODE1_SUB2		2
#define MODE1_SUB1		3
#define MODE1_SLEEP		4
#define MODE1_AI		  5
#define MODE1_EXTCLK	6
#define MODE1_RESTART	7

#define MODE2         0x01
#define MODE2_OUTNE0	0
#define MODE2_OUTNE1	1
#define MODE2_OUTDRV	2
#define MODE2_OCH		  3
#define MODE2_INVRT		4

#define SUBADR1				0x02
#define SUBADR2				0x03
#define SUBADR3				0x04
#define ALLCALLADR    0x05

#define LEDn_ON_L(n)  (0x06 + ((n) << 2))
#define LEDn_ON_H(n)  (0x07 + ((n) << 2))
#define LEDn_OFF_L(n)	(0x08 + ((n) << 2))
#define LEDn_OFF_H(n)	(0x09 + ((n) << 2))

#define ALL_LED_ON_L  0xFA
#define ALL_LED_ON_H  0xFB
#define ALL_LED_OFF_L	0xFC
#define ALL_LED_OFF_H	0xFD

#define PRE_SCALE     0xFE

#define MAX_OUTPUT_INDEX	15
#define MAX_OUTPUT_VALUE	0xFFF

/**
 * @brief	PCA9685 Inverted outputs
 */
typedef enum
{
	PCA9685_NotInvOutputs =	0,
	PCA9685_InvOutputs = 1
} PCA9685_InvOutputs_TypeDef;

#define IS_PCA9685_INV_OUTPUTS(INVRT)	(((INVRT) == PCA9685_NotInvOutputs) || \
                                       ((INVRT) == PCA9685_InvOutputs))

/**
 * @brief  PCA9685 Output driver types
 */
typedef enum
{
	PCA9685_OutputDriver_OpenDrain =	0,
	PCA9685_OutputDriver_TotemPole =	1
} PCA9685_OutputDriver_TypeDef;

#define IS_PCA9685_OUTPUT_DRIVER(OUTPUT_DRIVER)	(((OUTPUT_DRIVER) == PCA9685_OutputDriver_OpenDrain) || \
                                                 ((OUTPUT_DRIVER) == PCA9685_OutputDriver_TotemPole))

/**
 * @brief	PCA9685 Not enabled LED outputs defines the behaviour of the outputs
 *	when OE is pulled low
 */
typedef enum
{
	PCA9685_OutputNotEn_0 =	0,
	PCA9685_OutputNotEn_OUTDRV  =	1,
	PCA9685_OutputNotEn_High_Z1 =	2,
	PCA9685_OutputNotEn_High_Z2 =	3
} PCA9685_OutputNotEn_TypeDef;

#define IS_PCA9685_OUTPUT_NOT_EN(OUTNE)	(((OUTNE) >= PCA9685_OutputNotEn_0) && \
                                         ((OUTNE) <= PCA9685_OutputNotEn_High_Z2))

/**
 * @brief	PCA9685 Frequency
 *	Set by prescale = round(25 MHz / (4096 * freq)) - 1
 */
typedef enum
{
	PCA9685_Frequency_200Hz =	32,
	PCA9685_Frequency_100Hz =	65,
	PCA9685_Frequency_60Hz =	108,
	PCA9685_Frequency_50Hz =	130
} PCA9685_Frequency;

#define IS_PCA9685_FREQUENCY(FREQ) ((FREQ) == PCA9685_Frequency_200Hz || \
                                    (FREQ) == PCA9685_Frequency_100Hz || \
                                    (FREQ) == PCA9685_Frequency_60Hz  || \
                                    (FREQ) == PCA9685_Frequency_50Hz)

/**
 * @brief	PCA9685 Init structure definition
 */
typedef struct
{
  uint8_t Address;					                 // the address for the PCA9685
  PCA9685_InvOutputs_TypeDef InvOutputs;		 // if the outputs should be inverted
  PCA9685_OutputDriver_TypeDef OutputDriver; // the output driver
	PCA9685_OutputNotEn_TypeDef OutputNotEn;	 // what the outputs should be when OE=1
	PCA9685_Frequency PWMFrequency;
} PCA9685_Init_TypeDef;


uint8_t PCA9685_Init(PCA9685_Init_TypeDef *PCA9685_InitStruct);

void PCA9685_I2C_GPIO_Config(void);
void PCA9685_I2C_Mode_Config(void);
void PCA9685_I2C_Init(void);

ErrorStatus PCA9685_I2C_HasSlaveAtAddress(const uint8_t Address);
void PCA9685_I2C_BeginTransmission(uint8_t Address);
void PCA9685_I2C_Write (uint8_t Data);
void PCA9685_I2C_EndTransmission(void);

void PCA9685_SetOutput(uint8_t Address, uint8_t Output, uint16_t OnValue, uint16_t OffValue);
void PCA9685_SetAll(uint8_t Address, uint16_t OnValue, uint16_t OffValue);
void PCA9685_SetDutyCycleForOutput(uint8_t Address, uint8_t Output, uint8_t DutyCycle);
void PCA9685_Sleep(uint8_t Address);
void PCA9685_WakeUp(uint8_t Address);
ErrorStatus PCA9685_I2C_RequestFrom(uint8_t Address, uint8_t* Storage, uint8_t NumByteToRead);

#endif /* __PCA9685_H */

/******************* (C) COPYRIGHT 2016 Yuuki_Dach *************END OF FILE****/

