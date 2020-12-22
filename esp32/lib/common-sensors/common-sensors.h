#ifndef COMMON_SENSORS_H
#define COMMON_SENSORS_H

#include <Arduino.h>
#include <stdint.h>

enum class LIGHT_STATE:uint8_t {
  OFF      = 0,
  ON       = 1,
  UNKNOWN  = 2,
};

#define TMPR_OUT_OF_SCOPE_HIGH 900  // As floated int (Ex. 900 = 90.0)
#define TMPR_OUT_OF_SCOPE_LOW  -900 // As floated int (Ex. -900 = -90.0)
#define HUMD_OUT_OF_SCOPE_HIGH 100  // Regular int
#define HUMD_OUT_OF_SCOPE_LOW  0    // Regular int

#endif