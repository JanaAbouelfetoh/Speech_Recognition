/*
 * sram.h ? Manual SRAM driver for UT62256 (32 KB) via 74LS373 latch
 *
 * ???????????????????????????????????????????????????????????????????
 * ? ADDRESS BUS (15 bits):                                          ?
 * ?   Low byte  A0?A7  ? 74LS373 Q0?Q7  (latched from PA1?PA7+PD2)?
 * ?   High byte A8?A14 ? SRAM directly from PA1?PA7                ?
 * ?                                                                 ?
 * ? DATA BUS (8 bits, bidirectional):                               ?
 * ?   DQ0?DQ7 ? PA1?PA7 + PD2                                      ?
 * ?   PA1=D0, PA2=D1, PA3=D2, PA4=D3,                              ?
 * ?   PA5=D4, PA6=D5, PA7=D6, PD2=D7                               ?
 * ?                                                                 ?
 * ? CONTROL:                                                        ?
 * ?   PD3 ? 74LS373 LE  (latch enable, active HIGH)                ?
 * ?   PD6 ? SRAM WE     (write enable,  active LOW)                ?
 * ?   PD7 ? SRAM OE     (output enable, active LOW)                ?
 * ?   GND ? SRAM CE     (chip always enabled)                       ?
 * ???????????????????????????????????????????????????????????????????
 *
 * Timing (UT62256, 55 ns access time, safe at 11.0592 MHz ~90 ns cycle):
 *   One NOP ? 90 ns ? no extra delays needed.
 *
 * IMPORTANT ? PA0 is the MIC input (ADC0).
 * PA0 must NEVER be driven as a digital output.
 * All data/address operations use PA1?PA7 only.
 */

#ifndef SRAM_H
#define SRAM_H

#include <avr/io.h>
#include <stdint.h>

#define SRAM_SIZE   32768U   /* 32 KB = 0x0000?0x7FFF */

/*
 * Initialise all control pins.
 * Call once before any sram_read/sram_write.
 */
void sram_init(void);

/*
 * Write one byte to SRAM at the given 15-bit address (0x0000?0x7FFF).
 */
void sram_write(uint16_t addr, uint8_t data);

/*
 * Read one byte from SRAM at the given 15-bit address.
 */
uint8_t sram_read(uint16_t addr);

/*
 * Block write ? copies <len> bytes from AVR flash/RAM pointer <src>
 * into SRAM starting at <addr>.
 * Faster than calling sram_write() in a loop (no function call overhead).
 */
void sram_write_buf(uint16_t addr, const uint8_t *src, uint16_t len);

/*
 * Block read ? copies <len> bytes from SRAM at <addr> into <dst>.
 */
void sram_read_buf(uint16_t addr, uint8_t *dst, uint16_t len);

/* ?? Convenience: store/retrieve the ADC sample buffer ????????????? */
#define SRAM_AUDIO_BASE   0x0000U   /* 8000 bytes at start of SRAM   */

#endif /* SRAM_H */