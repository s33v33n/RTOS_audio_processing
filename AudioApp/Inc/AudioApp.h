#ifndef AUDIO_APP_H
#define AUDIO_APP_H

/* ----------------------------------------------------
 * 1. SYSTEM INCLUDES & STANDARD LIBRARIES
 * ---------------------------------------------------- */
#include "main.h"
#include "cmsis_os.h" // FreeRTOS
#include <stdint.h>
#include <string.h>
#include <math.h>


/* ----------------------------------------------------
 * 2. MY INCLUDES
 * ---------------------------------------------------- */
#include "Audio_pcm_from_pdm.h"
#include "CS43L22_Speaker.h"
#include "MP45DT02_microphone.h"
#include "fft_setup.h"


/* ----------------------------------------------------
 * 3. MY CONFIGURATION
 * ---------------------------------------------------- */


// -- Audio settings -- 

#define AUDIO_FREQ 16000    // Hz
#define IN_CHN 1            // DAC
#define OUT_CHN 1           // DAC

// NEW
#define PDM_DECIMATION_FACTOR   64

#define PDM_BUFFER_SIZE 256     // double buffering for processing PDM data

// NEW
#define PCM_BUFFER_SIZE  (((PDM_BUFFER_SIZE / 2) * 16) / PDM_DECIMATION_FACTOR)
//#define PCM_BUFFER_SIZE 16      // only one buffer for PCM data


// -- FFT --
#define FFT_BUFFER_SIZE 1024




// -- General -- 
#define ONE_BYTE 1



#endif /*AUDIO_APP_H*/
