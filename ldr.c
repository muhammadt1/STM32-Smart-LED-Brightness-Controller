#include "ldr.h"
#include "adc.h"

//automatic mode
uint16_t auto_mode(void)
{
    uint16_t adc = adc_read(0);

    uint32_t clamped;
    if (adc <= ADC_DARK)
        clamped = 0;
    else if (adc >= ADC_LIGHT)
        clamped = ADC_LIGHT - ADC_DARK;
    else
        clamped = adc - ADC_DARK;

    uint32_t range = ADC_LIGHT - ADC_DARK;
    return (uint16_t)(((range - clamped) * 100) / range);
}