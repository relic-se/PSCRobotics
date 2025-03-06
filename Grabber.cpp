
#include "Grabber.hpp"

Grabber::Grabber() {
  sparki.gripperOpen();
  sparki.servo(SERVO_CENTER);
  reset();
};

void Grabber::reset() {
  setState(GrabberState::SEARCH);
};

void Grabber::update() {
  // Update timer
  unsigned long current = millis();
  _timer += current - _millis;
  _millis = current;

  // Read ultrasonic sensor
  distance = sparki.ping();

  switch (state) {

    case GrabberState::SEARCH:
      // NOTE: If ping is invalid, distance is -1
      if (distance >= 0 && distance <= GRABBER_DISTANCE) {
        setState(GrabberState::CLOSING);
      }
      break;

  }
};

void Grabber::debug() {
  sparki.print("Dist: ");
  sparki.print(distance);
  sparki.println("cm");
};

bool Grabber::setState(GrabberState state, bool reset) {
  if (isState(state)) return false;

  switch (state) {

    case GrabberState::CLOSING:
      sparki.gripperClose();
      break;
  
    case GrabberState::OPENING:
      sparki.gripperOpen();
      break;

    default:
      sparki.gripperStop();
      break;

  }
  
  _state = state;
  if (reset) {
    _timer = 0;
    _millis = millis();
  }

  return true;
};

GrabberState Grabber::getState() {
  return _state;
};

bool Grabber::isState(GrabberState state) {
  return state == _state;
};
