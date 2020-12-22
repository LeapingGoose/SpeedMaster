#ifndef EzCircularBuffer_h
#define EzCircularBuffer_h

#include <stdint.h>

// @todo Currently we only use uint16_t but, for efficiency, that type should
// be settable by the caller using the template. The the current maximum number
// of elements is 65535.

// Element 0 (aka head) will always be the active item (currently being set).

template<typename T, uint16_t MAX_CAPACITY>
class EzCircularBuffer {

  public:
    // We create all of our items at initialization so we don't want
    // to "push" newly created items. Instead we'll let the caller set the value(s)
    // of the existing, next-in-line item provided as a pointer by this method.
    T* setNext() {
      // If we don't have any elements in use yet, then just
      // start with the head.
      if (_logCnt == 0) {
        _head = _tail = 0;
        _logCnt++;
      } else if (_logCnt < MAX_CAPACITY) {
        _head++;
        _logCnt++;
      } else {
        if (_head == MAX_CAPACITY - 1) {
          _tail = 1;
          _head = 0;
        } else {
          if (++_head == MAX_CAPACITY - 1) {
            _tail = 0;
          } else {
            _tail++;
          }
        }
      }
      // @todo currently assuming we have a structure or object with a reset method.
      // Because we're recycling values, we want to ensure they're clean before we use them.
      _log[_head].reset();
      return &(_log[_head]);
    }

    T* operator [](uint16_t index) {
      return get(index);
    }

    T* get(uint16_t index) {
      if (index > _logCnt-1) {
        return NULL;
      }
      int16_t valPos = _head - index;
      return &(_log[valPos < 0 ? _logCnt + valPos : valPos]);
    }

    T* first() {
      if (_logCnt == 0) return NULL;
      return &(_log[_head]);
    }

    T* last() {
      if (_logCnt == 0) return NULL;
      return &(_log[_tail]);
    }

    uint16_t count() {
      return _logCnt;
    }

    uint16_t maxCapacity() {
      return MAX_CAPACITY;
    }

    void reset() {
      _head    = 0;
      _tail    = 0;
      _logCnt  = 0;
    }

  private:
    // Create all of the log entrees up front so there are no surprises later with memory.
    T _log[MAX_CAPACITY];
    uint16_t _head    = 0;
    uint16_t _tail    = 0;
    uint16_t _logCnt  = 0;

};
#endif