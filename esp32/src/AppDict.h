#ifndef APP_DICT_H
#define APP_DICT_H

#include <Arduino.h>
#include <pgmspace.h>

namespace app_dict {

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
    down = 29,
    memory = 30,
    debug = 31,
    apIp = 32,                  // NO LONG USED
    stationIp = 33,             // NO LONGER USED
    macAddr = 34,               // MAC Address
    sensor = 35,
    sent = 36,
    email = 37,
    failed = 38,
  };

  // Example of 40 chars                        "----------------------------------------"
  const char dictEntry_on[]                  = "On";
  const char dictEntry_off[]                 = "Off";
  const char dictEntry_initializing[]        = "Initializing...";
  const char dictEntry_heat_on[]             = "Heater has turned on.";
  const char dictEntry_heat_off[]            = "Heater has turned off.";
  const char dictEntry_cool_on[]             = "Cooler has turned on.";
  const char dictEntry_cool_off[]            = "Cooler has turned off.";
  const char dictEntry_dehumd_on[]           = "Dehumidifier has turned on.";
  const char dictEntry_dehumd_off[]          = "Dehumidifier has turned off.";
  const char dictEntry_lights_turned_on[]    = "Lights has turned on.";
  const char dictEntry_lights_turned_off[]   = "Lights has turned off.";
  const char dictEntry_prefs[]               = "Preferences";
  const char dictEntry_tmpr[]                = "Temperature";
  const char dictEntry_humd[]                = "Humidity";
  const char dictEntry_dehumd[]              = "Dehumidifier";
  const char dictEntry_heater[]              = "Heater";
  const char dictEntry_cooler[]              = "Cooler";
  const char dictEntry_empty[]               = "";
  const char dictEntry_err[]                 = "Error.";
  const char dictEntry_glitching[]           = "Glitching.";
  const char dictEntry_glitching_recovered[] = "Glitching Recovered.";
  const char dictEntry_initialization[]      = "Initialization";
  const char dictEntry_complete[]            = "complete";
  const char dictEntry_humidifier[]          = "Humidifier";
  const char dictEntry_purge[]               = "Purge";
  const char dictEntry_adjust[]              = "Adjust";
  const char dictEntry_lights_on[]           = "Lights On";
  const char dictEntry_lights_off[]          = "Lights Off";
  const char dictEntry_up[]                  = "Up";
  const char dictEntry_down[]                = "Down";
  const char dictEntry_memory[]              = "Memory";
  const char dictEntry_debug[]               = "Debug";
  const char dictEntry_apIp[]                = "AP IP";
  const char dictEntry_stationIp[]           = "Stn IP";
  const char dictEntry_macAddr[]             = "MAC";
  const char dictEntry_sensor[]              = "Sensor";
  const char dictEntry_sent[]                = "Sent";
  const char dictEntry_email[]               = "EMail";
  const char dictEntry_failed[]              = "Failed";

  const char *const _dict[] = {
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
    dictEntry_down,
    dictEntry_memory,
    dictEntry_debug,
    dictEntry_apIp,
    dictEntry_stationIp,
    dictEntry_macAddr,
    dictEntry_sensor,
    dictEntry_sent,
    dictEntry_email,
    dictEntry_failed,
  };

  const char* dict(DICT textId) {
    return _dict[(int)textId]; 
  }

  const char* dict(DICT textId, char* txt) {
    return _dict[(int)textId];
  }
}

#endif