#include "AudioApp.h"

// 1. RTOS
extern osSemaphoreId_t pdmDataReadyHandle;
extern osSemaphoreId_t TXuartHandle;

// 2. Local variables
extern volatile uint8_t FullPdmBuffer;


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


// UART2 transmission callback - transfer to PC
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART2)
    {
        osSemaphoreRelease(TXuartHandle);
    }
}
