#ifndef UART_H
#define UART_H

/*
 * uart.h — UART driver for ATmega32A
 * PD1 (TXD) → USB-to-TTL RX
 * PD0 (RXD) → USB-to-TTL TX
 * Baud: 9600 @ 11.0592 MHz  →  UBRR = 71  (0% error, exact)
 */

#include <stdint.h>

void uart_init(void);
void uart_char(char c);
void uart_str(const char *s);
void uart_str_P(const char *s);   /* string in PROGMEM */
void uart_uint8(uint8_t v);
void uart_float(float v, uint8_t dec);
void uart_nl(void);               /* send \r\n */

/* Convenience macro for PROGMEM strings */
#include <avr/pgmspace.h>
#define uart_P(s)  uart_str_P(PSTR(s))

#endif
