/*

@todo
Nees to be minimalized and remove references and use of the display.
Just wanted to demonstrate a simple circular loop that reuses components.

*/

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <Wire.h>
#include <SSD1306Wire.h>
#include <Streaming.h>
#include <EzCircularBuffer.h>
#include <stdint.h>

#define LINE_HEIGHT 12
#define MAX_LINES   5
#define LINE1       0
#define LINE2       LINE1 + LINE_HEIGHT
#define LINE3       LINE2 + LINE_HEIGHT
#define LINE4       LINE3 + LINE_HEIGHT
#define LINE5       LINE4 + LINE_HEIGHT

void uiout(char* txt);

typedef struct ui_out_item_t{
  char txt[30];
  void set(char* newTxt)  { strcpy(txt, newTxt); };
  void reset()            { strcpy(txt, "");     };
} ui_out_item_t;

EzCircularBuffer<ui_out_item_t, MAX_LINES> _uiOutBuff;
SSD1306Wire ui(0x3c, D2, D1);

void setup() {
  delay(1000);
  Serial.begin(9600);
  while (!Serial);
  Serial << endl << endl << F("Initializing Display...") << endl;

  ui.init();
  ui.flipScreenVertically();
  ui.setTextAlignment(TEXT_ALIGN_LEFT);
  ui.setFont(ArialMT_Plain_10);
  delay(250);
  uiout("zero");
  Serial << endl << endl;
  delay(250);
  uiout("one");
  Serial << endl << endl;
  delay(250);
  uiout("two");
  Serial << endl << endl;
  delay(250);
  uiout("three");
  Serial << endl << endl;
  delay(250);
  uiout("four");
  Serial << endl << endl;
  delay(250);
  uiout("five");
  Serial << endl << endl;
  delay(250);
  uiout("six");
  Serial << endl << endl;
  delay(250);
  uiout("seven");
  Serial << endl << endl;
  delay(250);
  uiout("eight");
  Serial << endl << endl;
  delay(250);
  uiout("nine");
  Serial << endl << endl;
  delay(250);
  uiout("ten");
  Serial << endl << endl;
  delay(250);
}

void loop() {
}

void uiout(char* txt) {
  _uiOutBuff.setNext()->set(txt);
  ui.clear();
  for (uint8_t n = 0; n < _uiOutBuff.count(); n++) {
    ui.drawString(0, (MAX_LINES - 1 - n) * LINE_HEIGHT, _uiOutBuff.get(n)->txt);
  }
  ui.display();
}