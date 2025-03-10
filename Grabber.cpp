
#include "Grabber.hpp"

Grabber::Grabber() : Component() {
  _open = false;
};

void Grabber::setup() {
  sparki.servo(SERVO_CENTER);
  Component::setup();
};

void Grabber::reset() {
  open();
  Component::reset();
};

void Grabber::open() {
  if (_open) return;
  sparki.gripperOpen(GRABBER_OPEN);
  while (sparki.areMotorsRunning()) {
    delay(100);
  }
  sparki.gripperStop();
  _open = true;
};

void Grabber::close() {
  if (!_open) return;
  sparki.gripperClose(GRABBER_OPEN);
  while (sparki.areMotorsRunning()) {
    delay(100);
  }
  sparki.gripperStop();
  _open = false;
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
      setTicks(_distance);
      break;
    case GrabberState::RELEASE:
      open();
      setState(GrabberState::COMPLETE);
      break;
  }
  return current;
};

void Grabber::updateState(GrabberState state) {
  // Read ultrasonic sensor
  if (isState(GrabberState::SEARCH)) {
    _distance = sparki.ping();
  }

  switch (state) {

    case GrabberState::SEARCH:
      // NOTE: If ping is invalid, distance is -1
      if (_distance >= 0 && _distance <= GRABBER_DISTANCE) {
        setState(GrabberState::GRAB);
      }
      break;

    case GrabberState::GRAB:
      if (!noTicks()) {
        sparki.moveForward(1);
      } else {
        close();
        setState(GrabberState::HOLD);
      }
      break;

    case GrabberState::RELEASE:
      // doesn't run
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
