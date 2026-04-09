#ifndef AUDIO_PCM_FROM_PDM_H
#define AUDIO_PCM_FROM_PDM_H

// Includes are centralized in AudioApp.h

// Box to be passed to thread
typedef struct{
	uint16_t pdm_buffer_size;
	uint16_t pcm_buffer_size;
	uint16_t *pdm_buffer;
	uint16_t *pcm_buffer;

	int16_t *stereo_tx_buffer;

	volatile uint8_t *pFullPdmBuffer;
	osSemaphoreId_t dataReadySem;

	osSemaphoreId_t uartSem;
	UART_HandleTypeDef *uartHandle;

	PDM_Filter_Handler_t *FilterHandler;

} AudioTaskArgs_t;


// Init library to process PDM data
void PDM_Init(uint32_t AudioFreq, uint32_t in_channels, uint32_t out_channels);

void SetAudioData(void);

AudioTaskArgs_t *GetAudioArgs(void);


#endif /* AUDIO_PCM_FROM_PDM_H */
