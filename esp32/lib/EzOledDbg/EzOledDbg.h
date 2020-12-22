#ifndef EZOLEDDBG_H
#define EZOLEDDBG_H

#include <Arduino.h>
#include <stdint.h>

#include <Wire.h>
#include <SSD1306Wire.h>
#include <EzCircularBuffer.h>

#define OLED_SDA      21
#define OLED_SCL      22
#define OLED_ADDRESS  0x3c

namespace ez_oled {
  // #define LOG_TO_SERIAL true`

  #define LINE_HEIGHT 13
  #define MAX_LINES   5
  #define LOG_POS_X   66

  // Hold our rolling log text item.
  typedef struct ui_out_item_t{
    char txt[30];
    void set(char* newTxt) { strcpy(txt, newTxt); };
    void reset()           { strcpy(txt, "");     };
  } ui_out_item_t;

  enum class DBG_DIV:uint8_t {
    NONE    = 1,
    FIRST   = 2,
    SECOND  = 4,
    CURRENT = 8,
    BOTH    = 16,
    FULL    = 32
  };

  // Manage our scrolling log text.
  EzCircularBuffer<ui_out_item_t, MAX_LINES> _logBuff;

  SSD1306Wire ui(OLED_ADDRESS, OLED_SDA, OLED_SCL);

  void uiInit();
  void drawDottedHorizontalLine(int16_t x, int16_t y, int16_t length, uint8_t spacing=2);
  void drawDottedVerticalLine(int16_t x, int16_t y, int16_t length, uint8_t spacing=2);
  void clearUI(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1);
  void drawCanvas();
  void log(const char* txt);
  void showInfo(const char* txt, uint8_t lineNum, uint8_t pos = 0, DBG_DIV dbgDiv = DBG_DIV::CURRENT);
  void clearLog();

  void uiInit() {
    ui.init();
    ui.flipScreenVertically();
    ui.setTextAlignment(TEXT_ALIGN_LEFT);
    ui.setFont(ArialMT_Plain_10);
    drawCanvas();
  }

  void drawDottedVerticalLine(int16_t x, int16_t y, int16_t length, uint8_t spacing) {
    int16_t endPointY = length + y;
    for (uint16_t currY = y; currY <= endPointY; currY += spacing) {
      ui.setPixel(x, currY);
    }
  }

  void drawDottedHorizontalLine(int16_t x, int16_t y, int16_t length, uint8_t spacing) {
    for (uint16_t currX = x; currX <= length; currX += spacing) {
      ui.setPixel(currX, y);
    }
  }

  void drawCanvas() {
    ui.clear();
    // Vertical line splitting the screen.
    // drawDottedVerticalLine(64, 0, 64);
    // Horizontal lines on the left of the screen to divide the sections.
    for (uint8_t n = 0; n < MAX_LINES + 1; n++) {
      drawDottedHorizontalLine(0, n * LINE_HEIGHT, 64);
    }
  }

  void log(const char* txt) {
    _logBuff.setNext()->set(const_cast<char*>(txt));
    clearUI(65, 0, 128-65, 64);
    for (uint8_t n = 0; n < _logBuff.count(); n++) {
      ui.drawString(LOG_POS_X, (MAX_LINES - 1 - n) * LINE_HEIGHT, _logBuff.get(n)->txt);
#ifdef LOG_TO_SERIAL
      Serial.println(txt);
#endif
    }
    ui.display();
  }

  void clearLog() {
    clearUI(65, 0, 128-65, 64);
    _logBuff.reset();
  }

  void clearUI(uint8_t x0, uint8_t y0, uint8_t width, uint8_t height) {
    ui.setColor(OLEDDISPLAY_COLOR::BLACK);
    ui.fillRect(x0, y0, width, height);
    ui.setColor(OLEDDISPLAY_COLOR::WHITE);
  }

  // pos and lineNum are 1 based.
  // void showInfo(const char* txt, uint8_t lineNum, bool showInLog) {
  void showInfo(const char* txt, uint8_t lineNum, uint8_t pos, DBG_DIV dbgDiv) {
    if (pos > 2) {
      /** @todo Indicate error here */
      return;
    }
#ifdef LOG_TO_SERIAL
    Serial.printf("showInfo(\"%s\", %d) \n", txt, lineNum);
#endif
    uint8_t x1 = (pos <= 1 ? 0 : 33);
    // If no pos was given, use the whole line.
    // Otherwise, only clear half the line to show
    // the info in the pos given.
    uint8_t y1 = (lineNum - 1) * LINE_HEIGHT;

    switch (dbgDiv) {
    case DBG_DIV::FULL:
      // Draw both dividers, but only clear our space.
      clearUI(0, y1 + 1, 63, LINE_HEIGHT - 1);
      drawDottedVerticalLine(64, y1, LINE_HEIGHT);
      break;
    case DBG_DIV::BOTH:
      // Draw both dividers, but only clear our space.
      if (x1 == 0) {
        clearUI(0, y1 + 1, 31, LINE_HEIGHT - 1);
        drawDottedVerticalLine(32, y1, LINE_HEIGHT);
      } else {
        clearUI(x1 + 1, y1 + 1, 31, LINE_HEIGHT - 1);
        drawDottedVerticalLine(64, y1, LINE_HEIGHT);
      }
      break;
    case DBG_DIV::CURRENT:
      if (x1 == 0) {
        clearUI(0, y1 + 1, 31, LINE_HEIGHT - 1);
        drawDottedVerticalLine(32, y1, LINE_HEIGHT);
      } else {
        clearUI(x1 + 1, y1 + 1, 31, LINE_HEIGHT - 1);
        drawDottedVerticalLine(64, y1, LINE_HEIGHT);
      }
      break;
    case DBG_DIV::FIRST:
      clearUI(0, y1 + 1, 31, LINE_HEIGHT - 1);
      drawDottedVerticalLine(32, y1, LINE_HEIGHT);
      // drawDottedVerticalLine(32, y1, LINE_HEIGHT);
      break;
    case DBG_DIV::NONE:
      break;
    case DBG_DIV::SECOND:
      clearUI(x1 + 1, y1 + 1, 31, LINE_HEIGHT - 1);
      drawDottedVerticalLine(64, y1, LINE_HEIGHT);
      // drawDottedVerticalLine(64, y1, LINE_HEIGHT);
      break;
    }

    if (x1 == 0) {
      ui.drawString(x1, y1, txt);
    } else {
      ui.drawString(x1 + 1, y1, txt);
    }

    ui.display();
  }
}

#endif