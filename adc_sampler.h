//#ifndef ADC_SAMPLER_H
//#define ADC_SAMPLER_H
//
//#include <stdint.h>
//#include "config.h"
//
//extern volatile uint16_t adc_idx;
//extern volatile uint8_t  adc_recording;
//extern volatile uint8_t  overflow_count;
//
//void    adc_init(void);
//void    timer1_init(void);
//void    adc_start_recording(void);
//uint8_t adc_done(void);
//void    adc_flush(void);
//uint8_t adc_read_single(void);
//
//#endif
#ifndef ADC_SAMPLER_H
#define ADC_SAMPLER_H

#include <stdint.h>
#include "config.h"

extern volatile uint16_t adc_idx;
extern volatile uint8_t  adc_recording;
extern volatile uint8_t  overflow_count;

void    adc_init(void);
void    timer1_init(void);
void    adc_start_recording(void);
uint8_t adc_done(void);
void    adc_flush(void);
uint8_t adc_read_single(void);

#endif