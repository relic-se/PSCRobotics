#pragma once

#include <Arduino.h>
#include <Sparki.h>

#define SENSOR_THRESHOLD 700
#define FIND_TURN_COUNT 5
#define FINISH_TIME 2000

enum class WallFinderState {
  CALIBRATE,
  MOVE,
  FIND,
  CHECK_FINISH,
  FINISH
};

class WallFinder
{
public:
  WallFinder();
  
  void update();
  void debug();
  
  WallFinderState getState();
  bool isState(WallFinderState state);
  
protected:
  void reset();

  const char *getStateName();
  const char *getBoolName(bool value);

  bool checkSensor(int value);
  void read();

  bool calibrate(bool blocking = false);
  WallFinderState move();
  void find();
  bool findWall(int dir = DIR_CW);
  void checkFinish();

  bool setState(WallFinderState state);

private:
  // State
  WallFinderState _state;
  int _timer;

  // Sensors
  int _sensor_left_outer, _sensor_left_inner;
  bool _found_left;

  int _sensor_right_outer, _sensor_right_inner;
  bool _found_right;

  int _sensor_front;
  bool _found_front;

  bool _found_wall, _found_dead_end;

  // Calibration
  bool _calibrate_left, _calibrate_right;
  bool _calibrated;

};
