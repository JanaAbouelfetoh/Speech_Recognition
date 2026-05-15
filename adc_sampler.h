#ifndef ADC_SAMPLER_H
#define ADC_SAMPLER_H

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>
#include "config.h"

/* Recording status */
extern volatile uint16_t adc_idx;
extern volatile uint8_t  adc_recording;

/* Initialize ADC (8-bit, channel 0) */
void adc_init(void);

/* Initialize Timer1 for 8kHz sampling */
void timer1_init(void);

/* Start recording (writes directly to external SRAM) */
void adc_start_recording(void);

/* Returns 1 when BUF_SIZE samples recorded */
uint8_t adc_done(void);

/* Single reading for debugging */
uint8_t adc_read_single(void);

#endif