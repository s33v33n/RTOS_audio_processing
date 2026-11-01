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
#include "AudioApp.h"

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
/* Definitions for pcmTxQueue */
osMessageQueueId_t pcmTxQueueHandle;
const osMessageQueueAttr_t pcmTxQueue_attributes = {
  .name = "pcmTxQueue"
};
/* Definitions for pcmFftQueue */
osMessageQueueId_t pcmFftQueueHandle;
const osMessageQueueAttr_t pcmFftQueue_attributes = {
  .name = "pcmFftQueue"
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
  pdmDataReadyHandle = osSemaphoreNew(1, 0, &pdmDataReady_attributes);

  /* creation of TXuart */
  TXuartHandle = osSemaphoreNew(1, 1, &TXuart_attributes);

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* Create the queue(s) */
  /* creation of pcmTxQueue */
  pcmTxQueueHandle = osMessageQueueNew (3, 256, &pcmTxQueue_attributes);

  /* creation of pcmFftQueue */
  pcmFftQueueHandle = osMessageQueueNew (4, 256, &pcmFftQueue_attributes);

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
	AudioTask_Run();
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
	FftTask_Run();
  /* USER CODE END start_pcmFFT */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

