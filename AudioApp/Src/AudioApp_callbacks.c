#include "AudioApp.h"

// 1. RTOS
extern osSemaphoreId_t pdmDataReadyHandle;
extern osSemaphoreId_t TXuartHandle;
extern osMessageQueueId_t pcmTxQueueHandle;

// 2. Local variables
extern volatile uint8_t FullPdmBuffer;
extern int16_t stereo_tx_buffer[TX_BUFFER_SIZE];
static int16_t txBlock[PCM_BUFFER_SIZE];


// I2S double buffer for receiving HALF microphone data
void HAL_I2S_RxHalfCpltCallback(I2S_HandleTypeDef *hi2s)
{
    if (hi2s->Instance == SPI2)
    {
        FullPdmBuffer = 0;
        osSemaphoreRelease(pdmDataReadyHandle);
    }
}

// I2S double buffer for receiving FULL microphone data
void HAL_I2S_RxCpltCallback(I2S_HandleTypeDef *hi2s)
{
    if (hi2s->Instance == SPI2)
    {
        FullPdmBuffer = 1;
        osSemaphoreRelease(pdmDataReadyHandle);
    }
}

// Fill one half of the output buffer with the next PCM block (mono -> stereo) or silence on underrun
static void FillTxHalf(int16_t *dst)
{
    if (osMessageQueueGet(pcmTxQueueHandle, txBlock, NULL, 0) == osOK)
    {
        for (uint16_t i = 0; i < PCM_BUFFER_SIZE; i++)
        {
            dst[i * 2]     = txBlock[i]; // Left
            dst[i * 2 + 1] = txBlock[i]; // Right
        }
    }
    else
    {
        memset(dst, 0, PCM_BUFFER_SIZE * 2 * sizeof(int16_t));
    }
}


// UART2 transmission callback - transfer to PC
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART2)
    {
        osSemaphoreRelease(TXuartHandle);
    }
}

// I2S3 TX: first half played, DMA is now reading the second half -> refill the first one
void HAL_I2S_TxHalfCpltCallback(I2S_HandleTypeDef *hi2s)
{
    if (hi2s->Instance == SPI3)
    {
        FillTxHalf(&stereo_tx_buffer[0]);
    }
}

// I2S3 TX: second half played, DMA wrapped to the first half -> refill the second one
void HAL_I2S_TxCpltCallback(I2S_HandleTypeDef *hi2s)
{
    if (hi2s->Instance == SPI3)
    {
        FillTxHalf(&stereo_tx_buffer[TX_BUFFER_SIZE / 2]);
    }
}
