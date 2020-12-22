#include <EzCircularBuffer.h>
#include <Streaming.h>

const uint8_t MAX_STATS = 20;

typedef struct {
  uint16_t	onCount						= 1;	// 8 bytes
  uint8_t 	percentOn 				= 2;	// 8 bytes
  void reset() { onCount = 0; percentOn = 0; };
} time_slice_stats_t;

EzCircularBuffer<time_slice_stats_t, MAX_STATS> _stats;

void setup() {
  Serial.begin(9600); while (!Serial);

  for (int n = 0; n < 10; n++) {
    time_slice_stats_t* nextSlice = _stats.setNext();
    nextSlice->onCount = n;
    nextSlice->percentOn = n;
    printSlice(nextSlice);
  }

  Serial << endl << endl;

  Serial << F("do...") << endl;

	uint16_t n = _stats.count();
	do {
		n--;
		Serial << F("_stats->get(n)->percentOn = |") << _stats.get(n)->percentOn << F("|") << endl;
	} while(n);

  Serial << endl << endl;

  printStats();

  time_slice_stats_t* lastSlice = _stats.last();
}

void loop() {
}

void printStats() {
  for (int n = 0; n < MAX_STATS; n++) {
    if (_stats[n] != NULL) {
      printSlice(_stats[n]);
    } else {
      Serial << "ERROR: Position '" << n << "' is OUT OF SCOPE." << endl;
    }
  }

  Serial << endl << endl;

  for (int n = MAX_STATS-1; n >= 0; n--) {
    if (_stats[n] != NULL) {
      printSlice(_stats[n]);
    } else {
      Serial << "ERROR: Position '" << n << "' is OUT OF SCOPE." << endl;
    }
  }

}

void printSlice(time_slice_stats_t* slice) {
  Serial << "onCount: " << slice->onCount << " / percentOn: " << slice->percentOn << endl;
}