#pragma once

#include <Arduino.h>
#include <Sparki.h>

#define GRABBER_INITIAL_OPEN (10) // cm
#define GRABBER_DISTANCE (2) // cm
#define GRABBER_DURATION (2000) // ms

enum class GrabberState {
  SEARCH,
  GRAB,
  HOLD,
  RELEASE,
  COMPLETE
};

class Grabber
{
public:
  Grabber();
  
  void update();
  void debug();
  
  GrabberState getState();
  bool isState(GrabberState state);

  bool release(bool blocking = false);
  void reset();

protected: 
  bool setState(GrabberState state, bool reset = true);
  
private:
  GrabberState _state;
  unsigned long _timer, _millis;
  int distance;
};
