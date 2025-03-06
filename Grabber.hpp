#pragma once

#include <Arduino.h>
#include <Sparki.h>

#define GRABBER_DISTANCE (2) // cm

enum class GrabberState {
  SEARCH,
  CLOSING,
  HOLD,
  OPENING,
  COMPLETE // Unless we restart back to search
};

class Grabber
{
public:
  Grabber();
  
  void update();
  void debug();
  
  GrabberState getState();
  bool isState(GrabberState state);
  
  void reset();

protected: 
  bool setState(GrabberState state, bool reset = true);
  
private:
  GrabberState _state;
  unsigned long _timer, _millis;
  int distance;
};
