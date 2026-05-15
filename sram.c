/*
 * sram.c ? Bit-bang SRAM driver for UT62256 via 74LS373
 *
 * Data/address bus encoding on AVR ports:
 *   Bit 0 (D0 / A0)  ? PA1   (PORTA bit 1)
 *   Bit 1 (D1 / A1)  ? PA2
 *   Bit 2 (D2 / A2)  ? PA3
 *   Bit 3 (D3 / A3)  ? PA4
 *   Bit 4 (D4 / A4)  ? PA5
 *   Bit 5 (D5 / A5)  ? PA6
 *   Bit 6 (D6 / A6)  ? PA7
 *   Bit 7 (D7 / A7)  ? PD2
 *
 * So an 8-bit data/address byte maps as:
 *   PORTA[7:1] = byte[6:0]   (bits 6..0 of the byte ? PA7..PA1)
 *   PORTD[2]   = byte[7]     (bit 7 of the byte ? PD2)
 *
 * Address is 15 bits:
 *   Low  byte (A0?A7):  put on bus, pulse LE high?low to latch into 74LS373
 *   High byte (A8?A14): put on PA1?PA7 directly (A8=PA1 .. A14=PA7)
 *                       PD2 is don't-care for high byte (only 7 bits needed)
 *
 * ? PA0 is the ADC mic input ? NEVER drive it as output.
 *   DDRA is set to 0xFE (PA0 input, PA1?PA7 output).
 */

#include "sram.h"
#include <avr/io.h>
#include <util/delay.h>

/* ?? Pin aliases ???????????????????????????????????????????????????? */
#define LE_BIT   PD3    /* 74LS373 latch enable */
#define WE_BIT   PD6    /* SRAM write enable    */
#define OE_BIT   PD7    /* SRAM output enable   */

/* Control port = PORTD; data/addr bus = PORTA (PA1-7) + PORTD (PD2) */

/* Mask for PA1?PA7 (bus lines on PORTA, keeping PA0 safe) */
#define PA_BUS_MASK   0xFE    /* 0b11111110 */

/* Mask for PD2 (D7 of bus) while preserving PD3,PD6,PD7 control bits */
#define PD_DATA7_MASK 0x04    /* bit 2 = PD2 */

/* ?? Internal helpers ??????????????????????????????????????????????? */

/*
 * Drive the 8-bit bus value onto PA1-PA7 + PD2.
 * Preserves PA0 and all PD control bits (PD3, PD6, PD7, PD0, PD1).
 * byte bits [6:0] ? PA[7:1]
 * byte bit  [7]   ? PD[2]
 */
static inline void bus_drive(uint8_t byte)
{
    /* PA1-PA7 get bits 0-6 of byte shifted left by 1 */
    PORTA = (PORTA & ~PA_BUS_MASK) | ((byte << 1) & PA_BUS_MASK);
    /* PD2 gets bit 7 of byte */
    if (byte & 0x80)
        PORTD |=  (1 << PD2);
    else
        PORTD &= ~(1 << PD2);
}

/*
 * Read 8-bit bus value from PINA (PA1-PA7) + PIND (PD2).
 * Returns reconstructed byte.
 */
static inline uint8_t bus_read(void)
{
    uint8_t pa = PINA;
    uint8_t pd = PIND;
    /* PA[7:1] ? bits [6:0] of result */
    uint8_t result = (pa & PA_BUS_MASK) >> 1;
    /* PD[2] ? bit [7] of result */
    if (pd & (1 << PD2))
        result |= 0x80;
    return result;
}

/* Set bus direction: 1=output (write), 0=input (read) */
static inline void bus_output(void)
{
    DDRA  |=  PA_BUS_MASK;   /* PA1-PA7 outputs, PA0 stays input */
    DDRD  |=  (1 << PD2);
}

static inline void bus_input(void)
{
    DDRA  &= ~PA_BUS_MASK;   /* PA1-PA7 inputs */
    DDRD  &= ~(1 << PD2);
    /* Enable pull-ups to avoid floating during read */
    PORTA |=  PA_BUS_MASK;
    PORTD |=  (1 << PD2);
}

/* ?? Public API ???????????????????????????????????????????????????? */

void sram_init(void)
{
    /* Control pins as outputs */
    DDRD |= (1 << LE_BIT) | (1 << WE_BIT) | (1 << OE_BIT);

    /* Deassert all control signals (WE and OE are active-low) */
    PORTD |=  (1 << WE_BIT) | (1 << OE_BIT);  /* WE=1, OE=1 (inactive) */
    PORTD &= ~(1 << LE_BIT);                    /* LE=0 (latch closed)   */

    /* Bus starts as output */
    bus_output();
}

void sram_write(uint16_t addr, uint8_t data)
{
    /* ?? Phase 1: latch low address byte (A0?A7) ???????????????????? */
    bus_output();
    bus_drive((uint8_t)(addr & 0xFF));  /* A0-A7 on bus */

    PORTD |=  (1 << LE_BIT);   /* LE high ? latch transparent */
    __asm__ __volatile__("nop");
    PORTD &= ~(1 << LE_BIT);   /* LE low ? latch holds A0-A7  */

    /* ?? Phase 2: drive high address (A8?A14) on PA1-PA7 ??????????? */
    /* A8-A14 = bits 8-14 of addr = 7 bits, map to PA1-PA7           */
    uint8_t hi = (uint8_t)((addr >> 8) & 0x7F);   /* 7-bit high addr  */
    PORTA = (PORTA & ~PA_BUS_MASK) | ((hi << 1) & PA_BUS_MASK);

    /* ?? Phase 3: put data on bus ??????????????????????????????????? */
    bus_drive(data);    /* overwrites PA1-PA7 + PD2 with data byte     */

    /* ?? Phase 4: pulse WE ?????????????????????????????????????????? */
    PORTD &= ~(1 << WE_BIT);   /* WE low ? begin write                */
    __asm__ __volatile__("nop");
    __asm__ __volatile__("nop");
    PORTD |=  (1 << WE_BIT);   /* WE high ? latch data into SRAM      */
}

uint8_t sram_read(uint16_t addr)
{
    /* ?? Phase 1: latch low address byte ??????????????????????????? */
    bus_output();
    bus_drive((uint8_t)(addr & 0xFF));

    PORTD |=  (1 << LE_BIT);
    __asm__ __volatile__("nop");
    PORTD &= ~(1 << LE_BIT);

    /* ?? Phase 2: drive high address ??????????????????????????????? */
    uint8_t hi = (uint8_t)((addr >> 8) & 0x7F);
    PORTA = (PORTA & ~PA_BUS_MASK) | ((hi << 1) & PA_BUS_MASK);

    /* ?? Phase 3: release bus ? input for data ?????????????????????? */
    bus_input();

    /* ?? Phase 4: pulse OE and read ??????????????????????????????? */
    PORTD &= ~(1 << OE_BIT);   /* OE low ? SRAM drives bus           */
    __asm__ __volatile__("nop");
    __asm__ __volatile__("nop");
    uint8_t data = bus_read();
    PORTD |=  (1 << OE_BIT);   /* OE high ? SRAM releases bus        */

    /* Return bus to output mode */
    bus_output();
    return data;
}

void sram_write_buf(uint16_t addr, const uint8_t *src, uint16_t len)
{
    for (uint16_t i = 0; i < len; i++)
        sram_write(addr + i, src[i]);
}

void sram_read_buf(uint16_t addr, uint8_t *dst, uint16_t len)
{
    for (uint16_t i = 0; i < len; i++)
        dst[i] = sram_read(addr + i);
}
