#include "adc.h"

void adc_init(void)
{
    // ADC clock must be under 14MHz
    // At 8MHz HSI, dividing by 2 gives 4MHz
    RCC->CFGR |= RCC_CFGR_ADCPRE_DIV2;

    // Enable clocks for GPIOA and ADC1
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
    RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;

    // PA0 and PA1 as analog inputs
    // Clear all bits = analog input mode (MODE=00, CNF=00)
    GPIOA->CRL &= ~(0xF << 0);   // PA0
    GPIOA->CRL &= ~(0xF << 4);   // PA1

    // Turn on the ADC
    ADC1->CR2 = 0x00000001;

    // Small delay to let ADC stabilise
    volatile uint32_t d = 10000;
    while (d--) { __NOP(); }

    // Set longest sampling time for both channels (239.5 cycles)
    // This gives stable readings for resistive sensors
    ADC1->SMPR2  =  0x00000007;   // channel 0
    ADC1->SMPR2 |= (0x7 << 3);   // channel 1

    // Reset calibration then run it
    ADC1->CR2 |= ADC_CR2_RSTCAL;
    uint32_t t = 100000;
    while ((ADC1->CR2 & ADC_CR2_RSTCAL) && t--) { __NOP(); }

    ADC1->CR2 |= ADC_CR2_CAL;
    t = 100000;
    while ((ADC1->CR2 & ADC_CR2_CAL) && t--) { __NOP(); }
}

uint16_t adc_read(uint8_t channel)
{
    // Select the channel
    ADC1->SQR3 = channel;

    // Start conversion
    ADC1->CR2 |= 0x00000001;

    // Wait for end of conversion flag
    uint32_t t = 100000;
    while (!(ADC1->SR & ADC_SR_EOC) && t--) { __NOP(); }

    // Return result (0-4095)
    return (uint16_t)ADC1->DR;
}