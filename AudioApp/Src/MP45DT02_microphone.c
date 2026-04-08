#include "AudioApp.h"

extern I2S_HandleTypeDef hi2s2;

void MP45DT02_Start(uint16_t *pBuffer, uint32_t Size)
{
    HAL_I2S_Receive_DMA(&hi2s2, pBuffer, Size);
}
