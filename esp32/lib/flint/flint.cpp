#include "flint.h"

/**	Returns integer and decimal of an integer that's masquerading as a float using decimal shifting, aka a 'floated int'.
 * @param flint An integer representing a float. Ex. float of 1234.5 would be 12345 as flint (single precision only).
 * @param intPart Return value. Pointer for integer (whole) part of the represented float.
 * @param decPart Return value Pointer for decimal part of the represented float.
 * @example
 *   execute..: uint32_t v1, v2; flint32toIntAndDec(12345, &v1, &v2, 1);
 *   output...: v1 = 1234, v2 = 5
 */
void flintToIntAndDec(int16_t flint, int16_t* intPart, int16_t* decPart) {

  if (flint == 0){
    *intPart = 0;
    *decPart = 0;
    return;
  }

  if (intPart != NULL) {
    *intPart = flint / PWR10TO1;
  }

  if (decPart != NULL) {
    *decPart = flint - (*intPart * PWR10TO1);
  }

}

// Max of 4 decimal places (i.e. decPart must be smaller than 10000)
/** Create 'floated int' from an int (whole) and decimal.
 * @param intPart The integer/whole portion of the number.
 * @param decPart The decimal of the number (as n integer)
 * @returns intPart appended with the decPart.
 * @example makeFlint(25, 2); // returns 252
 */
int16_t createFlint(int16_t intPart, int16_t decPart) {
  return (intPart * PWR10TO1) + decPart;
}

/** Return the integer part of a 'floated integer'.
 * @example
 *    execute: getIntOfFlint(1234, 2);
 */
// int16_t getIntOfFlint32(int32_t flint, uint8_t decimals) {
int16_t getFlintInt(int16_t flint) {
  if (flint == 0) {
    return 0;
  }
  return flint / PWR10TO1;
}

int16_t getFlintDec(int16_t flint, bool positive) {
  if (flint == 0) {
    return 0;
  }

  int16_t result = flint - (int(flint / PWR10TO1) * PWR10TO1);

  if (positive == true && result < 0) {
    return result * -1;
  } else {
    return result;
  }
}

/** Adjust the int portion of a floated int.
 * @param flint The 'floating int' to adjust the int portion of.
 * @param adjBy The amount to adjust the integer by.
 * @note *** Works with one decimal place only ***
 */
int16_t flintAdjInt(int16_t flint, int16_t adjBy) {
  int16_t intPart, decPart;
  flintToIntAndDec(flint, &intPart, &decPart);
  intPart += adjBy;
  return createFlint(intPart, decPart);
}

/** Adjust the int portion of a floated int.
 * @param flint The 'floating int' to adjust the int portion of.
 * @param adjBy The amount to adjust the decimal by, 9 max.
 * @note *** Works with one decimal place only ***
 */
int16_t flintAdjDec(int16_t flint, int16_t adjBy) {
  if (adjBy == 0) { return flint; }

  int16_t intPart, decPart;
  flintToIntAndDec(flint, &intPart, &decPart);

  if (adjBy > 0) { // Increment

    if (decPart == 9) {
      intPart++;
      decPart = adjBy - 1;
    } else {
      decPart += adjBy;
    }

  } else {         // Decrement

    if (decPart == 0) {
      intPart--;
      decPart = 10 + adjBy;
    } else {
      decPart += adjBy;
    }

  }
  return createFlint(intPart, decPart);
}

/** Integer to float by shifting the decimal.
 * @example
 *   execute..: flintToFloat(12345);
 *   output...: 1234.5
 */
float flintToFloat(int16_t wholeInt) {
  return wholeInt / PWR10TO1;
}


// Two decimal places Example
//     float fval = 1.236;
//     cout << "fval = " << fval << "\n";
//     uint32_t flint = int(roundf(fval * 100));
//     cout << "flint = " << flint << "\n";
//     float fconverted = flint / 100.0;
//     cout << "converted = " << fconverted << "\n";


/** Converts a float to an integer by shifting the decimal.
 * @param val The float to convert.
 * @example 23.4 returns as 234
 */
int16_t floatToFlint(float val) {
  return int(roundf(val * PWR10TO1));
}

void floatToFlint(float val, int16_t* intPart, int16_t* decPart) {
  int16_t intVal = floatToFlint(val);
  *intPart = intVal / 10;
  *decPart = intVal - (*intPart * 10);
}