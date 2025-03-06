
#include "Grabber.hpp"

Grabber::Grabber() {
  sparki.gripperOpen(GRABBER_INITIAL_OPEN);
  sparki.servo(SERVO_CENTER);
  reset();
};

void Grabber::reset() {
  setState(GrabberState::SEARCH);
};

bool Grabber::release(bool blocking) {
  if (!isState(GrabberState::HOLD)) return false;
  if (!setState(GrabberState::RELEASE)) return false;
  if (!blocking) return true;

  // Wait until we've finished letting go of the object
  while (isState(GrabberState::RELEASE)) {
    delay(1);
    update();
  }
  return true;
};

void Grabber::update() {
  // Update timer
  unsigned long current = millis();
  _timer += current - _millis;
  _millis = current;

  // Read ultrasonic sensor
  distance = sparki.ping();

  switch (_state) {

    case GrabberState::SEARCH:
      // NOTE: If ping is invalid, distance is -1
      if (distance >= 0 && distance <= GRABBER_DISTANCE) {
        setState(GrabberState::GRAB);
      }
      break;

    case GrabberState::GRAB:
      if (_timer >= GRABBER_DURATION) {
        setState(GrabberState::HOLD);
      }
      break;

    case GrabberState::RELEASE:
      if (_timer > GRABBER_DURATION) {
        setState(GrabberState::COMPLETE);
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

    case GrabberState::GRAB:
      sparki.gripperClose();
      break;
  
    case GrabberState::RELEASE:
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
