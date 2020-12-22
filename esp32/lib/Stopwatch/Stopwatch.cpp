#include "Stopwatch.h"

Stopwatch::Stopwatch() {
}

void Stopwatch::start() {
  _isRunning = true;
	_lastMillis = millis();
}

void Stopwatch::stop() {
  _isRunning = false;
	_elapsed = millis() - _lastMillis;
}

void Stopwatch::reset() {
  _isRunning = false;
	_elapsed = 0;
}

unsigned long Stopwatch::getElapsed() {
	// LEFT OFF HERE> Need to return this as a flint.
	if (_isRunning) {
		_elapsed = millis() - _lastMillis;
	}
	return _elapsed;
}

uint32_t Stopwatch::getFlint() {

// Two decimal places Example
//     float fval = 1.236;
//     cout << "fval = " << fval << "\n";
//     uint32_t flint = int(roundf(fval * 100));
//     cout << "flint = " << flint << "\n";
//     float fconverted = flint / 100.0;
//     cout << "converted = " << fconverted << "\n";

    // uint32_t flint = int(roundf(fval * 100));


}

float Stopwatch::getElapsedAs100th() {
	if (_isRunning) {
		_elapsed = millis() - _lastMillis;
	}
	return _elapsed / 1000.0;
}

bool Stopwatch::isRunning() {
  return _isRunning;
}

// char* Stopwatch::getFormatted() {
//     uint32_t val = 1010;
//     float sec = (val / 1000.0);
//     float totSec = val / 1000;
//     uint32_t min = 0; //totSec / 60;

//     char _formattedTime[9];
//     sprintf(_formattedTime, "%d:%.2f", min, sec);
//     cout << _formattedTime << "\n";



// 	uint32_t ms = roundf((getElapsed() % 1000) / 10.0);
// 	uint32_t totSec = (_elapsed - ms) / 1000;
// 	uint32_t min = totSec / 60;
// 	uint32_t sec = totSec - (min * 60);

// 	#define numberOfSeconds(_time_) (_time_ % 60)
// 	#define numberOfMinutes(_time_) ((_time_ / 60) % 60) 

// 	sprintf(_formattedTime, "%d:%d.%d", min, sec, ms);
// 	return &_formattedTime[0];
// }
char* Stopwatch::getFormatted() {
	// uint32_t ms = roundf((getElapsed() % 1000) / 10.0);
	// uint32_t totSec = (_elapsed - ms) / 1000;
	// uint32_t min = totSec / 60;
	// uint32_t sec = totSec - (min * 60);

	// uint32_t val = ;
	float sec = (getElapsed() / 1000.0);
	uint32_t secWhole = sec;
	uint32_t min = secWhole / 60;
	sec -= min * 60;

	sprintf(_formattedTime, "%03d:%05.2f", min, sec);
	// sprintf(_formattedTime, "%d:%d.%d", min, sec, ms);
	return &_formattedTime[0];
}