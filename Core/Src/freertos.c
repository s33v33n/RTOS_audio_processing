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

	char print_buf[64];

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

	//uint16_t PcmCodecBuffer[16];
	//HAL_I2S_Transmit_DMA(CS43L22_I2S_HANDLE, PcmCodecBuffer, Size)

	/* Infinite loop */

	for(;;){

		if(osSemaphoreAcquire(pdmDataReadyHandle, osWaitForever) == osOK){

			// select PDM data for processing
			pPDMdata = ( *(args->pFullPdmBuffer) == 0) ? args->pdm_buffer : &(args->pdm_buffer[ (args->pdm_buffer_size) / 2]);

			// and process data
			PDM_Filter(pPDMdata, args->pcm_buffer, (args->FilterHandler));

			// wait for previous transfer
			if(osSemaphoreAcquire(TXuartHandle, 10) == osOK)
			{
				// for printing values
				sprintf(print_buf, "Audio: %d\r\n", (int16_t)args->pcm_buffer[0]);
				HAL_UART_Transmit_DMA(args->uartHandle, (uint8_t *)print_buf, strlen(print_buf));

				// one-shot DMA transfer							// UART sends 8-bit chunks but my data is 16-bit chunk
//				HAL_UART_Transmit_DMA(args->uartHandle, (uint8_t *)(args -> pcm_buffer), (args->pcm_buffer_size) * 2);
			}
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

	/* Infinite loop */
	  for(;;)
	  {
	    osDelay(1);
	  }
  /* USER CODE END start_pcmFFT */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

