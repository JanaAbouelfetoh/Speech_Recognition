//#include "config.h"
//#include "adc_sampler.h"
//#include "sram.h"
//#include <avr/io.h>
//#include <avr/interrupt.h>
//
///* ?? Ring buffer ?? */
//#define RING_SIZE  64
//#define RING_MASK  (RING_SIZE - 1)
//
//static volatile uint8_t  ring_buf[RING_SIZE];
//static volatile uint8_t  ring_head = 0;
//static volatile uint8_t  ring_tail = 0;
//static          uint16_t flush_write_idx = 0;
//
//volatile uint16_t adc_idx        = 0;
//volatile uint8_t  adc_recording  = 0;
//volatile uint8_t  overflow_count = 0;
//
///* ?? ADC init: AVcc ref, left-adjust, ch0, prescaler /64 ?? */
//void adc_init(void)
//{
//    ADMUX  = (1 << REFS0) | (1 << ADLAR) | (ADC_CHANNEL & 0x07);
//    ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1); /* /64 */
//    ADCSRA |= (1 << ADSC);
//    while (ADCSRA & (1 << ADSC));  /* dummy conversion */
//}
//
///* ?? Timer1: CTC, no prescaler, 8000Hz exactly ?? */
//void timer1_init(void)
//{
//    TCCR1A = 0;
//    TCCR1B = (1 << WGM12) | (1 << CS10);
//    OCR1A  = TIMER1_OCR;   /* 1381 */
//    TIMSK |= (1 << OCIE1A);
//}
//
///* ?? ISR: ~850 cycles, budget 1382 cycles ??
// * Only does: ADC trigger+wait (75탎) + ring write (3 cycles).
// * No UART, no SRAM, no delays. */
//ISR(TIMER1_COMPA_vect)
//{
//    if (!adc_recording) return;
//    if (adc_idx >= BUF_SIZE) { adc_recording = 0; return; }
//
//    ADCSRA |= (1 << ADSC);
//    while (ADCSRA & (1 << ADSC));
//    uint8_t sample = ADCH;
//
//    uint8_t next = (ring_head + 1) & RING_MASK;
//    if (next == ring_tail)
//    {
//        overflow_count++;   /* main loop too slow */
//    }
//    else
//    {
//        ring_buf[ring_head] = sample;
//        ring_head = next;
//    }
//    adc_idx++;
//}
//
///* ?? adc_flush(): call from main loop during recording ??
// * Drains ring buffer ? SRAM. Safe to call repeatedly. */
//void adc_flush(void)
//{
//    while (ring_tail != ring_head && flush_write_idx < BUF_SIZE)
//    {
//        uint8_t s = ring_buf[ring_tail];
//        ring_tail = (ring_tail + 1) & RING_MASK;
//        sram_write(SRAM_AUDIO_BASE + flush_write_idx, s);
//        flush_write_idx++;
//    }
//}
//
//void adc_start_recording(void)
//{
//    flush_write_idx = 0;
//    ring_head       = 0;
//    ring_tail       = 0;
//    overflow_count  = 0;
//    adc_idx         = 0;
//    adc_recording   = 1;
//}
//
//uint8_t adc_done(void)
//{
//    return (adc_idx >= BUF_SIZE) ? 1 : 0;
//}
//
//uint8_t adc_read_single(void)
//{
//    ADCSRA |= (1 << ADSC);
//    while (ADCSRA & (1 << ADSC));
//    return ADCH;
//}
/**
 * adc_sampler.c ? Fixed ADC + Timer1 for 8kHz audio recording
 * =============================================================
 *
 * FIXES FROM OLD VERSION:
 * -----------------------
 * 1. REMOVED all uart_str() calls from ISR.
 *    Old: ISR printed ~20 chars via UART = ~1.7ms per ISR call.
 *    Budget: 125탎 total. Was 13x over budget ? corrupted timing.
 *
 * 2. REMOVED sram_write() from ISR.
 *    sram_write() has _delay_us() calls and bus switching inside it.
 *    All blocking operations removed from ISR entirely.
 *    ISR now only writes one byte to a RAM ring buffer (~3 cycles).
 *    Main loop drains ring buffer to SRAM via adc_flush().
 *
 * 3. ADC prescaler /128 ? /64.
 *    /128: ADC clock=86.4kHz, conversion=150탎 > 125탎 budget. BROKEN.
 *    /64:  ADC clock=172.8kHz, conversion=75탎. Fits with room to spare.
 *
 * USAGE IN MAIN LOOP:
 *    adc_start_recording();
 *    while (!adc_done()) adc_flush();
 *    adc_flush();   // drain remainder
 *    // now SRAM[SRAM_AUDIO_BASE..+BUF_SIZE] has all samples
 */

#include "config.h"
#include "adc_sampler.h"
#include "sram.h"
#include <avr/io.h>
#include <avr/interrupt.h>

/* ?? Ring buffer ?? */
#define RING_SIZE  64
#define RING_MASK  (RING_SIZE - 1)

static volatile uint8_t  ring_buf[RING_SIZE];
static volatile uint8_t  ring_head = 0;
static volatile uint8_t  ring_tail = 0;
static          uint16_t flush_write_idx = 0;

volatile uint16_t adc_idx        = 0;
volatile uint8_t  adc_recording  = 0;
volatile uint8_t  overflow_count = 0;

/* ?? ADC init: AVcc ref, left-adjust, ch0, prescaler /64 ?? */
void adc_init(void)
{
    ADMUX  = (1 << REFS0) | (1 << ADLAR) | (ADC_CHANNEL & 0x07);
    ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1); /* /64 */
    ADCSRA |= (1 << ADSC);
    while (ADCSRA & (1 << ADSC));  /* dummy conversion */
}

/* ?? Timer1: CTC, no prescaler, 8000Hz exactly ?? */
void timer1_init(void)
{
    TCCR1A = 0;
    TCCR1B = (1 << WGM12) | (1 << CS10);
    OCR1A  = TIMER1_OCR;   /* 1381 */
    TIMSK |= (1 << OCIE1A);
}

/* ?? ISR: ~850 cycles, budget 1382 cycles ??
 * Only does: ADC trigger+wait (75탎) + ring write (3 cycles).
 * No UART, no SRAM, no delays. */
ISR(TIMER1_COMPA_vect)
{
    if (!adc_recording) return;
    if (adc_idx >= BUF_SIZE) { adc_recording = 0; return; }

    ADCSRA |= (1 << ADSC);
    while (ADCSRA & (1 << ADSC));
    uint8_t sample = ADCH;

    uint8_t next = (ring_head + 1) & RING_MASK;
    if (next == ring_tail)
    {
        overflow_count++;   /* main loop too slow */
    }
    else
    {
        ring_buf[ring_head] = sample;
        ring_head = next;
    }
    adc_idx++;
}

/* ?? adc_flush(): call from main loop during recording ??
 * Drains ring buffer ? SRAM. Safe to call repeatedly. */
void adc_flush(void)
{
    while (ring_tail != ring_head && flush_write_idx < BUF_SIZE)
    {
        uint8_t s = ring_buf[ring_tail];
        ring_tail = (ring_tail + 1) & RING_MASK;
        sram_write(SRAM_AUDIO_BASE + flush_write_idx, s);
        flush_write_idx++;
    }
}

void adc_start_recording(void)
{
    flush_write_idx = 0;
    ring_head       = 0;
    ring_tail       = 0;
    overflow_count  = 0;
    adc_idx         = 0;
    adc_recording   = 1;
}

uint8_t adc_done(void)
{
    return (adc_idx >= BUF_SIZE) ? 1 : 0;
}

uint8_t adc_read_single(void)
{
    ADCSRA |= (1 << ADSC);
    while (ADCSRA & (1 << ADSC));
    return ADCH;
}