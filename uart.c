#include "config.h"
#include "uart.h"
#include <avr/io.h>

void uart_init(void)
{
    // Set baud rate
    UBRRH = (uint8_t)(UART_UBRR >> 8);
    UBRRL = (uint8_t)(UART_UBRR);

    // Enable TX
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

// Add this function
void uart_int(uint16_t n)
{
    char buf[6];
    char *p = buf + 5;
    *p = '\0';
    do {
        *(--p) = '0' + (n % 10);
        n /= 10;
    } while (n > 0);
    while (*p) uart_char(*p++);
}