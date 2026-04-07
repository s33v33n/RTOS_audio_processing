#include "fft_setup.h"
#include "Audio_pcm_from_pdm.h"

/* Required (lost pcm buffer) */
extern uint16_t pcm_buffer[16];

#define PCM_BUFFER_SIZE 16
#define FFT_BUFFER_SIZE 1024

float fftBuffIn[FFT_BUFFER_SIZE];
float fftBuffComplex[FFT_BUFFER_SIZE];
float fftBuffOut[FFT_BUFFER_SIZE / 2];

uint8_t fftCmplt = 0;

arm_rfft_fast_instance_f32 fftHandler;
extern UART_HandleTypeDef huart2; // Audio_pcm_from_pdm.h

// structure to pass
fftArgs_t fftArgs = {0};


void fft_Init(){

	// FFT init
	arm_rfft_fast_init_f32(&fftHandler, FFT_BUFFER_SIZE);

	fftArgs.pcm_buffer_size = PCM_BUFFER_SIZE;
	fftArgs.fft_buffer_size = FFT_BUFFER_SIZE;

	fftArgs.fftBuffIn = fftBuffIn;
	fftArgs.fftComplex = fftBuffComplex;
	fftArgs.fftBuffOut = fftBuffOut;

	fftArgs.fftCmplt = fftCmplt;
	fftArgs.fftHandler = &fftHandler;
	fftArgs.uartHandle = &huart2;

	// ... and my lost pcm buffer
	fftArgs.source_pcm_data = pcm_buffer;

}


fftArgs_t *GetfftArgs(void){

	return &fftArgs;
}
