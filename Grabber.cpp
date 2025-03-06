
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
  sparki.print(distance);
  sparki.println(" cm");
};

bool Grabber::setState(GrabberState state) {
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
  return true;
};

GrabberState Grabber::getState() {
  return _state;
};

bool Grabber::isState(GrabberState state) {
  return state == _state;
};
