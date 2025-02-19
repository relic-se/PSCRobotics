#pragma once

#define SERVO_MIN -80
#define SERVO_MAX 80
#define SERVO_RANGE (SERVO_MAX - SERVO_MIN)
#define SERVO_TIME 1000 // ms

#define PING_COUNT 3 // number of readings to average
#define PING_DELAY 20

#include <Arduino.h>

class Scanner
{
public:
  Scanner(size_t resolution);
  ~Scanner();

  void update();
  int get(size_t index);
  size_t length();
  int getMax();
  int getMin();
  int getMaxIndex();
  int getMinIndex();
  int getMid();

protected:
  void clear();
  void servo(int angle);
  int ping();

private:
  size_t _size;
  int *_data;
  int _lastAngle;

};
