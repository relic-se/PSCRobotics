#pragma once

#include <Arduino.h>
#include <Sparki.h>
#include "Component.hpp"

#define GRABBER_OPEN (10) // cm
#define GRABBER_DISTANCE (5) // cm

enum class GrabberState : uint8_t {
  SEARCH,
  GRAB,
  HOLD,
  RELEASE,
  COMPLETE
};

class Grabber : public Component<GrabberState>
{
public:
  Grabber();
  void setup() override;
  void reset() override;
  
  bool release(bool blocking = false);
  int getDistance();
  
  void debug() override;

protected:
  void open();
  void close();
  
  GrabberState changeState(GrabberState next, GrabberState previous);
  void updateState(GrabberState state);
  const char *getStateName(GrabberState state);
  
private:
  int _distance;
  bool _open;
};
