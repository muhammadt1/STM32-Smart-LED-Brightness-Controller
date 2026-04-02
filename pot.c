#include "pot.h"
#include "adc.h"

uint16_t manual_mode(void)
{
    return (uint16_t)((adc_read(1) * 100) / 4095);
}