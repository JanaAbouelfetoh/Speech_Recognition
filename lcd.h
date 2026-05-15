#ifndef LCD_H
#define LCD_H

/*
 * lcd.h — 4-bit LCD driver
 *
 * Wiring (matches your schematic):
 *   PC0 → RS
 *   PC1 → E
 *   PC2 → D4
 *   PC3 → D5
 *   PC4 → D6
 *   PC5 → D7
 *   RW  → GND  (always write)
 *   VSS → GND, VDD → 5V
 *   V0  → middle of 10k pot between VDD and GND (contrast)
 *   A   → 5V via 100Ω resistor (backlight anode)
 *   K   → GND (backlight cathode)
 */

#include <stdint.h>

void lcd_init(void);
void lcd_clear(void);
void lcd_home(void);
void lcd_goto(uint8_t row, uint8_t col);  /* row=0 or 1, col=0..15 */
void lcd_char(char c);
void lcd_str(const char *s);
void lcd_str_P(const char *s);            /* PROGMEM string */
void lcd_uint8(uint8_t v);

#include <avr/pgmspace.h>
#define lcd_P(s) lcd_str_P(PSTR(s))

#endif
