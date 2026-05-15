#ifndef CLASSIFIER_H
#define CLASSIFIER_H

#include <stdint.h>
#include "config.h"

uint8_t classify(const float feat[N_FEATURES]);

#endif