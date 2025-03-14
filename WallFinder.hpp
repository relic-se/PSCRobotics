#pragma once

#include <Arduino.h>
#include <Sparki.h>
#include "Component.hpp"

#define SENSOR_THRESHOLD (700)
#define FIND_TURN_DEGREES (30) // How many degrees right and left we should check when finding the wall
#define TURN_LEFT_MOVE (7) // How far we should go forward before turning left

enum class WallFinderState : uint8_t {
  CALIBRATE,
  MOVE,
  FIND,
  TURN_LEFT,
  TURN_RIGHT
};

class WallFinder : public Component<WallFinderState>
{
public:
  WallFinder();
  void setup() override;
  void debug() override;

protected:
  WallFinderState changeState(WallFinderState next, WallFinderState previous);
  void updateState(WallFinderState state);
  const char *getStateName(WallFinderState state);

  bool checkSensor(int value);
  void read();

  void calibrate();
  void move();
  void find();
  bool findWall(int dir = DIR_CW);
  void turn();

private:
  // Sensors
  int _sensor_left_outer, _sensor_left_inner;
  bool _found_left;

  int _sensor_right_outer, _sensor_right_inner;
  bool _found_right;

  int _sensor_front;
  bool _found_front;

  bool _found_wall, _found_dead_end;

};
