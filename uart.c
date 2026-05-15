#include "config.h"
#include "uart.h"
#include <avr/io.h>

void uart_init(void)
{
    // Set baud rate
    UBRRH = (uint8_t)(UART_UBRR >> 8);
    UBRRL = (uint8_t)(UART_UBRR);

    // Enable ONLY TX first (simpler + safer)
    UCSRB = (1 << TXEN);

    // 8-bit data, 1 stop bit, no parity
    UCSRC = (1 << URSEL) | (1 << UCSZ1) | (1 << UCSZ0);
}

void uart_char(char c)
{
    while (!(UCSRA & (1 << UDRE)));  // wait until buffer empty
    UDR = c;
}

void uart_str(const char *s)
{
    while (*s)
    {
        uart_char(*s++);
    }
}
