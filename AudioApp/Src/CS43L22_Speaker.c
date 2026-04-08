#include "AudioApp.h"

// #define ONE_BYTE 1

HAL_StatusTypeDef CS43L22_RegWrite(uint8_t regAddr, uint8_t regData){

	return HAL_I2C_Mem_Write(CS43L22_I2C_HANDLE, CS43L22_I2C_ADDR, regAddr,I2C_MEMADD_SIZE_8BIT, &regData, ONE_BYTE, HAL_MAX_DELAY);
}

HAL_StatusTypeDef CS43L22_RegRead(uint8_t regAddr, uint8_t *regData){

	return HAL_I2C_Mem_Read(CS43L22_I2C_HANDLE, CS43L22_I2C_ADDR, regAddr, I2C_MEMADD_SIZE_8BIT, regData, ONE_BYTE, HAL_MAX_DELAY);
}


void CS43L22_Reset(void){

	HAL_GPIO_WritePin(CS43L22_RESET_GPIO_Port, CS43L22_RESET_Pin, GPIO_PIN_RESET);
	osDelay(20);

	HAL_GPIO_WritePin(CS43L22_RESET_GPIO_Port, CS43L22_RESET_Pin, GPIO_PIN_SET);
	osDelay(20);
}

HAL_StatusTypeDef CS43L22_Init(void){
	HAL_StatusTypeDef Init_status = HAL_OK;
	uint8_t Data = 0;

	/* 1 & 2. RESET */
	CS43L22_Reset();


	/* 3.  MY SET UP */

	// Registers
	uint8_t init_regs[] = {

			0x02, // Power Ctl 1
			0x04, // Power Ctl 2
			0x05, // Clocking Ctl
			0x06, // Interface Ctl
			0x0A, // Analog Zero-Cross and SR
			0x27, // Limit Ctl 1
			0x1A, // PCM Volume A
			0x1B, // PCM Volume B
			0x20, // Master Volume A
			0x21  // Master Volume B
	};

	// Values
	uint8_t init_vals[] = {

			0x01, // 0x02: Keep powered down until magic sequence is done
			0xAF, // 0x04: Headphones ON, Speaker OFF
			0x81, // 0x05: Auto-detect clock, MCLK divided by 2
			0x04, // 0x06: Slave mode, I2S Philips Standard, 16-bit
			0x00, // 0x0A: Disable Zero-Cross
			0x00, // 0x27: Disable Limiter
			0x0A, // 0x1A: PCM Vol A (0 dB)
			0x0A, // 0x1B: PCM Vol B (0 dB)
			0xE0, // 0x20: Master Vol A (-16 dB)
			0xE0  // 0x21: Master Vol B (-16 dB)
	};

	uint8_t array_elements = sizeof(init_regs) / sizeof(init_regs[0]);

	for(uint8_t i = 0; i < array_elements; i++){

		Init_status = CS43L22_RegWrite(init_regs[i], init_vals[i]);
		if( Init_status != HAL_OK){
			return Init_status;
		}
	}



	/* 4. Required initialization */
	Init_status |= CS43L22_RegWrite(0x00, 0x99);
	Init_status |= CS43L22_RegWrite(0x47, 0x80);

	Init_status |= CS43L22_RegRead(0x32, &Data);
	Init_status |= CS43L22_RegWrite(0x32, Data | 0x80);

	Init_status |= CS43L22_RegRead(0x32, &Data);
	Init_status |= CS43L22_RegWrite(0x32, Data & ~(0x80));

	Init_status |= CS43L22_RegWrite(0x00, 0x00);

	if(Init_status != HAL_OK){
		return Init_status;
	}

	/* 5 & 6. Power up the Codec */
	Init_status = CS43L22_RegWrite(0x02, 0x9E);

	return Init_status;
}



