#define F_CPU 11059200UL
#include <avr/io.h>
#include <util/delay.h>
#include "uart.h"

/* Helper function to print integers */
void uart_int(uint16_t n) {
    char buf[6];
    char *p = buf + 5;
    *p = '\0';
    do {
        *(--p) = '0' + (n % 10);
        n /= 10;
    } while (n > 0);
    while (*p) uart_char(*p++);
}

int main(void) {
    uart_init();
    
    /* Configure data pins as inputs with pull-ups */
    DDRB &= ~0x1E;
    DDRD &= ~((1 << PD4) | (1 << PD5));
    DDRC &= ~((1 << PC6) | (1 << PC7));
    
    /* Enable pull-ups */
    PORTB |= 0x1E;
    PORTD |= (1 << PD4) | (1 << PD5);
    PORTC |= (1 << PC6) | (1 << PC7);
    
    uart_str("\r\n=== BUS PULL-UP TEST ===\r\n");
    uart_str("Reading data pins with pull-ups enabled:\r\n");
    
    uint8_t val = (PINB & 0x1E) >> 1;
    if (PIND & (1 << PD4)) val |= 0x10;
    if (PIND & (1 << PD5)) val |= 0x20;
    if (PINC & (1 << PC6)) val |= 0x40;
    if (PINC & (1 << PC7)) val |= 0x80;
    
    uart_str("Data bus reads: 0x");
    uart_char("0123456789ABCDEF"[(val >> 4) & 0x0F]);
    uart_char("0123456789ABCDEF"[val & 0x0F]);
    uart_str("\r\n");
    
    if (val == 0xFF) {
        uart_str("All pins read HIGH - pull-ups working\r\n");
    } else {
        uart_str("Some pins stuck LOW - check wiring\r\n");
    }
    
    /* Now test if SRAM can pull pins LOW during read */
    uart_str("\r\n=== SRAM DRIVE TEST ===\r\n");
    uart_str("Writing 0x00 to address 0x1000, then reading...\r\n");
    
    /* Configure everything for write */
    DDRA = 0xFE;
    DDRD |= (1 << PD2) | (1 << PD3) | (1 << PD6) | (1 << PD7);
    DDRB |= 0x1E;
    DDRD |= (1 << PD4) | (1 << PD5);
    DDRC |= (1 << PC6) | (1 << PC7);
    
    PORTD |= (1 << PD6) | (1 << PD7);
    PORTD &= ~(1 << PD3);
    
    uint16_t addr = 0x1000;
    uint8_t write_val = 0x00;
    
    /* Write 0x00 */
    PORTA = (PORTA & ~0xFE) | ((addr & 0xFF) << 1);
    if (addr & 0x80) PORTD |= (1 << PD2); else PORTD &= ~(1 << PD2);
    _delay_us(2);
    
    PORTD |= (1 << PD3);
    _delay_us(2);
    PORTD &= ~(1 << PD3);
    _delay_us(2);
    
    PORTA = (PORTA & ~0xFE) | (((addr >> 8) & 0x7F) << 1);
    _delay_us(2);
    
    /* Set data to 0x00 */
    PORTB &= ~0x1E;
    PORTD &= ~((1 << PD4) | (1 << PD5));
    PORTC &= ~((1 << PC6) | (1 << PC7));
    _delay_us(2);
    
    PORTD &= ~(1 << PD6);
    _delay_us(5);
    PORTD |= (1 << PD6);
    _delay_us(2);
    
    uart_str("Write complete. Now reading (SRAM should drive 0x00)...\r\n");
    
    /* Read - configure data pins as inputs with pull-ups DISABLED */
    DDRB &= ~0x1E;
    DDRD &= ~((1 << PD4) | (1 << PD5));
    DDRC &= ~((1 << PC6) | (1 << PC7));
    PORTB &= ~0x1E;      /* No pull-ups */
    PORTD &= ~((1 << PD4) | (1 << PD5));
    PORTC &= ~((1 << PC6) | (1 << PC7));
    _delay_us(2);
    
    /* Address phase for read */
    PORTA = (PORTA & ~0xFE) | ((addr & 0xFF) << 1);
    if (addr & 0x80) PORTD |= (1 << PD2); else PORTD &= ~(1 << PD2);
    _delay_us(2);
    
    PORTD |= (1 << PD3);
    _delay_us(2);
    PORTD &= ~(1 << PD3);
    _delay_us(2);
    
    PORTA = (PORTA & ~0xFE) | (((addr >> 8) & 0x7F) << 1);
    _delay_us(2);
    
    /* Enable SRAM output */
    PORTD &= ~(1 << PD7);
    _delay_us(5);
    
    /* Read */
    uint8_t read_val = (PINB & 0x1E) >> 1;
    if (PIND & (1 << PD4)) read_val |= 0x10;
    if (PIND & (1 << PD5)) read_val |= 0x20;
    if (PINC & (1 << PC6)) read_val |= 0x40;
    if (PINC & (1 << PC7)) read_val |= 0x80;
    
    PORTD |= (1 << PD7);
    
    uart_str("Read value: 0x");
    uart_char("0123456789ABCDEF"[(read_val >> 4) & 0x0F]);
    uart_char("0123456789ABCDEF"[read_val & 0x0F]);
    uart_str("\r\n");
    
    if (read_val == 0x00) {
        uart_str("SUCCESS! SRAM drove pins LOW!\r\n");
    } else if (read_val == 0xFF) {
        uart_str("FAIL: SRAM not driving (pins still HIGH from pull-ups?)\r\n");
        uart_str("Check: SRAM Pin 20 (CE) must be GND!\r\n");
        uart_str("Check: SRAM Pin 22 (OE) connected to PD7\r\n");
        uart_str("Check: SRAM Pin 27 (WE) connected to PD6\r\n");
    } else {
        uart_str("FAIL: SRAM driving partial value: ");
        uart_int(read_val);
        uart_str("\r\n");
    }
    
    /* Test 2: Write pattern and read back */
    uart_str("\r\n=== PATTERN TEST ===\r\n");
    
    /* Configure for write again */
    DDRB |= 0x1E;
    DDRD |= (1 << PD4) | (1 << PD5);
    DDRC |= (1 << PC6) | (1 << PC7);
    
    uint8_t test_patterns[] = {0x55, 0xAA, 0xFF, 0x00};
    
    for (uint8_t p = 0; p < 4; p++) {
        write_val = test_patterns[p];
        
        uart_str("Writing 0x");
        uart_char("0123456789ABCDEF"[(write_val >> 4) & 0x0F]);
        uart_char("0123456789ABCDEF"[write_val & 0x0F]);
        uart_str(" to address 0x1000... ");
        
        /* Write */
        PORTA = (PORTA & ~0xFE) | ((addr & 0xFF) << 1);
        if (addr & 0x80) PORTD |= (1 << PD2); else PORTD &= ~(1 << PD2);
        _delay_us(2);
        
        PORTD |= (1 << PD3);
        _delay_us(2);
        PORTD &= ~(1 << PD3);
        _delay_us(2);
        
        PORTA = (PORTA & ~0xFE) | (((addr >> 8) & 0x7F) << 1);
        _delay_us(2);
        
        PORTB = (PORTB & ~0x1E) | ((write_val & 0x0F) << 1);
        if (write_val & 0x10) PORTD |= (1 << PD4); else PORTD &= ~(1 << PD4);
        if (write_val & 0x20) PORTD |= (1 << PD5); else PORTD &= ~(1 << PD5);
        if (write_val & 0x40) PORTC |= (1 << PC6); else PORTC &= ~(1 << PC6);
        if (write_val & 0x80) PORTC |= (1 << PC7); else PORTC &= ~(1 << PC7);
        _delay_us(2);
        
        PORTD &= ~(1 << PD6);
        _delay_us(5);
        PORTD |= (1 << PD6);
        _delay_us(2);
        
        /* Read back */
        DDRB &= ~0x1E;
        DDRD &= ~((1 << PD4) | (1 << PD5));
        DDRC &= ~((1 << PC6) | (1 << PC7));
        PORTB &= ~0x1E;
        PORTD &= ~((1 << PD4) | (1 << PD5));
        PORTC &= ~((1 << PC6) | (1 << PC7));
        _delay_us(2);
        
        PORTA = (PORTA & ~0xFE) | ((addr & 0xFF) << 1);
        if (addr & 0x80) PORTD |= (1 << PD2); else PORTD &= ~(1 << PD2);
        _delay_us(2);
        
        PORTD |= (1 << PD3);
        _delay_us(2);
        PORTD &= ~(1 << PD3);
        _delay_us(2);
        
        PORTA = (PORTA & ~0xFE) | (((addr >> 8) & 0x7F) << 1);
        _delay_us(2);
        
        PORTD &= ~(1 << PD7);
        _delay_us(5);
        
        read_val = (PINB & 0x1E) >> 1;
        if (PIND & (1 << PD4)) read_val |= 0x10;
        if (PIND & (1 << PD5)) read_val |= 0x20;
        if (PINC & (1 << PC6)) read_val |= 0x40;
        if (PINC & (1 << PC7)) read_val |= 0x80;
        
        PORTD |= (1 << PD7);
        
        DDRB |= 0x1E;
        DDRD |= (1 << PD4) | (1 << PD5);
        DDRC |= (1 << PC6) | (1 << PC7);
        
        uart_str("read 0x");
        uart_char("0123456789ABCDEF"[(read_val >> 4) & 0x0F]);
        uart_char("0123456789ABCDEF"[read_val & 0x0F]);
        
        if (read_val == write_val) {
            uart_str(" OK\r\n");
        } else {
            uart_str(" FAIL\r\n");
        }
        
        _delay_ms(100);
    }
    
    uart_str("\r\n=== TEST COMPLETE ===\r\n");
    uart_str("\r\nIf all reads returned 0x00, check:\r\n");
    uart_str("1. SRAM Pin 20 (CE) must be connected to GND\r\n");
    uart_str("2. SRAM Pin 28 (VCC) must have +5V\r\n");
    uart_str("3. SRAM Pin 14 (GND) must have 0V\r\n");
    uart_str("4. SRAM Pin 22 (OE) connected to PD7\r\n");
    uart_str("5. SRAM Pin 27 (WE) connected to PD6\r\n");
    
    while(1);
}