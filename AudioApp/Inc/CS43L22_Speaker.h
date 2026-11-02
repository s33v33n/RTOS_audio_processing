#ifndef CS43L22_H
#define CS43L22_H

// Includes are centralized in AudioApp.h

// I2C
#define CS43L22_I2C_ADDR         	(0x4A << 1) // [7:2] - fixed to 100101 , [1] - '0' (connected to GND) , left shift for write operation
#define CS43L22_I2C_HANDLE 			&hi2c1

//I2S
#define CS43L22_I2S_HANDLE 			&hi2s3


HAL_StatusTypeDef CS43L22_RegWrite(uint8_t regAddr, uint8_t regData);
HAL_StatusTypeDef CS43L22_RegRead(uint8_t regAddr, uint8_t *regData);
HAL_StatusTypeDef CS43L22_Init(void);


void CS43L22_Reset(void);


#endif /*CS43L22_H*/
