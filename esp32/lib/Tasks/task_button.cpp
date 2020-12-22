#include "task_button.h"

namespace task_btn {

  /**
   *  Private
   */

  namespace {
    // Controller button
    #define BTN1_DEBOUNCE_TICKS 50
    #define BTN1_CLICK_TICKS    50
    #define BTN1_PRESS_TICKS    1500

    // Timer start/stop button
    #define BTN2_DEBOUNCE_TICKS 50
    #define BTN2_CLICK_TICKS    50 // 10
    #define BTN2_PRESS_TICKS    1500 // 2000

    OneButton _btn1(PIN_BTN_1, true);
    OneButton _btn2(PIN_BTN_2, true);

    bool _pressingBtn2 = false;

    onBtnActionFn _onBtnActionCb;


    /** Button One
     */
    void onReleaseBtn1() {
      _onBtnActionCb(1, BTN_EVENT::RELEASE);
    }

    void onHoldBtn1() {
      _onBtnActionCb(1, BTN_EVENT::HOLD);
    }

    void onDblClickBtn1() {
      _onBtnActionCb(1, BTN_EVENT::DBL_CLICK);
    }

    // void onDblReleaseBtn1() {
    //   _onBtnActionCb(1, BTN_EVENT::DBL_CLICK);
    // }


    /** Button Two
     */
    void onPressBtn2() {
      // We already know we're being pressed, we can ignore this now.
      if (_pressingBtn2 == true) {
        return;
      }

      _pressingBtn2 = true;

      _onBtnActionCb(2, BTN_EVENT::PRESS);
      _pressingBtn2 = true;
    }

    void onReleaseBtn2() {
      _pressingBtn2 = false;
      // if (_pressingBtn2 == true) {

      // }
      _onBtnActionCb(2, BTN_EVENT::RELEASE);

    }

  }

  /**
   *  Public
   */

  void setup(onBtnActionFn cb) {
    lg.verbose(F("task_button::setup(...)" CR));
    _onBtnActionCb = cb;

    // Button
    _btn1.attachClick(onReleaseBtn1);
    _btn1.attachDoubleClick(onDblClickBtn1);
    _btn1.attachLongPressStart(onHoldBtn1);
    _btn1.setDebounceTicks(BTN1_DEBOUNCE_TICKS);
    _btn1.setClickTicks(BTN1_CLICK_TICKS);
    _btn1.setPressTicks(BTN1_PRESS_TICKS);

    // Button Two (Big Red)
    _btn2.attachClick(onReleaseBtn2);     // Release button
    _btn2.attachPressStart(onPressBtn2);  // Press Button
    // _btn2.attachPressStart(onPressBtn2);

    /** Max for 32 bit. @todo move into a #define */
    _btn2.setClickTicks(2147483647);
    _btn2.setPressTicks(2147483647);

  }

  void loop() {
    _btn1.tick();
    _btn2.tick();
  }

}