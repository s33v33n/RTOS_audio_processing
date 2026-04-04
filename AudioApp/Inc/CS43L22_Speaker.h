#ifndef CS43L22_H
#define CS43L22_H

#include "main.h"

// declaration is in another file
extern I2C_HandleTypeDef hi2c1;
extern I2S_HandleTypeDef hi2s3;

// I2C
#define CS43L22_I2C_ADDR         	(0x4A << 1) // [7:2] - fixed to 100101 , [1] - '0' (connected to GND) , left shift for write operation
#define CS43L22_I2C_HANDLE 			&hi2c1

//I2S
#define CS43L22_I2S_HANDLE 			&hi2s3

// Device ID
#define CS43L22_REG_ID           	0xE0 // [7:3] - fixed to 11100 , [2:0] - reserved


HAL_StatusTypeDef CS43L22_RegWrite(uint8_t regAddr, uint8_t regData);
HAL_StatusTypeDef CS43L22_RegRead(uint8_t regAddr, uint8_t *regData);

HAL_StatusTypeDef CS43L22_Init(void);
void CS43L22_Reset(void);


#endif /*CS43L22_H*/
