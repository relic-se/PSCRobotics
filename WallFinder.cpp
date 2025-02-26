#include "WallFinder.hpp"

WallFinder::WallFinder() {
  reset();
};

void WallFinder::reset() {
  _state = WallFinderState::CALIBRATE;
  _calibrated = false;
  _calibrate_left = _calibrate_right = false;
};

void WallFinder::update() {
  // Evaluate sensors
  read();

  switch (_state) {
    case WallFinderState::CALIBRATE:
      if (calibrate()) {
        _state = WallFinderState::MOVE;
      }
      break;

    case WallFinderState::MOVE:
      _state = move();
      break;

    case WallFinderState::FIND:
      find();

      // Always go back to move state after wall find
      _state = WallFinderState::MOVE;
      break;
  }
};

WallFinderState WallFinder::getState() {
  return _state;
};

bool WallFinder::isState(WallFinderState state) {
  return state == _state;
};

bool WallFinder::checkSensor(int value) {
  return value < SENSOR_THRESHOLD;
}

void WallFinder::read() {
  _sensor_left_outer = sparki.edgeLeft();
  _sensor_left_inner = sparki.lineLeft();
  _found_left = checkSensor(_sensor_left_outer);
  
  _sensor_right_outer = sparki.edgeRight();
  _sensor_right_inner = sparki.lineRight();
  _found_right = checkSensor(_sensor_right_outer);

  _sensor_front = sparki.lineCenter();
  _found_front = checkSensor(_sensor_front);

  if (_found_left && _found_front) {
    _found_left = false;
    _found_dead_end = true;
  }

  /* // NOTE: Not necessary if following left wall
  if (_found_right && _found_front) {
    _found_right = false;
    _found_dead_end = true;
  }
  */
};

void WallFinder::debug() {
  sparki.clearLCD();

  // State
  sparki.print("State: ");
  sparki.println(getStateName());

  // Sensors
  sparki.print("L: ");
  sparki.print(getBoolName(_found_left));
  sparki.print(" R: ");
  sparki.print(getBoolName(_found_right));
  sparki.print(" F: ");
  sparki.println(getBoolName(_found_front));

  sparki.print("D: ");
  sparki.println(getBoolName(_found_dead_end));

  // Calibration
  sparki.print("C: ");
  sparki.print(getBoolName(_calibrated));
  sparki.print(" CL: ");
  sparki.print(getBoolName(_calibrate_left));
  sparki.print(" CR: ");
  sparki.println(getBoolName(_calibrate_right));
};

const char *WallFinder::getStateName() {
  switch (_state) {
    case WallFinderState::CALIBRATE:
      return "Calibrate";
    case WallFinderState::MOVE:
      return "Move";
    case WallFinderState::FIND:
      return "Find Wall";
    case WallFinderState::FINISH:
      return "Finish";
  }
  return "unknown";
};

const char *WallFinder::getBoolName(bool value) {
  return value ? "y" : "n";
};

bool WallFinder::calibrate(bool blocking) {
  if (_calibrated) return false;

  if (blocking) _calibrate_left = false;

  do {
    _calibrate_right = false;

    // Move a little and check if we've hit the left wall yet
    if (!_calibrate_left && !_calibrated) {
      sparki.moveLeft(1);
      _calibrate_left = _found_left;
    }

    if (_calibrate_left && !_calibrated) {
      if (!_calibrate_right) {
        sparki.motorRotate(MOTOR_LEFT, DIR_CCW, 15);
        sparki.motorRotate(MOTOR_RIGHT, DIR_CW, 55);
      }

      if (_found_left && _found_right) {
        // Sparki is -30 degrees from wall when both sensors are active
        _calibrate_right = true;

        // Re-orient sparki
        sparki.moveForward();
        delay(1000);
        sparki.moveRight(24);
        sparki.moveBackward();
        delay(1000);
        sparki.moveStop();

        _calibrated = true;
      }
    }
  } while (blocking && !_calibrated);

  return _calibrated;
};

WallFinderState WallFinder::move() {
  WallFinderState state = WallFinderState::MOVE;
  if (_found_dead_end) {
    // Right turn
    sparki.moveRight(90);
    sparki.moveForward();
  } else if (_found_left) { // Continue moving
    sparki.moveForward();
  } else if (!_found_left && !_found_right && !_found_front) { // All sensors off, finished
    sparki.moveStop();
    state = WallFinderState::FINISH;
  } else if (!_found_left) { // Off of wall
    sparki.moveStop();
    state = WallFinderState::FIND;
    sparki.moveForward(0.5);
  }
  return state;
};

void WallFinder::find() {
  // Search right then left
  if (findWall(DIR_CW) || findWall(DIR_CCW)) {
    return;
  }

  // Check for 90 degree left turn
  sparki.moveForward(7);
  sparki.moveLeft(90);
  sparki.moveStop();
};

bool WallFinder::findWall(int dir) {
  int count = 0;
  while (!_found_left && count++ < FIND_TURN_COUNT) {
    if (dir == DIR_CW) {
      sparki.moveRight();
    } else {
      sparki.moveLeft();
    }
    read();
  }

  if (!_found_left) { // Couldn't find wall
    // Turn back to original position
    if (dir == DIR_CW) {
      sparki.moveLeft(FIND_TURN_COUNT);
    } else {
      sparki.moveRight(FIND_TURN_COUNT);
    }
  }
  
  return _found_left;
};
