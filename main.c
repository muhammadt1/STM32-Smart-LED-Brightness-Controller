#include "stm32f10x.h"
#include "adc.h"
#include "ldr.h"
#include "pot.h"
#include "light.h"
#include "display.h"

static void delay_ms(uint32_t ms)
{
    volatile uint32_t count;
    while (ms--)
    {
        count = 4000;
        while (count > 0) { count--; }
    }
}

static void button_init(void)
{
    GPIOA->CRL &= ~(0xF << 16);
    GPIOA->CRL |=  (0x8 << 16);
    GPIOA->ODR |=  (1 << 4);
}

static int button_pressed(void)
{
    return (GPIOA->IDR & (1 << 4)) == 0;
}

int main(void)
{
    adc_init();
    light_init();
    display_init();
    button_init();
    display_init_screen();

    int mode      = 0;
    int btn_prev  = 1;
    uint16_t last_pct = 999;

    while (1)
    {
        // Button toggle
        int btn = button_pressed();
        if (btn && !btn_prev)
        {
            delay_ms(50);
            if (button_pressed())
            {
                mode = !mode;
                last_pct = 999;
                display_mode(mode);
            }
        }
        btn_prev = btn;

        // Get brightness from correct mode
        uint16_t pct;
        if (mode == 0)
            pct = auto_mode();
        else
            pct = manual_mode();

        // Set LED and update display
        light_set_brightness(pct);

        if (pct != last_pct)
        {
            display_brightness(pct);
            last_pct = pct;
        }

        delay_ms(100);
    }
}