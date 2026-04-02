#ifndef DISPLAY_H
#define DISPLAY_H

#include "stm32f10x.h"

// Initialise I2C and LCD
void display_init(void);

// Show AUTO MODE or MANUAL MODE on row 0
void display_mode(int mode);

// Show brightness percentage on row 1
void display_brightness(uint16_t pct);

// Show startup screen
void display_init_screen(void);

#endif