#ifndef LIGHT_H
#define LIGHT_H

#include "stm32f10x.h"

void light_init(void);
void light_set_brightness(uint16_t pct);

#endif