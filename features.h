#ifndef FEATURES_H
#define FEATURES_H

#include "config.h"
#include <stdint.h>

/* Extract features from audio stored in external SRAM (SRAM_AUDIO_BASE) */
void extract_features(float feat[N_FEATURES]);

#endif