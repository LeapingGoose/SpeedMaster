#include <EzLib.h>
#include <stdint.h>
// #include <Streaming.h>

void setup() {
  Serial.begin(9600);
  double dbl = 0;
  uint8_t whole, remainder;

  for (int n = 0; n < 1000; n++) {
    dbl += .01;
    ezlib::dblToInts(dbl, &whole, &remainder);
    Serial << F("FINAL = |") << whole << F(".") << remainder << F("|") << endl;
  }

}

void loop() {
}