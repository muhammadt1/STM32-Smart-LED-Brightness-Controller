# ENEL351 Smart LED Brightness Controller

An embedded system for the STM32F103RB (ARM Cortex-M3) that controls LED brightness using two modes: automatic (ambient light sensor) and manual (potentiometer). Mode is toggled with a push button, and brightness level is shown on a 16x2 LCD display.

## Hardware

| Component | Pin |
|-----------|-----|
| LDR (light sensor) | PA0 |
| Potentiometer | PA1 |
| LED (PWM output) | PA8 |
| LCD (I2C via PCF8574) | I2C bus |
| Mode button | GPIO |

## Modes

- **Auto** - Brightness adjusts automatically based on ambient light (ADC thresholds: dark < 1500, bright > 3500)
- **Manual** - Brightness set by potentiometer, mapped to 0-100%

Press the button to toggle between modes.

## Project Structure

```
main.c          - Main loop, mode switching
adc.c/h         - ADC init and sensor reads
light.c/h       - PWM LED driver via Timer1 (1kHz)
ldr.c/h         - Auto mode logic
pot.c/h         - Manual mode logic
display.c/h     - I2C LCD driver
```

## Building

1. Open `ENELFINALPROJECT.uvprojx` in Keil uVision
2. Build the project (F7)
3. Flash to the STM32F103RB via JTAG/SWD
