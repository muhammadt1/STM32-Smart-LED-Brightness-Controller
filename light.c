#include "light.h"

void light_init(void)
{
    // PA8 = TIM1_CH1, Alternate Function Push-Pull 50MHz
    // PA8 is in CRH bits [3:0]
    // CNF=10 (AF push-pull), MODE=11 (50MHz)
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
    GPIOA->CRH &= ~(0xF << 0);
    GPIOA->CRH |=  (0xB << 0);

    // TIM1 is on APB2 bus
    RCC->APB2ENR |= RCC_APB2ENR_TIM1EN;

    // 8MHz / (PSC+1) = 8MHz / 8 = 1MHz timer clock
    // 1MHz / (ARR+1) = 1MHz / 1000 = 1kHz PWM frequency
    TIM1->PSC   = 7;
    TIM1->ARR   = 999;
    TIM1->CCR1  = 0;

    // PWM Mode 1 on channel 1
    TIM1->CCMR1 |= TIM_CCMR1_OC1M_2 | TIM_CCMR1_OC1M_1;
    TIM1->CCMR1 |= TIM_CCMR1_OC1PE;

    // Enable channel 1 output
    TIM1->CCER |= TIM_CCER_CC1E;

    // TIM1 is an advanced timer - must set MOE to enable output
    TIM1->BDTR |= TIM_BDTR_MOE;

    // Enable auto-reload preload and generate update event
    TIM1->CR1 |= TIM_CR1_ARPE;
    TIM1->EGR |= TIM_EGR_UG;

    // Start the timer
    TIM1->CR1 |= TIM_CR1_CEN;
}
void light_set_brightness(uint16_t pct)
{
    if (pct > 100) pct = 100;
    uint32_t duty = ((uint32_t)pct * pct * 999) / 10000;
    TIM1->CCR1 = (uint16_t)duty;
}