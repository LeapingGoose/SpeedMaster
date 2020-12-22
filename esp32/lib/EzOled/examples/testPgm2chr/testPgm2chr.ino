#include <EzLib.h>
#include <stdint.h>
// #include <Streaming.h>

PGM_P _text;

static char _iqgDictBuff[30];

enum class DICT {
  on = 0,
  off = 1,
  initializing = 2,
  heat_on = 3,
  heat_off = 4,
  cool_on = 5,
  cool_off = 6,
  dehumd_on = 7,
  dehumd_off = 8,
  lights_turned_on = 9,
  lights_turned_off = 10,
  prefs = 11,
  tmpr = 12,
  humd = 13,
  dehumd = 14,
  heater = 15,
  cooler = 16,
  empty = 17,
  err = 18,
  glitching = 19,
  glitching_recovered = 20,
  initialization = 21,
  complete = 22,
  humidifier = 23,
  purge = 24,
  adjust = 25,
  lights_on = 26,
  lights_off = 27,
  up = 28,
  down = 29
};

// Example of 40 chars                          			"------------------------------"
const char dictEntry_on[]										PROGMEM = "On";
const char dictEntry_off[]									PROGMEM = "Off";
const char dictEntry_initializing[]					PROGMEM = "Initializing...";
const char dictEntry_heat_on[]							PROGMEM = "Heater has turned on.";
const char dictEntry_heat_off[]							PROGMEM = "Heater has turned off.";
const char dictEntry_cool_on[]							PROGMEM = "Cooler has turned on.";
const char dictEntry_cool_off[]							PROGMEM = "Cooler has turned off.";
const char dictEntry_dehumd_on[]						PROGMEM = "Dehumidifier has turned on.";
const char dictEntry_dehumd_off[]						PROGMEM = "Dehumidifier has turned off.";
const char dictEntry_lights_turned_on[]			PROGMEM = "Lights has turned on.";
const char dictEntry_lights_turned_off[]		PROGMEM = "Lights has turned off.";
const char dictEntry_prefs[]								PROGMEM = "Preferences";
const char dictEntry_tmpr[]									PROGMEM = "Temperature";
const char dictEntry_humd[]									PROGMEM = "Humidity";
const char dictEntry_dehumd[]								PROGMEM = "Dehumidifier";
const char dictEntry_heater[]								PROGMEM = "Heater";
const char dictEntry_cooler[]								PROGMEM = "Cooler";
const char dictEntry_empty[]								PROGMEM = "";
const char dictEntry_err[]									PROGMEM = "Error.";
const char dictEntry_glitching[]						PROGMEM = "Glitching.";
const char dictEntry_glitching_recovered[]	PROGMEM = "Glitching Recovered.";
const char dictEntry_initialization[]				PROGMEM = "Initialization";
const char dictEntry_complete[]							PROGMEM = "complete";
const char dictEntry_humidifier[]						PROGMEM = "Humidifier";
const char dictEntry_purge[]								PROGMEM = "Purge";
const char dictEntry_adjust[]								PROGMEM = "Adjust";
const char dictEntry_lights_on[]						PROGMEM = "Lights On";
const char dictEntry_lights_off[]						PROGMEM = "Lights Off";
const char dictEntry_up[]										PROGMEM = "Up";
const char dictEntry_down[]									PROGMEM = "Down";

const char *const _dict[] PROGMEM = {
  dictEntry_on,
  dictEntry_off,
  dictEntry_initializing,
  dictEntry_heat_on,
  dictEntry_heat_off,
  dictEntry_cool_on,
  dictEntry_cool_off,
  dictEntry_dehumd_on,
  dictEntry_dehumd_off,
  dictEntry_lights_turned_on,
  dictEntry_lights_turned_off,
  dictEntry_prefs,
  dictEntry_tmpr,
  dictEntry_humd,
  dictEntry_dehumd,
  dictEntry_heater,
  dictEntry_cooler,
  dictEntry_empty,
  dictEntry_err,
  dictEntry_glitching,
  dictEntry_glitching_recovered,
  dictEntry_initialization,
  dictEntry_complete,
  dictEntry_humidifier,
  dictEntry_purge,
  dictEntry_adjust,
  dictEntry_lights_on,
  dictEntry_lights_off,
  dictEntry_up,
  dictEntry_down
};

PGM_P getText() {
	return _text;
}

void setText(PGM_P text) {
	_text = text;
}

PGM_P dictP(DICT textId) {
  return _dict[(int)textId];
}

void printText(PGM_P pgmText) {
  char* txt = ezlib::pgm2chr(pgmText);
  Serial.println(txt);
}

void setup() {
  Serial.begin(9600);
  // Serial << "Starting..." << endl;
  setText(dictP(DICT::complete));
  printText(_text);
}

void loop() {
}