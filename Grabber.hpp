#pragma once

#include <Arduino.h>
#include <Sparki.h>

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
  bool setState(GrabberState state);

  
private:
  GrabberState _state;
  int distance;
};
