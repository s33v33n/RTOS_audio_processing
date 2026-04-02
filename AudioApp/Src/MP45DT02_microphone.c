#include "MP45DT02_microphone.h"
#include "cmsis_os.h"

extern I2S_HandleTypeDef hi2s2;
extern osSemaphoreId_t pdmDataReadyHandle;

extern volatile uint8_t FullPdmBuffer;

void MP45DT02_Start(uint16_t *pBuffer, uint32_t Size)
{
    HAL_I2S_Receive_DMA(&hi2s2, pBuffer, Size);
}

void HAL_I2S_RxHalfCpltCallback(I2S_HandleTypeDef *hi2s)
{
    if (hi2s->Instance == SPI2)
    {
        FullPdmBuffer = 0;
        osSemaphoreRelease(pdmDataReadyHandle);
    }
}

void HAL_I2S_RxCpltCallback(I2S_HandleTypeDef *hi2s)
{
    if (hi2s->Instance == SPI2)
    {
        FullPdmBuffer = 1;
        osSemaphoreRelease(pdmDataReadyHandle);
    }
}
