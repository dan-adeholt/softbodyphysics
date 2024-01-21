#ifndef __MIN_MAX_H
#define __MIN_MAX_H

template <typename T>
T max(T a, T b) {
  return a > b ? a : b;
}

template <typename T>
T min(T a, T b) {
  return a < b ? a : b;
}

template <typename T>
T clamp(T value, T min, T max) {
  return value < min ? min : (value > max ? max : value);
}

#endif