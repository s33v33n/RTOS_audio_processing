#include "AudioApp.h"


// pcmData thread
void AudioTask_Run(void)
{
	// My whole AudioApp initialization
	AudioApp_Init();

	AudioTaskArgs_t *args = GetAudioArgs();
	uint16_t *pPDMdata = NULL;

	/* Infinite loop */
	for(;;){

		if(osSemaphoreAcquire(pdmDataReadyHandle, osWaitForever) == osOK){

			// 1. select PDM data for processing
			pPDMdata = ( *(args->pFullPdmBuffer) == 0) ? args->pdm_buffer : &(args->pdm_buffer[ (args->pdm_buffer_size) / 2]);

			// 2. and process data (MONO)
			PDM_Filter(pPDMdata, args->pcm_buffer, (args->FilterHandler));

			// 3. Hand the block over to FFT task (queue copies the data)
			osMessageQueuePut(pcmFftQueueHandle, args->pcm_buffer, 0, 0);

			// 4. Hand the block over to the output (queue copies the data, TX callbacks consume it)
			osMessageQueuePut(pcmTxQueueHandle, args->pcm_buffer, 0, 0);

		}

	}
}


// pcmFFT thread
void FftTask_Run(void)
{
	fft_Init();

	fftArgs_t *args = GetfftArgs();
	uint16_t fftIndex = 0;
	int16_t pcmBlock[PCM_BUFFER_SIZE];

	// 2/N (one-sided spectrum) / 0.5 (Hann gain) -> magnitude in PCM units
	const float scale = 4.0f / FFT_BUFFER_SIZE;


	/* Infinite loop */
	  for(;;)
	  {
		  if(osMessageQueueGet(pcmFftQueueHandle, pcmBlock, NULL, osWaitForever) == osOK){

			  // Filling the FFT buffer
			  for(uint16_t i = 0; i < PCM_BUFFER_SIZE; i++){
				  args -> fftBuffIn[fftIndex] = (float)pcmBlock[i] * args -> window[fftIndex];
				  fftIndex++;


				  /* When FFT buffer is full -> count FFT
				   * FFT is computed in MONO format - doesn't need to
				   * get rid of doubled samples
				   * */
				  if(fftIndex >= (args -> fft_buffer_size)){

					  // 1. Calculate fft values (input: read audio data, output: complex number)
					  arm_rfft_fast_f32(args -> fftHandler, args -> fftBuffIn, args -> fftComplex, 0);

					  // 2. Skip this frame if UART DMA is still sending the previous one
					  if(osSemaphoreAcquire(TXuartHandle, 0) == osOK)
					  {
						  // 3. Calculate Magnitude from complex samples ([0] = DC, [1] = Nyquist)
						  arm_cmplx_mag_f32(args -> fftComplex, args -> frame -> mag, (args -> fft_buffer_size) / 2);
						  args -> frame -> mag[0] = fabsf(args -> fftComplex[0]);
						  arm_scale_f32(args -> frame -> mag, scale, args -> frame -> mag, (args -> fft_buffer_size) / 2);

						  // 4. Send results via UART (sync + spectrum in one DMA transfer)
						  if(HAL_UART_Transmit_DMA(args->uartHandle, (uint8_t *)(args->frame), sizeof(fftFrame_t)) != HAL_OK){
							  osSemaphoreRelease(TXuartHandle);
						  }
					  }

					  fftIndex = 0;
				  }
			 }

		  }

	  }
}
