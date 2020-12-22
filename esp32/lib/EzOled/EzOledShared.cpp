#include "EzOledShared.h"

/** @todo move this into the setup **/
// #define OLED_SDA      21
// #define OLED_SCL      22
// #define OLED_ADDRESS  0x3c

SSD1306Wire __tft(OLED_ADDRESS, OLED_SDA, OLED_SCL);

/**************************************************************************************************
*   TSPoint Class
**/
TSPoint::TSPoint(void) {
  x = y = z = 0;
}

TSPoint::TSPoint(int16_t x0, int16_t y0, int16_t z0) {
  x = x0;
  y = y0;
  z = z0;
}

bool TSPoint::operator==(TSPoint p1) {
  return  ((p1.x == x) && (p1.y == y) && (p1.z == z));
}

bool TSPoint::operator!=(TSPoint p1) {
  return  ((p1.x != x) || (p1.y != y) || (p1.z != z));
}