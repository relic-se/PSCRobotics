
#include "Grabber.hpp"

Grabber::Grabber() : Component() {
  sparki.gripperOpen(GRABBER_INITIAL_OPEN);
  sparki.servo(SERVO_CENTER);
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

int Grabber::getDistance() {
  return _distance;
};

GrabberState Grabber::changeState(GrabberState current, GrabberState previous) {
  switch (current) {
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
  return current;
};

void Grabber::updateState(GrabberState state) {
  // Read ultrasonic sensor
  _distance = sparki.ping();

  switch (state) {

    case GrabberState::SEARCH:
      // NOTE: If ping is invalid, distance is -1
      if (_distance >= 0 && _distance <= GRABBER_DISTANCE) {
        setState(GrabberState::GRAB);
      }
      break;

    case GrabberState::GRAB:
      if (getTimer() >= GRABBER_DURATION) {
        setState(GrabberState::HOLD);
      }
      break;

    case GrabberState::RELEASE:
      if (getTimer() > GRABBER_DURATION) {
        setState(GrabberState::COMPLETE);
      }
      break;

  }
};

void Grabber::debug() {
  Component::debug();

  // Ping distance
  sparki.print("Dist: ");
  sparki.print(_distance);
  sparki.println("cm");
};

const char *Grabber::getStateName(GrabberState state) {
  switch (state) {
    case GrabberState::SEARCH:
      return "Searching";
    case GrabberState::GRAB:
      return "Grabbing";
    case GrabberState::HOLD:
      return "Holding";
    case GrabberState::RELEASE:
      return "Releasing";
    case GrabberState::COMPLETE:
      return "Completed";
    default:
      return Component::getStateName(state);
  }
};
