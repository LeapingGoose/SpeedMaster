#include "EzLib.h"

using namespace ezlib;

char tmpBuf[TMP_BUFF_SIZE] = ""; // Temp space for char work.
char tmpBuf2[TMP_BUFF_SIZE] = ""; // Temp space for char work.

long ezlib::map(long x, long in_min, long in_max, long out_min, long out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}
void ezlib::strlower(char* str) {
  int i = 0;
  while (str[i] && str[i] != 0) {
    str[i] = tolower(str[i]);
    i++;
  }
}

// @note Currently only works with spaces.
void ezlib::strtrim(char* str) {
  // number of leading spaces
  int start = 0;
  char* buffer = str;
  while (*str && *str++ == ' ') ++start;
  // move to end of string
  while (*str++);
  int end = str - buffer - 1; 
  // backup over trailing spaces	
  while (end > 0 && buffer[end - 1] == ' ') --end;
  // remove trailing spaces
  buffer[end] = 0;
  // exit if no leading spaces or string is now empty	
  if (end <= start || start == 0) return;
  str = buffer + start;
  // remove leading spaces: K&R	
  while ((*buffer++ = *str++));
}

// Break up a double into two whole numbers.
// One being the current whole and the other the fraction.
// val = 45.9 returns whole=45, fraction = 9
// @todo Find a faster cleaner way to do this.
void ezlib::dblToInts(double val, uint8_t* whole, uint8_t* fraction, uint8_t decimals) {
  double integer;
  double remainder = modf(val, &integer) * (decimals * 10);
  *fraction = round(remainder);
  *whole = round(integer);
  if (*fraction == 10) {
    *fraction = 0;
    ++*whole;
  }
}

char* ezlib::pgm2chr(PGM_P pgm) {
  return strcpy_P(tmpBuf, pgm);
}

char* ezlib::tochar(uint32_t num) {
  sprintf(tmpBuf, "%u", num);
  return tmpBuf;
}

// Been having issues with the Serial not being available right away on the
// ESP32 so we'll call this on our startups and then just adjust this code
// once we find a better way.
void ezlib::SerialBegin(uint32_t speed = 115200) {
  delay(1000);
  Serial.begin(speed);
  while (!Serial);
  delay(1000);
}

unsigned long ezlib::secToMs(unsigned long seconds) {
  return seconds * 1000UL;
}

unsigned long ezlib::msToSecs(unsigned long ms) {
  return ms / 1000UL;
}

unsigned long ezlib::minToMs(unsigned long minutes) {
  return minutes * SECS_PER_MIN * 1000UL;
}