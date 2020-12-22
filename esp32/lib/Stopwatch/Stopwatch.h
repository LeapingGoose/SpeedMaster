#ifndef _STOPWATCH_H
#define _STOPWATCH_H

#include "Arduino.h"
#include <flint.h>

class Stopwatch {
	typedef void(*StopwatchAction)();

  public:
    Stopwatch();
    void          start();
    void          stop();
    void          reset();
    unsigned long getElapsed();
    bool          isRunning();
    float         getElapsedAs100th();
    uint32_t      getFlint();
    char*         getFormatted();

  private:
    unsigned long _lastMillis;
    unsigned long _elapsed;
    unsigned long _rawElapsed;
    unsigned long _tuning = 8;
    bool          _isRunning = false;
    char          _formattedTime[10];
};

#endif
