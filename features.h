//#ifndef FEATURES_H
//#define FEATURES_H
//
//#include "config.h"
//#include <stdint.h>
//
///* Extract features from audio stored in external SRAM (SRAM_AUDIO_BASE) */
//void extract_features(float feat[N_FEATURES]);
//
//#endif
#ifndef FEATURES_H
#define FEATURES_H

#include <stdint.h>
#include "config.h"

/* Signature matches features.c: float* not float[] to avoid GCC warning */
void extract_features(float *feat);

#endif /* FEATURES_H */