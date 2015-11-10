#pragma once

#include <cstring>
#include <stdexcept>

namespace elysian {

template <typename T, int SIZE>
class RingBuffer {
 public:
  RingBuffer() : mReadIdx(0), mWriteIdx(0) {}

  const T &front() const { return mBuffer[mReadIdx]; }

  void pop_front() { forward(1); }

  bool empty() { return mWriteIdx == mReadIdx; }

  int getAvailableSpace() {
    return (mReadIdx <= mWriteIdx) ? SIZE - mWriteIdx + mReadIdx - 1
                                   : mReadIdx - mWriteIdx - 1;
  }

  int getOccupiedSpace() { return SIZE - 1 - getAvailableSpace(); }

  void clear() {
    mReadIdx = 0;
    mWriteIdx = 0;
  }

  void forward(int amount) {
    mReadIdx = (mReadIdx + amount) % SIZE;
    validateRingBuffer();
  }

  void validateRingBuffer() {
    if (mReadIdx < 0) {
      throw std::runtime_error("Bad ring buffer read pointer.");
    }
    if (mReadIdx > SIZE - 1) {
      throw std::runtime_error("Bad ring buffer read pointer.");
    }
    if (mWriteIdx < 0) {
      throw std::runtime_error("Bad ring buffer write pointer.");
    }
    if (mWriteIdx > SIZE - 1) {
      throw std::runtime_error("Bad ring buffer write pointer.");
    }
  }

  void push_back(const T &item) { write(&item, 1); }

  int read(T *dest, const int max_length) {
    int length = peek(dest, max_length);
    forward(length);
    return length;
  }

  int peek(T *dest, const int max_length) {
    if (mReadIdx == mWriteIdx) {  // Nothing to read
      return 0;
    }
    if (mReadIdx < mWriteIdx) {  // Data is available and not fragmented
      int length = mWriteIdx - mReadIdx;
      if (length < max_length) {
        memcpy(dest, mBuffer + mReadIdx, sizeof(T) * length);
        return length;
      } else {
        memcpy(dest, mBuffer + mReadIdx, sizeof(T) * max_length);
        return max_length;
      }
    } else {  // Data is avaible but fragmented
      int length1 = SIZE - mReadIdx;
      int length2 = mWriteIdx;
      if (length1 + length2 < max_length) {
        memcpy(dest, mBuffer + mReadIdx, sizeof(T) * length1);
        memcpy(dest + length1, mBuffer, sizeof(T) * length2);
        return length1 + length2;
      } else if (length1 < max_length) {
        memcpy(dest, mBuffer + mReadIdx, sizeof(T) * length1);
        memcpy(dest + length1, mBuffer, sizeof(T) * (max_length - length1));
        return max_length;
      } else {
        memcpy(dest, mBuffer + mReadIdx, sizeof(T) * max_length);
        return max_length;
      }
    }
  }

  int write(const T *src, const int length) {
    validateRingBuffer();
    int availablespace = getAvailableSpace();
    int writelength = length < availablespace ? length : availablespace;
    if (writelength == 0) {
      return 0;  // No memory available, need to read data first
    }
    if (mWriteIdx < mReadIdx) {
      memcpy(mBuffer + mWriteIdx, src, sizeof(T) * writelength);
      mWriteIdx += writelength;
    } else {
      int length1 = SIZE - mWriteIdx;
      if (length1 < writelength) {
        memcpy(mBuffer + mWriteIdx, src, sizeof(T) * length1);
        memcpy(mBuffer, src + length1, sizeof(T) * (writelength - length1));
        mWriteIdx = writelength - length1;
      } else {
        memcpy(mBuffer + mWriteIdx, src, sizeof(T) * (writelength));
        mWriteIdx = (mWriteIdx + writelength) % SIZE;
      }
    }
    validateRingBuffer();
    return writelength;
  }

 private:
  T mBuffer[SIZE];
  int mReadIdx, mWriteIdx;
};
}
