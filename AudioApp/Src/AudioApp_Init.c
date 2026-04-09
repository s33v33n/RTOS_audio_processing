#include "AudioApp.h"


void AudioApp_Init(void){

	/* --- start_pcmData --- */

	// 1. Init Audio settings
	SetAudioData();

	// 2. Get data for Audio Settings
	AudioTaskArgs_t *args = GetAudioArgs();

	// 3. Init DAC Codec
	if (CS43L22_Init() != HAL_OK)
		{
			char print_buf[] = "CS43L22 initialization failed\r\n";
			HAL_UART_Transmit(args->uartHandle, (uint8_t *)print_buf, strlen(print_buf), HAL_MAX_DELAY);

			// Trap thread
			while(1){
				osDelay(1000);
			}
		}

	// 4. Start Codec (DMA transmission)
	HAL_I2S_Transmit_DMA(CS43L22_I2S_HANDLE, (uint16_t *)args -> stereo_tx_buffer, (args->pcm_buffer_size) * 2);



	/* --- start_pcmFFT --- */

	// 1. FFT Init
	fft_Init();
}
