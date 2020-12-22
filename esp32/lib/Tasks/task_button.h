#ifndef task_btn_H
#define task_btn_H

#include <OneButton.h>
#include <stdint.h>
#include <ArduinoLog.h>

enum class BTN_EVENT:uint8_t {
  RELEASE    = 0,
  DBL_CLICK  = 1,
  HOLD       = 2,
  PRESS      = 3,
};

typedef void (*onBtnActionFn) (uint8_t btnId, BTN_EVENT action);

enum class BUTTON_STATUS:uint8_t {
  READY         = 1,
  UNAVAILABLE   = 2
};

namespace task_btn {

  /**
   *  Settings - Adjust as Required.
   */
  #define PIN_BTN_1 4    // Small Button
  #define PIN_BTN_2 15   // Big Button (for start/stop timer)

  /**
   *  Routines
   */
  void setup(onBtnActionFn cb);
  void loop();

}

#endif