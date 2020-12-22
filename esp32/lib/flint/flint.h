/**
 * A 'flint' stands for 'floated int' which is an integer holding a SINGLE PRECISION float.
 * Flints are useful to avoid the difficulties brought about by floats.
 * 
 * @note    *** All floating functionality is for single precision only ***
 * @example A flint of 257 represents a float of 25.7
 */

#ifndef FLINT_H
#define FLINT_H

#include "Arduino.h"
#include <stdint.h>

#define PWR10TO1 10      // 10 to the power of 1 for the decimal place.s

int16_t createFlint(int16_t intPart, int16_t decPart);
int16_t getFlintInt(int16_t flint);
int16_t getFlintDec(int16_t flint, bool positive = false);
int16_t flintAdjDec(int16_t flint, int16_t adjBy);
int16_t flintAdjInt(int16_t flint, int16_t adjBy);
void    flintToIntAndDec(int16_t flint, int16_t* intPart, int16_t* decPart);
void    floatToFlint(float val, int16_t* intPart, int16_t* decPart);
int16_t floatToFlint(float val);
float   flintToFloat(int16_t wholeInt);

#endif