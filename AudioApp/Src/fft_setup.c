#include "AudioApp.h"


float fftBuffIn[FFT_BUFFER_SIZE];
float fftBuffComplex[FFT_BUFFER_SIZE];
float fftWindow[FFT_BUFFER_SIZE];

fftFrame_t fftFrame = { .sync = FFT_FRAME_SYNC };

arm_rfft_fast_instance_f32 fftHandler;


fftArgs_t fftArgs = {0};

void fft_Init(){

	// FFT init
	arm_rfft_fast_init_f32(&fftHandler, FFT_BUFFER_SIZE);

	// Hann window - reduces spectral leakage
	for(uint16_t n = 0; n < FFT_BUFFER_SIZE; n++){
		fftWindow[n] = 0.5f - 0.5f * cosf(2.0f * PI * n / (FFT_BUFFER_SIZE - 1));
	}

	fftArgs.fft_buffer_size = FFT_BUFFER_SIZE;

	fftArgs.fftBuffIn = fftBuffIn;
	fftArgs.fftComplex = fftBuffComplex;
	fftArgs.window = fftWindow;
	fftArgs.frame = &fftFrame;

	fftArgs.fftHandler = &fftHandler;
	fftArgs.uartHandle = &huart2;
}


fftArgs_t *GetfftArgs(void){

	return &fftArgs;
}
