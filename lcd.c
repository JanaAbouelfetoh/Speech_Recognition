/*
 * lcd.c — 4-bit LCD driver (HD44780 compatible)
 * PC0=RS, PC1=E, PC2=D4, PC3=D5, PC4=D6, PC5=D7
 */

#include "config.h"
#include "lcd.h"
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <util/delay.h>

/* ── Low-level pulse & nibble ─────────────────────────────────────── */
static void lcd_pulse_e(void)
{
    LCD_PORT |=  (1 << LCD_E_BIT);
    _delay_us(1);
    LCD_PORT &= ~(1 << LCD_E_BIT);
    _delay_us(50);
}

/* Send one nibble on D4-D7 */
static void lcd_nibble(uint8_t nib)
{
    /* Clear D4-D7 bits first */
    LCD_PORT &= ~((1<<LCD_D4_BIT)|(1<<LCD_D5_BIT)|(1<<LCD_D6_BIT)|(1<<LCD_D7_BIT));
    if (nib & 0x01) LCD_PORT |= (1 << LCD_D4_BIT);
    if (nib & 0x02) LCD_PORT |= (1 << LCD_D5_BIT);
    if (nib & 0x04) LCD_PORT |= (1 << LCD_D6_BIT);
    if (nib & 0x08) LCD_PORT |= (1 << LCD_D7_BIT);
    lcd_pulse_e();
}

/* Send a full byte (high nibble first) */
static void lcd_byte(uint8_t rs, uint8_t data)
{
    if (rs) LCD_PORT |=  (1 << LCD_RS_BIT);
    else    LCD_PORT &= ~(1 << LCD_RS_BIT);
    lcd_nibble(data >> 4);
    lcd_nibble(data & 0x0F);
}

static void lcd_cmd(uint8_t cmd)  { lcd_byte(0, cmd);  _delay_us(40);  }
static void lcd_data(uint8_t d)   { lcd_byte(1, d);    _delay_us(40);  }

/* ── Public API ──────────────────────────────────────────────────── */
void lcd_init(void)
{
    LCD_DDR |= (1<<LCD_RS_BIT)|(1<<LCD_E_BIT)|
               (1<<LCD_D4_BIT)|(1<<LCD_D5_BIT)|(1<<LCD_D6_BIT)|(1<<LCD_D7_BIT);
    LCD_PORT &= ~((1<<LCD_RS_BIT)|(1<<LCD_E_BIT)|
                  (1<<LCD_D4_BIT)|(1<<LCD_D5_BIT)|(1<<LCD_D6_BIT)|(1<<LCD_D7_BIT));

    _delay_ms(50);             /* wait for LCD power-on              */

    /* 4-bit init sequence (3 × function set in 8-bit mode first)    */
    LCD_PORT &= ~(1 << LCD_RS_BIT);
    lcd_nibble(0x03); _delay_ms(5);
    lcd_nibble(0x03); _delay_us(150);
    lcd_nibble(0x03); _delay_us(150);
    lcd_nibble(0x02);          /* switch to 4-bit mode               */

    lcd_cmd(0x28);             /* 4-bit, 2 lines, 5×8 dots           */
    lcd_cmd(0x0C);             /* display ON, cursor OFF, blink OFF  */
    lcd_cmd(0x06);             /* entry: increment cursor, no shift  */
    lcd_cmd(0x01);             /* clear display                      */
    _delay_ms(2);
}

void lcd_clear(void) { lcd_cmd(0x01); _delay_ms(2); }
void lcd_home(void)  { lcd_cmd(0x02); _delay_ms(2); }

void lcd_goto(uint8_t row, uint8_t col)
{
    uint8_t addr = (row == 0) ? col : (0x40 + col);
    lcd_cmd(0x80 | addr);
}

void lcd_char(char c)    { lcd_data(c); }

void lcd_str(const char *s)
{
    while (*s) lcd_data(*s++);
}

void lcd_str_P(const char *s)
{
    char c;
    while ((c = pgm_read_byte(s++))) lcd_data(c);
}

void lcd_uint8(uint8_t v)
{
    if (v >= 100) lcd_char('0' + v / 100);
    if (v >= 10)  lcd_char('0' + (v / 10) % 10);
    lcd_char('0' + v % 10);
}

