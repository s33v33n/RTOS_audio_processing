/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "CS43L22_Speaker.h"
#include "MP45DT02_microphone.h"
#include "fft_setup.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */



/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */

/* USER CODE END Variables */
/* Definitions for pcmData */
osThreadId_t pcmDataHandle;
const osThreadAttr_t pcmData_attributes = {
  .name = "pcmData",
  .stack_size = 1024 * 4,
  .priority = (osPriority_t) osPriorityRealtime,
};
/* Definitions for pcmFFT */
osThreadId_t pcmFFTHandle;
const osThreadAttr_t pcmFFT_attributes = {
  .name = "pcmFFT",
  .stack_size = 1024 * 4,
  .priority = (osPriority_t) osPriorityLow,
};
/* Definitions for pdmDataReady */
osSemaphoreId_t pdmDataReadyHandle;
const osSemaphoreAttr_t pdmDataReady_attributes = {
  .name = "pdmDataReady"
};
/* Definitions for TXuart */
osSemaphoreId_t TXuartHandle;
const osSemaphoreAttr_t TXuart_attributes = {
  .name = "TXuart"
};
/* Definitions for fftPcmDataReady */
osSemaphoreId_t fftPcmDataReadyHandle;
const osSemaphoreAttr_t fftPcmDataReady_attributes = {
  .name = "fftPcmDataReady"
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void start_pcmData(void *argument);
void start_pcmFFT(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* Create the semaphores(s) */
  /* creation of pdmDataReady */
  pdmDataReadyHandle = osSemaphoreNew(1, 1, &pdmDataReady_attributes);

  /* creation of TXuart */
  TXuartHandle = osSemaphoreNew(1, 1, &TXuart_attributes);

  /* creation of fftPcmDataReady */
  fftPcmDataReadyHandle = osSemaphoreNew(1, 1, &fftPcmDataReady_attributes);

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of pcmData */
  pcmDataHandle = osThreadNew(start_pcmData, NULL, &pcmData_attributes);

  /* creation of pcmFFT */
  pcmFFTHandle = osThreadNew(start_pcmFFT, NULL, &pcmFFT_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_start_pcmData */
/**
  * @brief  Function implementing the pcmData thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_start_pcmData */
void start_pcmData(void *argument)
{
  /* USER CODE BEGIN start_pcmData */

	// 0. Variables
	char print_buf[64]; // help variable

	int16_t uart_tx_buffer[32];   						// MONO tx UART buffer
	memset(uart_tx_buffer, 0, sizeof(uart_tx_buffer));	// clear buffer

	int16_t stereo_tx_buffer[128]; 							// STEREO tx DAC buffer
	memset(stereo_tx_buffer, 0, sizeof(stereo_tx_buffer));	// clear buffer


	// 1. Init Audio settings
	SetAudioData();

	// 2. Get data for thread and select temporary PDM buffer
	AudioTaskArgs_t *args = GetAudioArgs();
	uint16_t *pPDMdata = NULL;

	// 3. Init DAC Codec
	if (CS43L22_Init() != HAL_OK)
		{
		strcpy(print_buf, "Codec initialization failed\r\n");
			HAL_UART_Transmit(args->uartHandle, (uint8_t *)print_buf, strlen(print_buf), HAL_MAX_DELAY);

			// Trap thread
			while(1){
				osDelay(1000);
			}
		}

	// 4. Start Codec (DMA transmission)
	HAL_I2S_Transmit_DMA(CS43L22_I2S_HANDLE, (uint16_t *)stereo_tx_buffer, (args->pcm_buffer_size) * 2);

	/* Infinite loop */
	for(;;){

		if(osSemaphoreAcquire(pdmDataReadyHandle, osWaitForever) == osOK){

			// 1. select PDM data for processing
			pPDMdata = ( *(args->pFullPdmBuffer) == 0) ? args->pdm_buffer : &(args->pdm_buffer[ (args->pdm_buffer_size) / 2]);

			// 2. and process data (MONO)
			PDM_Filter(pPDMdata, args->pcm_buffer, (args->FilterHandler));
			osSemaphoreRelease(fftPcmDataReadyHandle);

			// 3. MONO -> STEREO & DAC data send (DMA in circular mode)
			for (int i = 0; i < args->pcm_buffer_size; i++) {
				stereo_tx_buffer[i * 2]     = args->pcm_buffer[i]; // Left
				stereo_tx_buffer[i * 2 + 1] = args->pcm_buffer[i]; // Right
			}

			//HAL_I2S_Transmit_DMA(CS43L22_I2S_HANDLE, (uint16_t *)stereo_tx_buffer, (args->pcm_buffer_size) * 2);

//			if(osSemaphoreAcquire(TXuartHandle, 10) == osOK)
//			{
//
//				// UART sends 8-bit chunks but my data is 16-bit chunk
//				memcpy(uart_tx_buffer, args->pcm_buffer, (args->pcm_buffer_size) * 2);
//
//				// Synchronize
//				uint16_t sync_word = 0xAAAA;
//				HAL_UART_Transmit(args->uartHandle, (uint8_t*)&sync_word, 2, HAL_MAX_DELAY);
//
//				// one-shot DMA transfer
//				HAL_UART_Transmit_DMA(args->uartHandle, (uint8_t *)uart_tx_buffer, (args->pcm_buffer_size) * 2);
//			}
		}

	}
  /* USER CODE END start_pcmData */
}

/* USER CODE BEGIN Header_start_pcmFFT */
/**
* @brief Function implementing the pcmFFT thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_start_pcmFFT */
void start_pcmFFT(void *argument)
{
  /* USER CODE BEGIN start_pcmFFT */

	// 1. FFT Init
	fft_Init();
	fftArgs_t *args = GetfftArgs();


	uint16_t fftIndex = 0;

	/* Infinite loop */
	  for(;;)
	  {
		  if(osSemaphoreAcquire(fftPcmDataReadyHandle, osWaitForever) == osOK){

			  // Filling the FFT buffer
			  for(uint8_t i=0; i < args -> pcm_buffer_size; i++){
				  args -> fftBuffIn[fftIndex] = (float)((int16_t)args -> source_pcm_data[i]);
				  fftIndex++;


				  /* When FFT buffer is full -> count FFT
				   * FFT is computed in MONO format - doesn't need to get
				   * rid of doubled samples
				   * */
				  if(fftIndex >= (args -> fft_buffer_size)){

					  // 1. Calculate fft values (input: read audio data, output: complex number)
					  arm_rfft_fast_f32(args -> fftHandler, args -> fftBuffIn, args -> fftComplex, 0);

					  // 2. Calculate Magnitude from complex samples
					  for(uint16_t i=0; i < (args -> fft_buffer_size) / 2; i++){

						  float real = args -> fftComplex[2*i] * args -> fftComplex[2 *i];
						  float imag = args -> fftComplex[2*i + 1] * args -> fftComplex[2 *i + 1];

						  args -> fftBuffOut[i] = sqrtf(real + imag);
					  }

					  if(osSemaphoreAcquire(TXuartHandle, 10) == osOK)
					  {
					      uint16_t sync_word = 0xBBBB;
					      HAL_UART_Transmit(args->uartHandle, (uint8_t*)&sync_word, 2, HAL_MAX_DELAY);

					      uint16_t bytes_to_send = ((args->fft_buffer_size) / 2) * sizeof(float);

					      HAL_UART_Transmit_DMA(args->uartHandle, (uint8_t *)(args->fftBuffOut), bytes_to_send);
					  }

					  fftIndex = 0;
				  }
			 }

		  }

	  }
  /* USER CODE END start_pcmFFT */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

