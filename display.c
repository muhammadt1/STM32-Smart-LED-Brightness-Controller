#include "display.h"

#define LCD_ADDR  0x20
#define LCD_RS    (1 << 0)
#define LCD_EN    (1 << 2)
#define LCD_BL    (1 << 3)

// ----------------------------------------------------------------
// Delay
// ----------------------------------------------------------------
static void delay_us(uint32_t us)
{
    volatile uint32_t count = us * 4;
    while (count--) { __NOP(); }
}

static void delay_ms(uint32_t ms)
{
    while (ms--) { delay_us(1000); }
}

// ----------------------------------------------------------------
// I2C
// ----------------------------------------------------------------
static void i2c_setup(void)
{
    RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;
    RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;
    RCC->APB1ENR |= RCC_APB1ENR_I2C1EN;
    delay_us(10);

    GPIOB->CRL &= ~((0xF << 24) | (0xF << 28));
    GPIOB->CRL |=  ((0xF << 24) | (0xF << 28));

    I2C1->CR1 |=  I2C_CR1_SWRST;
    delay_us(10);
    I2C1->CR1 &= ~I2C_CR1_SWRST;
    delay_us(10);

    I2C1->CR2   = 8;
    I2C1->CCR   = 40;
    I2C1->TRISE = 9;
    I2C1->CR1  |= I2C_CR1_PE;
}

static void i2c_start(void)
{
    uint32_t t = 100000;
    while ((I2C1->SR2 & I2C_SR2_BUSY) && t--) { __NOP(); }
    I2C1->CR1 |= I2C_CR1_START;
    t = 100000;
    while (!(I2C1->SR1 & I2C_SR1_SB) && t--) { __NOP(); }
    (void)I2C1->SR1;
}

static void i2c_send_address(uint8_t addr)
{
    I2C1->DR = (uint8_t)(addr << 1);
    uint32_t t = 100000;
    while (!(I2C1->SR1 & I2C_SR1_ADDR) && t--) { __NOP(); }
    (void)I2C1->SR1;
    (void)I2C1->SR2;
}

static void i2c_send_byte(uint8_t data)
{
    uint32_t t = 100000;
    while (!(I2C1->SR1 & I2C_SR1_TXE) && t--) { __NOP(); }
    I2C1->DR = data;
    t = 100000;
    while (!(I2C1->SR1 & I2C_SR1_BTF) && t--) { __NOP(); }
}

static void i2c_stop(void)
{
    I2C1->CR1 |= I2C_CR1_STOP;
    uint32_t t = 100000;
    while ((I2C1->SR2 & I2C_SR2_MSL) && t--) { __NOP(); }
    delay_us(10);
}

static void pcf8574_write(uint8_t data)
{
    i2c_start();
    i2c_send_address(LCD_ADDR);
    i2c_send_byte(data);
    i2c_stop();
}

// ----------------------------------------------------------------
// LCD low level
// ----------------------------------------------------------------
static void lcd_pulse_enable(uint8_t data)
{
    pcf8574_write(data | LCD_EN);
    delay_us(1);
    pcf8574_write(data & ~LCD_EN);
    delay_us(50);
}

static void lcd_send_nibble(uint8_t nibble, uint8_t flags)
{
    lcd_pulse_enable((nibble & 0xF0) | flags | LCD_BL);
}

static void lcd_send_byte(uint8_t byte, uint8_t flags)
{
    lcd_send_nibble(byte & 0xF0,        flags);
    lcd_send_nibble((byte << 4) & 0xF0, flags);
    delay_us(50);
}

static void lcd_cmd(uint8_t cmd) { lcd_send_byte(cmd, 0);      }
static void lcd_char(uint8_t ch) { lcd_send_byte(ch,  LCD_RS); }

static void lcd_print(const char *str)
{
    while (*str) { lcd_char((uint8_t)*str++); }
}

static void lcd_set_cursor(uint8_t col, uint8_t row)
{
    uint8_t addr = (row == 0) ? (0x00 + col) : (0x40 + col);
    lcd_cmd(0x80 | addr);
}

// ----------------------------------------------------------------
// Public display functions
// ----------------------------------------------------------------
void display_init(void)
{
    i2c_setup();
    delay_ms(50);

    lcd_send_nibble(0x30, 0); delay_ms(5);
    lcd_send_nibble(0x30, 0); delay_us(150);
    lcd_send_nibble(0x30, 0); delay_us(150);
    lcd_send_nibble(0x20, 0); delay_us(150);

    lcd_cmd(0x28); delay_us(50);
    lcd_cmd(0x08); delay_us(50);
    lcd_cmd(0x01); delay_ms(2);
    lcd_cmd(0x06); delay_us(50);
    lcd_cmd(0x0C); delay_us(50);
}

void display_mode(int mode)
{
    lcd_set_cursor(0, 0);
    if (mode == 0)
        lcd_print("AUTO MODE  ");
    else
        lcd_print("MANUAL MODE");
}

void display_brightness(uint16_t pct)
{
    lcd_set_cursor(0, 1);
    lcd_print("BRITE LVL:");

    if (pct >= 100)
        lcd_char('1');
    else
        lcd_char(' ');

    if (pct >= 10)
        lcd_char('0' + (pct / 10) % 10);
    else
        lcd_char(' ');

    lcd_char('0' + (pct % 10));
    lcd_char('%');
    lcd_char(' ');
}

void display_init_screen(void)
{
    lcd_set_cursor(0, 0);
    lcd_print("AUTO MODE  ");
    lcd_set_cursor(0, 1);
    lcd_print("BRITE LVL:  0%");
}