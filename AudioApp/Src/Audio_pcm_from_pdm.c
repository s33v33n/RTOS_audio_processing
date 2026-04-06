#include "Audio_pcm_from_pdm.h"
#include "MP45DT02_microphone.h"

// Audio settings
#define AUDIO_FREQ 16000
#define IN_CHN 1
#define OUT_CHN 1

// double buffering for processing PDM data
#define PDM_BUFFER_SIZE 256

// only one buffer for PCM data
#define PCM_BUFFER_SIZE 16

uint16_t pdm_buffer[PDM_BUFFER_SIZE];
uint16_t pcm_buffer[PCM_BUFFER_SIZE];
volatile uint8_t FullPdmBuffer;

PDM_Filter_Handler_t PDM_FilterHandler;
PDM_Filter_Config_t  PDM_FilterConfig;

AudioTaskArgs_t audioArgs = {0};

extern osSemaphoreId_t pdmDataReadyHandle;
extern osSemaphoreId_t TXuartHandle;
extern UART_HandleTypeDef huart2;


void PDM_Init(uint32_t AudioFreq, uint32_t in_channels, uint32_t out_channels)
{
    // Necessary to unlock PDM library
    __HAL_RCC_CRC_CLK_ENABLE();
    CRC->CR = CRC_CR_RESET;

    // Init PDM filters
    PDM_FilterHandler.bit_order = PDM_FILTER_BIT_ORDER_LSB;
    PDM_FilterHandler.endianness = PDM_FILTER_ENDIANNESS_BE;
    PDM_FilterHandler.high_pass_tap = 2122358088;
    PDM_FilterHandler.in_ptr_channels = in_channels;
    PDM_FilterHandler.out_ptr_channels = out_channels;
    PDM_Filter_Init(&PDM_FilterHandler);

    // PDM config phase
    PDM_FilterConfig.output_samples_number = AudioFreq / 1000;
    PDM_FilterConfig.mic_gain = 24;
    PDM_FilterConfig.decimation_factor = PDM_FILTER_DEC_FACTOR_128;
    PDM_Filter_setConfig(&PDM_FilterHandler, &PDM_FilterConfig);
}


void SetAudioData(void){

	// filter initialization
	PDM_Init(AUDIO_FREQ, IN_CHN, OUT_CHN);

	// Start Microphone
	MP45DT02_Start(pdm_buffer, PDM_BUFFER_SIZE);

	// Box to pass to RTOS thread
	audioArgs.pdm_buffer_size = PDM_BUFFER_SIZE;
	audioArgs.pcm_buffer_size = PCM_BUFFER_SIZE;
	audioArgs.pdm_buffer = pdm_buffer;
	audioArgs.pcm_buffer = pcm_buffer;
	audioArgs.pFullPdmBuffer = &FullPdmBuffer;
	audioArgs.filterHandler = &PDM_FilterHandler;
	audioArgs.dataReadySem = pdmDataReadyHandle;
	audioArgs.uartSem = TXuartHandle;
	audioArgs.uartHandle = &huart2;
	audioArgs.FilterHandler = &PDM_FilterHandler;
}


AudioTaskArgs_t *GetAudioArgs(void){

	return &audioArgs;
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART2)
    {
        osSemaphoreRelease(TXuartHandle);
    }
}
