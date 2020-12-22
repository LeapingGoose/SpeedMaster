#include <CircularBuffer.h>
#include <Streaming.h>


const uint8_t TIME_SLICE_DUR_MINS = 60;
typedef struct {
  uint16_t	onCount						= 1;	// 8 bytes
  uint8_t 	percentOn 				= 2;	// 8 bytes
  uint8_t 	percentLongestOn	= 3;	// 8 bytes
  uint8_t 	percentLongestOff	= 4;	// 8 bytes
} time_slice_stats_t;

CircularBuffer<time_slice_stats_t, 10> _stats;

void setup() {
  Serial.begin(9600); while (!Serial);
  printSlice(&(_stats[9]));
}

void loop() {
}

void printSlice(time_slice_stats_t* slice) {
  Serial << "_stats.size()      = " << _stats.size() << endl;
  Serial << "onCount            = " << slice->onCount << endl;
  Serial << "percentOn          = " << slice->percentOn << endl;
  Serial << "percentLongestOn   = " << slice->percentLongestOn << endl;
  Serial << "percentLongestOff  = " << slice->percentLongestOff << endl;
}