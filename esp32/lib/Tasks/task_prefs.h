#ifndef TASK_PREFS_H
#define TASK_PREFS_H

#include <stdint.h>
#include <Preferences.h>
#include <ArduinoLog.h>
// #include "task_network.h"
#include "common-network.h"

namespace task_prefs {

  /**
   *  Support
   */
  typedef struct prefs_t {
    bool     serverHide = false;
    uint8_t  pairedMac[6] = { 0, 0, 0, 0, 0, 0 };
    uint16_t lightsOnLvl = 100;                       // Level of the LDR required to turn lights on.

    /** ADD ENTRY LOCATION 1 **/

  } prefs_t;

  /**
   *  Routines
   */
  prefs_t*  save(prefs_t* prefs = nullptr);
  prefs_t*  read();
  prefs_t*  get();
  void      serialOut();
  bool      SaveImmediate();
  void      SaveImmediate(bool immediate);

  /** Preference Values **/

  bool      getServerHide();                         // Server Hide
  void      setServerHide(bool hide);
  
}

#endif