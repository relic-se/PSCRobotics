#pragma once

#include <Arduino.h>
#include <Sparki.h>

#define SENSOR_THRESHOLD (700)
#define FIND_TURN_DEGREES (15) // How many degrees right and left we should check when finding the wall
#define FINISH_TIME (2000) // Length of time to continue checking for finish in ms
#define TURN_LEFT_MOVE (7) // How far we should go forward before turning left

enum class WallFinderState {
  CALIBRATE,
  MOVE,
  FIND,
  TURN_LEFT,
  TURN_RIGHT,
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
  void reset();
  
protected:
  const char *getStateName();
  const char *getBoolName(bool value);

  bool checkSensor(int value);
  void read();

  void calibrate();
  void move();
  void find();
  bool findWall(int dir = DIR_CW);
  void turn();
  void checkFinish();

  bool setState(WallFinderState state, bool reset = true);

private:
  // State
  WallFinderState _state;
  unsigned long _millis, _timer, _ticks;

  // Sensors
  int _sensor_left_outer, _sensor_left_inner;
  bool _found_left;

  int _sensor_right_outer, _sensor_right_inner;
  bool _found_right;

  int _sensor_front;
  bool _found_front;

  bool _found_wall, _found_dead_end;

};
