#ifndef CONFIG_H
#define CONFIG_H

#define F_CPU           11059200UL
#define SAMPLE_RATE     8000U
#define TIMER1_OCR      ((F_CPU / SAMPLE_RATE) - 1U)  /* 1381 */
#define BAUD_RATE       115200UL
#define UART_UBRR       ((F_CPU / (16UL * BAUD_RATE)) - 1UL)

#define ADC_CHANNEL     0
#define BUF_SIZE        8000
#define SRAM_AUDIO_LEN  BUF_SIZE

#define FEAT_FRAME_LEN  128
#define FEAT_HOP_LEN    64
#define N_FEATURES      16
#define N_WORDS         8

/* LED pins (PB1-PB4) */
#define LED_MASK        0x1E
#define LED_WRITE(pat)  (PORTB = (PORTB & ~LED_MASK) | (((pat) & 0x0F) << 1))

/* LCD pins (PC0-PC5) */
#define LCD_RS_BIT      PC0
#define LCD_E_BIT       PC1
#define LCD_D4_BIT      PC2
#define LCD_D5_BIT      PC3
#define LCD_D6_BIT      PC4
#define LCD_D7_BIT      PC5
#define LCD_PORT        PORTC
#define LCD_DDR         DDRC

/* Button (PB0) */
#define BTN_PIN         PINB
#define BTN_BIT         PB0
#define BTN_PRESSED()   (!(BTN_PIN & (1 << BTN_BIT)))

#endif