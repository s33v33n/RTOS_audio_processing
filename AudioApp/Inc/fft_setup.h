#ifndef FFT_SETUP_H
#define FFT_SETUP_H

// Includes are centralized in AudioApp.h


// One UART frame: sync word + magnitude spectrum (sent in a single DMA transfer)
typedef struct{
	uint32_t sync;
	float mag[FFT_BUFFER_SIZE / 2];
} fftFrame_t;

// Box to be passed to thread
typedef struct{

	uint16_t fft_buffer_size;
	float *fftBuffIn;
	float *fftComplex;
	const float *window;
	fftFrame_t *frame;
	arm_rfft_fast_instance_f32 *fftHandler;
	UART_HandleTypeDef *uartHandle;

} fftArgs_t;

void fft_Init(void);

fftArgs_t *GetfftArgs(void);

#endif /* FFT_SETUP_H */
