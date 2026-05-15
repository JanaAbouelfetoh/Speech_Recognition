/**
 * main.c - Fixed Speech Recognition Main
 * ========================================
 *
 * FIXES FROM OLD VERSION:
 * -----------------------
 *
 * FIX 1: Init order corrected.
 *   OLD: adc_init() ? timer1_init() ? uart_init() ? lcd_init() ? sram_init()
 *        uart_init() called 3rd, so adc_init/timer1_init printed garbage.
 *        sram_init() called last, but sram_write() was already being called.
 *   NEW: uart_init() FIRST, then sram_init(), then adc_init(), timer1_init().
 *        Timer1 interrupt is armed last so ISR never fires before SRAM ready.
 *
 * FIX 2: SRAM clear removed from main loop.
 *   OLD: 8000x sram_write(127) before recording = ~152ms of bus activity.
 *        Timer1 ISR fires during this, ISR also calls sram_write() = conflict.
 *   NEW: No pre-clear needed. adc_start_recording() resets index to 0.
 *        Every byte gets overwritten by the recording. No conflict.
 *
 * FIX 3: Flush loop has no extra delay.
 *   OLD: _delay_us(50) inside flush loop = throttled drain = ring overflow.
 *   NEW: Flush loop runs as fast as possible. sram_write() itself takes
 *        ~19us which is the only delay. Ring drains faster than ISR fills it.
 *
 * FIX 4: Timer1 only enabled when needed.
 *   OLD: timer1_init() called at startup, ISR runs forever burning cycles.
 *   NEW: Timer1 interrupt enabled just before recording, disabled after.
 *        Saves CPU during LCD/UART operations.
 *
 * FIX 5: Software gain for weak microphone signal.
 *   Your ADC reads 60-64 (range=4, centered at ~62 not 128).
 *   This means: mic output is near GND, no DC bias to mid-supply.
 *   Hardware fix: build the preamp circuit (see comment at bottom).
 *   Software workaround applied in features.c via ADC_DC_OFFSET and
 *   ADC_SOFT_GAIN in config.h. Samples are re-centered and amplified
 *   before feature extraction so the math works on real signal values.
 *
 * FIX 6: leds_init() removed.
 *   PB1-PB4 are SRAM data bus pins. Configuring them as LED outputs
 *   and driving them corrupts the SRAM data bus on every leds_show() call.
 *   LEDs are disabled until you move them to non-SRAM pins.
 */

/**
 * main.c - Fixed Speech Recognition Main
 */

#include "config.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <string.h>
#include "uart.h"
#include "sram.h"
#include "adc_sampler.h"
#include "features.h"
#include "classifier.h"
#include "lcd.h"
#include "word_templates.h"

/* uart_int() already declared in uart.h ? do NOT redeclare here */

static void print_byte(uint8_t v)
{
    uart_char('0' + v / 100);
    uart_char('0' + (v / 10) % 10);
    uart_char('0' + v % 10);
}

static void verify_sram_audio(void)
{
    uint8_t mn = 255, mx = 0;
    for (uint16_t i = 0; i < BUF_SIZE; i++)
    {
        uint8_t v = sram_read(SRAM_AUDIO_BASE + i);
        if (v < mn) mn = v;
        if (v > mx) mx = v;
    }

    uart_str("Sample range: ");
    print_byte(mn);
    uart_str(" - ");
    print_byte(mx);
    uart_str("  (range=");
    uart_int(mx - mn);
    uart_str(")\r\n");

    uart_str("First 20: ");
    for (uint8_t i = 0; i < 20; i++)
    {
        print_byte(sram_read(SRAM_AUDIO_BASE + i));
        uart_char(' ');
    }
    uart_str("\r\n");

    if (mx - mn < 10)
        uart_str("WARNING: Tiny range - mic needs DC bias or preamp.\r\n");
}

static uint8_t do_recording(void)
{
    uart_str("Recording... speak now!\r\n");
    lcd_clear();
    lcd_str("Recording...");

    adc_start_recording();

    uint16_t timeout_ms = 0;
    while (!adc_done())
    {
        adc_flush();
        _delay_ms(1);
        timeout_ms++;
        if (timeout_ms > 1500)
        {
            adc_recording = 0;
            uart_str("ERROR: Recording timeout!\r\n");
            return 0;
        }
    }
    adc_flush();   /* drain remainder */

    uart_str("Done. Samples=");
    uart_int(adc_idx);
    if (overflow_count > 0)
    {
        uart_str("  OVERFLOW=");
        uart_int(overflow_count);
    }
    uart_str("\r\n");

    return 1;
}

int main(void)
{
    /* 1. UART first so all inits can print */
    uart_init();
    uart_str("\r\n=== SPEECH RECOGNITION ===\r\n");

    /* 2. SRAM before anything uses the bus */
    sram_init();

    /* 3. ADC */
    adc_init();

    /* 4. LCD */
    lcd_init();

    /* 5. Button */
    DDRB  &= ~(1 << PB0);
    PORTB |=  (1 << PB0);

    /* 6. Enable interrupts then arm timer */
    sei();
    timer1_init();

    uart_str("Ready. Words: on off start stop left right up down\r\n");
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

            uart_str("\r\n--- RECORDING ---\r\n");

            if (!do_recording())
            {
                lcd_clear();
                lcd_str("Rec failed");
                _delay_ms(1500);
                lcd_clear();
                lcd_str("Press Button");
                continue;
            }

            verify_sram_audio();

            uart_str("Extracting features...\r\n");
            lcd_clear();
            lcd_str("Processing...");

            float feat[N_FEATURES];
            extract_features(feat);
            uart_str("Features done.\r\n");

            uart_str("Classifying...\r\n");
            uint8_t idx = classify(feat);

            char word_buf[10];
            strcpy_P(word_buf, (PGM_P)pgm_read_word(&word_labels[idx]));

            lcd_clear();
            lcd_str(word_buf);

            uart_str("DETECTED: ");
            uart_str(word_buf);
            uart_str("  (idx=");
            uart_int(idx);
            uart_str(")\r\n");

            _delay_ms(2000);
            while (!(PINB & (1 << PB0)));
            _delay_ms(200);

            lcd_clear();
            lcd_str("Press Button");
        }

        _delay_ms(10);
    }
}
///**
// * main_audio_test.c - Record audio and dump over UART for PC playback
// * =====================================================================
// * 
// * HOW IT WORKS:
// * 1. Press button ? records 8000 samples into SRAM at 8kHz
// * 2. Sends marker "AUDIO_START\r\n" over UART
// * 3. Dumps all 8000 raw bytes as binary over UART
// * 4. Sends marker "AUDIO_END\r\n"
// * 5. Python script on PC captures this, saves as WAV, plays it
// *
// * BAUD RATE NOTE:
// * 8000 bytes at 115200 baud = ~0.7 seconds to transmit.
// * This is fine ? we dump AFTER recording is complete.
// */
//
//
//#include <avr/io.h>
//#include <avr/interrupt.h>
//#include <util/delay.h>
//#include "config.h"
//#include "uart.h"
//#include "sram.h"
//#include "adc_sampler.h"
//#include "lcd.h"
//
//static void print_byte(uint8_t v)
//{
//    uart_char('0' + v / 100);
//    uart_char('0' + (v / 10) % 10);
//    uart_char('0' + v % 10);
//}
//
//static uint8_t do_recording(void)
//{
//    lcd_clear();
//    lcd_str("Recording...");
//    uart_str("Recording...\r\n");
//
//    adc_start_recording();
//
//    uint16_t timeout_ms = 0;
//    while (!adc_done())
//    {
//        adc_flush();
//        _delay_ms(1);
//        if (++timeout_ms > 1500)
//        {
//            adc_recording = 0;
//            uart_str("TIMEOUT\r\n");
//            return 0;
//        }
//    }
//    adc_flush();
//    return 1;
//}
//
//static void dump_audio_uart(void)
//{
//    /* Print stats first so you can see signal quality */
//    uint8_t mn = 255, mx = 0;
//    for (uint16_t i = 0; i < BUF_SIZE; i++)
//    {
//        uint8_t v = sram_read(SRAM_AUDIO_BASE + i);
//        if (v < mn) mn = v;
//        if (v > mx) mx = v;
//    }
//    uart_str("STATS min=");
//    print_byte(mn);
//    uart_str(" max=");
//    print_byte(mx);
//    uart_str(" range=");
//    uart_int(mx - mn);
//    uart_str("\r\n");
//
//    /* Send binary audio data between markers */
//    /* Python looks for these exact strings     */
//    uart_str("AUDIO_START\r\n");
//
//    for (uint16_t i = 0; i < BUF_SIZE; i++)
//    {
//        uint8_t sample = sram_read(SRAM_AUDIO_BASE + i);
//        uart_char(sample);   /* raw binary byte ? NOT ascii */
//    }
//
//    uart_str("AUDIO_END\r\n");
//
//    lcd_clear();
//    lcd_str("Sent!");
//    uart_str("Dump complete.\r\n");
//}
//
//int main(void)
//{
//    uart_init();
//    uart_str("\r\n=== AUDIO DUMP TEST ===\r\n");
//    uart_str("Press button to record and dump audio.\r\n");
//
//    sram_init();
//    adc_init();
//    lcd_init();
//
//    DDRB  &= ~(1 << PB0);
//    PORTB |=  (1 << PB0);
//
//    sei();
//    timer1_init();
//
//    lcd_clear();
//    lcd_str("Press Button");
//
//    while (1)
//    {
//        if (!(PINB & (1 << PB0)))
//        {
//            _delay_ms(30);
//            if (PINB & (1 << PB0)) continue;
//            while (!(PINB & (1 << PB0)));
//            _delay_ms(30);
//
//            if (do_recording())
//            {
//                lcd_clear();
//                lcd_str("Sending...");
//                dump_audio_uart();
//            }
//
//            _delay_ms(500);
//            lcd_clear();
//            lcd_str("Press Button");
//        }
//        _delay_ms(10);
//    }
//}