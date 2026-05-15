/**
 * main_audio_test.c - Record audio and dump over UART for PC playback
 * =====================================================================
 * 
 * HOW IT WORKS:
 * 1. Press button ? records 8000 samples into SRAM at 8kHz
 * 2. Sends marker "AUDIO_START\r\n" over UART
 * 3. Dumps all 8000 raw bytes as binary over UART
 * 4. Sends marker "AUDIO_END\r\n"
 * 5. Python script on PC captures this, saves as WAV, plays it
 *
 * BAUD RATE NOTE:
 * 8000 bytes at 115200 baud = ~0.7 seconds to transmit.
 * This is fine ? we dump AFTER recording is complete.
 */


#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "config.h"
#include "uart.h"
#include "sram.h"
#include "adc_sampler.h"
#include "lcd.h"

static void print_byte(uint8_t v)
{
    uart_char('0' + v / 100);
    uart_char('0' + (v / 10) % 10);
    uart_char('0' + v % 10);
}

static uint8_t do_recording(void)
{
    lcd_clear();
    lcd_str("Recording...");
    uart_str("Recording...\r\n");

    adc_start_recording();

    uint16_t timeout_ms = 0;
    while (!adc_done())
    {
        adc_flush();
        _delay_ms(1);
        if (++timeout_ms > 1500)
        {
            adc_recording = 0;
            uart_str("TIMEOUT\r\n");
            return 0;
        }
    }
    adc_flush();
    return 1;
}

static void dump_audio_uart(void)
{
    /* Print stats first so you can see signal quality */
    uint8_t mn = 255, mx = 0;
    for (uint16_t i = 0; i < BUF_SIZE; i++)
    {
        uint8_t v = sram_read(SRAM_AUDIO_BASE + i);
        if (v < mn) mn = v;
        if (v > mx) mx = v;
    }
    uart_str("STATS min=");
    print_byte(mn);
    uart_str(" max=");
    print_byte(mx);
    uart_str(" range=");
    uart_int(mx - mn);
    uart_str("\r\n");

    /* Send binary audio data between markers */
    /* Python looks for these exact strings     */
    uart_str("AUDIO_START\r\n");

    for (uint16_t i = 0; i < BUF_SIZE; i++)
    {
        uint8_t sample = sram_read(SRAM_AUDIO_BASE + i);
        uart_char(sample);   /* raw binary byte ? NOT ascii */
    }

    uart_str("AUDIO_END\r\n");

    lcd_clear();
    lcd_str("Sent!");
    uart_str("Dump complete.\r\n");
}

int main(void)
{
    uart_init();
    uart_str("\r\n=== AUDIO DUMP TEST ===\r\n");
    uart_str("Press button to record and dump audio.\r\n");

    sram_init();
    adc_init();
    lcd_init();

    DDRB  &= ~(1 << PB0);
    PORTB |=  (1 << PB0);

    sei();
    timer1_init();

    lcd_clear();
    lcd_str("Press Button");

    while (1)
    {
        if (!(PINB & (1 << PB0)))
        {
            _delay_ms(30);
            if (PINB & (1 << PB0)) continue;
            while (!(PINB & (1 << PB0)));
            _delay_ms(30);

            if (do_recording())
            {
                lcd_clear();
                lcd_str("Sending...");
                dump_audio_uart();
            }

            _delay_ms(500);
            lcd_clear();
            lcd_str("Press Button");
        }
        _delay_ms(10);
    }
}
