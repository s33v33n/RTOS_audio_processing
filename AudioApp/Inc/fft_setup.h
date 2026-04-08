#ifndef FFT_SETUP_H
#define FFT_SETUP_H

// Includes are centralized in AudioApp.h


// Box to be passed to thread
typedef struct{

	uint16_t pcm_buffer_size;
	uint16_t fft_buffer_size;
	float *fftBuffIn;
	float *fftComplex;
	float *fftBuffOut;
	uint8_t fftCmplt;
	arm_rfft_fast_instance_f32 *fftHandler;
	UART_HandleTypeDef *uartHandle;

	// ... and my PCM data
	uint16_t *source_pcm_data;

} fftArgs_t;

void fft_Init(void);

fftArgs_t *GetfftArgs(void);

#endif /* FFT_SETUP_H */
