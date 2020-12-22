#ifndef EZLIB_H
#define EZLIB_H

/**
*		Basically dumping easily reusable code here.
*		May put into a more reusable library later.
**/

#include "Arduino.h"
#include <stdint.h>

namespace ezlib {

	#define SECS_PER_MIN  (60UL)
  #define TMP_BUFF_SIZE 30

  // @todo Look at resolving this "properly".
  // The reason we have the same "map" function as the Arduino.h library is
  // because we get a conflict from the stl_map.h library that's added to the
  // project from the Email task.
  long          map(long x, long in_min, long in_max, long out_min, long out_max);
  void          strlower(char* str);
  void          strtrim(char* str);
  void          dblToInts(double val, uint8_t* whole, uint8_t* fraction, uint8_t decimals = 1);
  float         singlePrec(float val);
  char*         pgm2chr(PGM_P pgm);
  char*         tochar(uint32_t num);
  void          SerialBegin(uint32_t speed);
  unsigned long secToMs(unsigned long seconds);
  unsigned long msToSecs(unsigned long ms);
  unsigned long minToMs(unsigned long minutes);

}
#endif