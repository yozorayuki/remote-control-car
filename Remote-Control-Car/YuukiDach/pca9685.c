/**
  ******************************************************************************
  * @author        Yuuki_Dach
  * @version       V1.0.0
  * @date          16-October-2016
  * @description   Functions of PCA9685(PWM)
  ******************************************************************************
  * @attention
  *
  * PCA9865 IS USED AS AN I2C-BUS CONTROLLED 16-CHANNEL CONTROLLER.
  *
  * <h2><center>&copy; COPYRIGHT 2016 Yuuki_Dach</center></h2>
  ******************************************************************************
  */
	
#include "pca9685.h"
#include "stm32f10x.h"

void PCA9685_I2C_GPIO_Config(void) {
    GPIO_InitTypeDef GPIO_InitStructure;
  
    RCC_APB2PeriphClockCmd(I2C_GPIO_CLK, ENABLE);
    RCC_APB1PeriphClockCmd(I2C_CLK, ENABLE);
  
    GPIO_InitStructure.GPIO_Pin = I2C_SCL | I2C_SDA;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
    GPIO_Init(I2C_GPIO, &GPIO_InitStructure);
}


void PCA9685_I2C_Mode_Config(void) {
    I2C_InitTypeDef I2C_InitStructure;
  
    I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
    I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
    I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
    I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
    I2C_InitStructure.I2C_ClockSpeed = I2C_CLOCK_SPEED;
    
    I2C_Init(PCA9685_I2C, &I2C_InitStructure);
    I2C_Cmd(PCA9685_I2C, ENABLE);
}


void PCA9685_I2C_Init(void) {
    PCA9685_I2C_GPIO_Config();
    PCA9685_I2C_Mode_Config();
}


ErrorStatus PCA9685_I2C_HasSlaveAtAddress(const uint8_t Address) {
    //Add address that to be compared
    return SUCCESS;
}


void PCA9685_I2C_BeginTransmission(uint8_t Address) {
    I2C_GenerateSTART(PCA9685_I2C, ENABLE);
  
    // Make sure the START condition has been released on the bus 
    while (!I2C_CheckEvent(PCA9685_I2C, I2C_EVENT_MASTER_MODE_SELECT));
  
    I2C_Send7bitAddress(PCA9685_I2C, Address << 1, I2C_Direction_Transmitter);
  
    // Make sure a slave has acknowledge the address
    while (!I2C_CheckEvent(PCA9685_I2C, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
}


void PCA9685_I2C_Write (uint8_t Data) {
    I2C_SendData(PCA9685_I2C, Data);
    while (!I2C_CheckEvent(PCA9685_I2C, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
}


void PCA9685_I2C_EndTransmission(void) {
    I2C_GenerateSTOP(PCA9685_I2C, ENABLE);
}


/**
 * @brief	Initializes the PCA9685
 * @param	None
 * @retval	1: A PCA9685 at address [Address] has been initialized
 * @retval	0: Initialization failed
 */
uint8_t PCA9685_Init(PCA9685_Init_TypeDef *PCA9685_InitStruct) {
    uint8_t twiStatus = 0, mode2 = 0;
  
    assert_param(IS_PCA9685_INV_OUTPUTS(PCA9685_InitStruct->InvOutputs));
    assert_param(IS_PCA9685_OUTPUT_DRIVER(PCA9685_InitStruct->OutputDriver));
    assert_param(IS_PCA9685_OUTPUT_NOT_EN(PCA9685_InitStruct->OutputNotEn));
    assert_param(IS_PCA9685_FREQUENCY(PCA9685_InitStruct->PWMFrequency));

    PCA9685_I2C_Init();
	
    if (PCA9685_I2C_HasSlaveAtAddress(PCA9685_InitStruct->Address)) {
        // Only when Sleep is 1 then we can set its frequency
        PCA9685_I2C_BeginTransmission(PCA9685_InitStruct->Address);
        PCA9685_I2C_Write(MODE1);
        PCA9685_I2C_Write(1 << MODE1_SLEEP);
        PCA9685_I2C_EndTransmission();

        /* PRE_SCALE Register:
        * Set to value specified in PCA9685_InitStruct->PWMFrequency;
        * Has to be set when device is in sleep mode
        */
        PCA9685_I2C_BeginTransmission(PCA9685_InitStruct->Address);
        PCA9685_I2C_Write(PRE_SCALE);
        PCA9685_I2C_Write(PCA9685_InitStruct->PWMFrequency);
        PCA9685_I2C_EndTransmission();

        /* MODE1 Register:
        * Internal clock
        * Register Auto-Increment enabled
        * Normal mode
        * Does not respond to subaddresses
        * Responds to All Call I2C-bus address
        */
        PCA9685_I2C_BeginTransmission(PCA9685_InitStruct->Address);
        PCA9685_I2C_Write(MODE1);
        PCA9685_I2C_Write((1 << MODE1_AI) | (1 << MODE1_ALLCALL));
        PCA9685_I2C_EndTransmission();

        /* MODE2 Register:
        * Outputs change on STOP command
        */
        mode2 = (PCA9685_InitStruct->InvOutputs << MODE2_INVRT) |
                (PCA9685_InitStruct->OutputDriver << MODE2_OUTDRV)|
                (PCA9685_InitStruct->OutputNotEn  << MODE2_OUTNE0);
        PCA9685_I2C_BeginTransmission(PCA9685_InitStruct->Address);
        PCA9685_I2C_Write(MODE2);
        PCA9685_I2C_Write(mode2);
        PCA9685_I2C_EndTransmission();
    }
	
    PCA9685_WakeUp(PCA9685_InitStruct->Address);
  
    return twiStatus;
}


/**
 * @brief	Sets a specific output for a PCA9685
 * @param	Address: The address to the PCA9685
 * @param	Output: The output to set
 * @param	OnValue: The value at which the output will turn on
 * @param	OffValue: The value at which the output will turn off
 * @retval	None
 */
void PCA9685_SetOutput(uint8_t Address, uint8_t Output, uint16_t OnValue, uint16_t OffValue) {
    // Optional: PCA9685_I2C_SlaveAtAddress(Address), might make things slower
    if (Output <= MAX_OUTPUT_INDEX && OnValue <= MAX_OUTPUT_VALUE && OffValue <= MAX_OUTPUT_VALUE) {
        PCA9685_I2C_BeginTransmission(Address);
        PCA9685_I2C_Write(LEDn_ON_L(Output));
        PCA9685_I2C_Write(OnValue & 0xFF);			// LEDn_ON_L
        PCA9685_I2C_Write((OnValue >> 8) & 0xF);	// LEDn_ON_H
        PCA9685_I2C_Write(OffValue & 0xFF);			// LEDn_OFF_L
        PCA9685_I2C_Write((OffValue >> 8) & 0xF);	// LEDn_OFF_H
        PCA9685_I2C_EndTransmission();
    }
}


/**
 * @brief	Sets all outputs for a PCA9685
 * @param	Address: The address to the PCA9685
 * @param	OnValue: The value at which the outputs will turn on
 * @param	OffValue: The value at which the outputs will turn off
 * @retval	None
 */
void PCA9685_SetAll(uint8_t Address, uint16_t OnValue, uint16_t OffValue) {
    if (OnValue <= MAX_OUTPUT_VALUE && OffValue <= MAX_OUTPUT_VALUE) {
        PCA9685_I2C_BeginTransmission(Address);
        PCA9685_I2C_Write(ALL_LED_ON_L);
        PCA9685_I2C_Write(OnValue & 0xFF);			// ALL_LED_ON_L
        PCA9685_I2C_Write((OnValue >> 8) & 0xF);	// ALL_LED_ON_H
        PCA9685_I2C_Write(OffValue & 0xFF);			// ALL_LED_OFF_L
        PCA9685_I2C_Write((OffValue >> 8) & 0xF);	// ALL_LED_OFF_H
        PCA9685_I2C_EndTransmission();
    }
}


/**
 * @brief	Sets a specific output for a PCA9685 based on an approximate duty cycle
 * @param	Address: The address to the PCA9685
 * @param	Output: The output to set
 * @param	DutyCycle: The duty cycle for the output
 * @retval	None
 */
void PCA9685_SetDutyCycleForOutput(uint8_t Address, uint8_t Output, uint8_t DutyCycle) {
    uint16_t offValue = DutyCycle * 41; // 0 - 4100
    if (offValue > MAX_OUTPUT_VALUE) offValue = MAX_OUTPUT_VALUE;
    PCA9685_SetOutput(Address, Output, 0, offValue);
}


/**
 * @brief	Enter sleep mode
 * @param	Address: The address to the PCA9685
 * @retval	None
 */
void PCA9685_Sleep(uint8_t Address) {
    PCA9685_I2C_BeginTransmission(Address);
    PCA9685_I2C_Write(MODE1);
    PCA9685_I2C_Write(1 << MODE1_SLEEP);
    PCA9685_I2C_EndTransmission();
}


/**
 * @brief	Requests a given amount of data from a slave
 * @param	Address: The address to the slave to request from
 * @param	Storage: Pointer to where the data should be stored
 * @param	ByteCount: The amount of bytes to receive
 * @retval	...
 * 	- SUCCESS:
 * 	- ERROR:
 */
ErrorStatus PCA9685_I2C_RequestFrom(uint8_t Address, uint8_t* Storage, uint8_t NumByteToRead) {
    // While the bus is busy 
    while (I2C_GetFlagStatus(PCA9685_I2C, I2C_FLAG_BUSY));

    I2C_GenerateSTART(PCA9685_I2C, ENABLE);

    while (!I2C_CheckEvent(PCA9685_I2C, I2C_EVENT_MASTER_MODE_SELECT));
    I2C_Send7bitAddress(PCA9685_I2C, Address << 1, I2C_Direction_Receiver);
    while (!I2C_CheckEvent(PCA9685_I2C, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));


    // While there is data to be read
    while (NumByteToRead) {
        if (NumByteToRead == 0) {
          // Send STOP Condition
          I2C_GenerateSTOP(PCA9685_I2C, ENABLE);
        }

        // Check if data has been received
        if (I2C_CheckEvent(PCA9685_I2C, I2C_EVENT_MASTER_BYTE_RECEIVED)) {
            // Read a byte from the slave
            *Storage = I2C_ReceiveData(PCA9685_I2C);

            // Point to the next location where the next byte read will be saved
            Storage++;

            // Decrement the read bytes counter
            NumByteToRead--;
        }
    }

    return SUCCESS;
}


/**
 * @brief	Exit sleep mode
 * @param	Address: The address to the PCA9685
 * @retval	None
 */
void PCA9685_WakeUp(uint8_t Address) {
    uint8_t registerData = 0;
    PCA9685_I2C_BeginTransmission(Address);
    PCA9685_I2C_Write(MODE1);
    PCA9685_I2C_EndTransmission();
    PCA9685_I2C_RequestFrom(Address, &registerData, 1);

    registerData &= ~(1 << MODE1_SLEEP);

    PCA9685_I2C_BeginTransmission(Address);
    PCA9685_I2C_Write(MODE1);
    PCA9685_I2C_Write(registerData);
    PCA9685_I2C_EndTransmission();
}

/******************* (C) COPYRIGHT 2016 Yuuki_Dach *************END OF FILE****/

