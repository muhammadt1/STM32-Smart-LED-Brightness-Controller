#ifndef ADC_H
#define ADC_H

#include "stm32f10x.h"

// Initialise ADC1 for PA0 (LDR) and PA1 (potentiometer)
void adc_init(void);

// Read ADC value from a channel
// channel 0 = PA0 = LDR
// channel 1 = PA1 = potentiometer
// returns 0-4095
uint16_t adc_read(uint8_t channel);

#endif