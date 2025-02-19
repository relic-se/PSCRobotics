#include "Scanner.hpp"
#include <Sparki.h>

Scanner::Scanner(size_t resolution) {
  _size = resolution;
  _data = (int *)malloc(sizeof(int) * _size);
  clear();

  sparki.servo(SERVO_MIN);
  delay(SERVO_TIME);
  _lastAngle = SERVO_MIN;
};

Scanner::~Scanner() {
  free(_data);
};

void Scanner::clear() {
  for (size_t i = 0; i < _size; i++) {
    _data[i] = -1;
  }
};

void Scanner::update() {
  bool direction = _lastAngle < 0; // direction == true, left to right; direction == false, right to left
  for (size_t i = 0; i < _size; i++) {
    if (direction) {
      servo(SERVO_MIN + SERVO_RANGE * i / (_size - 1));
      _data[i] = ping();
    } else {
      servo(SERVO_MAX - SERVO_RANGE * i / (_size - 1));
      _data[_size - i - 1] = ping();
    }
  }
};

int Scanner::get(size_t index) {
  if (index >= _size) return -1;
  return _data[index];
};

size_t Scanner::length() {
  return _size;
};

int Scanner::getMax() {
  int value = -1;
  for (size_t i = 0; i < _size; i++) {
    if (value < _data[i]) value = _data[i];
  }
  return value;
};

int Scanner::getMin() {
  int value = 32767;
  for (size_t i = 0; i < _size; i++) {
    if (value > _data[i]) value = _data[i];
  }
  return value;
};

int Scanner::getMid() {
  int minValue = getMin(), maxValue = getMax();
  if (minValue < 0 && maxValue < 0) return -1;
  return (maxValue + minValue) >> 1;
};

int Scanner::getMaxIndex() {
  int value = -1, index = 0;
  for (size_t i = 0; i < _size; i++) {
    if (value >= _data[i]) continue;
    value = _data[i];
    index = i;
  }
  return value;
};

int Scanner::getMinIndex() {
  int value = -1, index = 0;
  for (size_t i = 0; i < _size; i++) {
    if (value <= _data[i]) continue;
    value = _data[i];
    index = i;
  }
  return value;
};

void Scanner::servo(int angle) {
  angle = min(max(angle, SERVO_MIN), SERVO_MAX);
  if (angle == _lastAngle) return;
  sparki.servo(angle);
  delay(abs(angle - _lastAngle) * SERVO_TIME / SERVO_RANGE);
  _lastAngle = angle;
};

int Scanner::ping() {
  int result = 0;
  for (size_t i = 0; i < PING_COUNT; i++) {
    result += sparki.ping();
    if (i < PING_COUNT - 1) {
      delay(PING_DELAY);
    }
  }
  return result / PING_COUNT;
};
