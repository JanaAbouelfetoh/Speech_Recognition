#include "config.h"
#include "adc_sampler.h"
#include "sram.h"
#include "uart.h"
#include <avr/io.h>
#include <avr/interrupt.h>

volatile uint16_t adc_idx = 0;
volatile uint8_t  adc_recording = 0;

void adc_init(void)
{
    /* AVcc reference, left-adjust (8-bit in ADCH), ADC0 channel */
    ADMUX = (1 << REFS0) | (1 << ADLAR) | (ADC_CHANNEL & 0x07);
    
    /* Enable ADC, prescaler /128 = 86.4kHz ADC clock */
    ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
    
    /* First dummy conversion */
    ADCSRA |= (1 << ADSC);
    while (ADCSRA & (1 << ADSC));
    
    uart_str("ADC Init OK\r\n");
}

void timer1_init(void)
{
    TCCR1A = 0;
    TCCR1B = (1 << WGM12) | (1 << CS10);
    OCR1A = TIMER1_OCR;
    TIMSK |= (1 << OCIE1A);
    
    uart_str("Timer1 Init OK\r\n");
}

/* FIXED ISR - reads ADC correctly */
ISR(TIMER1_COMPA_vect)
{
    if (!adc_recording) return;
    if (adc_idx >= BUF_SIZE) return;
    
    ADCSRA |= (1 << ADSC);
    while (ADCSRA & (1 << ADSC));
    
    uint8_t sample = ADCH;
    
    /* CRITICAL DEBUG: Print address and value for first 5 writes */
    if (adc_idx < 5) {
        uart_str("Write[");
        uart_char('0' + (adc_idx/10));
        uart_char('0' + (adc_idx%10));
        uart_str("]=");
        uart_char('0' + (sample/100));
        uart_char('0' + ((sample/10)%10));
        uart_char('0' + (sample%10));
        uart_str(" to addr ");
        uint16_t addr = SRAM_AUDIO_BASE + adc_idx;
        uart_char('0' + ((addr/1000)%10));
        uart_char('0' + ((addr/100)%10));
        uart_char('0' + ((addr/10)%10));
        uart_char('0' + (addr%10));
        uart_str("\r\n");
    }
    
    sram_write(SRAM_AUDIO_BASE + adc_idx, sample);
    
    adc_idx++;
}

void adc_start_recording(void)
{
    adc_idx = 0;
    adc_recording = 1;
    uart_str("Recording started...\r\n");
}

uint8_t adc_done(void)
{
    if (adc_idx >= BUF_SIZE) {
        adc_recording = 0;
        return 1;
    }
    return 0;
}

uint8_t adc_read_single(void)
{
    ADCSRA |= (1 << ADSC);
    while (ADCSRA & (1 << ADSC));
    return ADCH;
}