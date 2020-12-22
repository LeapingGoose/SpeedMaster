#ifndef EZ_TONE_H
#define EZ_TONE_H

#include <Arduino.h>
#include <stdint.h>
#include "pitches.h"

#define TONE_CHANNEL 15

void tone(uint8_t pin, unsigned int frequency, unsigned long duration = 0, uint8_t channel = TONE_CHANNEL);
void noTone(uint8_t pin, uint8_t channel = TONE_CHANNEL);

#endif