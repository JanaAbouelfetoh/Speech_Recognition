#ifndef UART_H
#define UART_H

#include <avr/io.h>
#include <stdint.h>

void uart_init(void);
void uart_char(char c);
void uart_str(const char *s);
void uart_int(uint16_t n);

#endif