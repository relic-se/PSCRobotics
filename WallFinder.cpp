#include "WallFinder.hpp"

WallFinder::WallFinder() {
  reset();
};

void WallFinder::reset() {
  setState(WallFinderState::CALIBRATE);
};

void WallFinder::update() {
  // Update timer
  unsigned long current = millis();
  _timer += current - _millis;
  _millis = current;

  // Evaluate sensors
  read();

  switch (_state) {
    case WallFinderState::CALIBRATE:
    case WallFinderState::CALIBRATE_LEFT:
    case WallFinderState::CALIBRATE_RIGHT:
      if (calibrate()) {
        setState(WallFinderState::MOVE);
      }
      break;

    case WallFinderState::MOVE:
      move();
      break;

    case WallFinderState::FIND:
      find();
      break;

    case WallFinderState::CHECK_FINISH:
      checkFinish();
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
  sparki.print(" F: ");
  sparki.print(getBoolName(_found_front));
  sparki.print(" R: ");
  sparki.println(getBoolName(_found_right));

  sparki.print("D: ");
  sparki.println(getBoolName(_found_dead_end));

  // Calibration
  sparki.print("C: ");
  sparki.print(getBoolName(_calibrated));
  sparki.print(" CL: ");
  sparki.print(getBoolName(_calibrate_left));
  sparki.print(" CR: ");
  sparki.println(getBoolName(_calibrate_right));

  sparki.updateLCD();
};

const char *WallFinder::getStateName() {
  switch (_state) {
    case WallFinderState::CALIBRATE:
      return "Calibrate";
    case WallFinderState::CALIBRATE_LEFT:
      return "Calibrate Left";
    case WallFinderState::CALIBRATE_RIGHT:
      return "Calibrate Right";
    case WallFinderState::MOVE:
      return "Move";
    case WallFinderState::FIND:
      return "Find Wall";
    case WallFinderState::CHECK_FINISH:
      return "Checking Finish";
    case WallFinderState::FINISH:
      return "Finish";
  }
  return "unknown";
};

const char *WallFinder::getBoolName(bool value) {
  return value ? "y" : "n";
};

bool WallFinder::calibrate() {
  if (_calibrated) return false;

  _calibrate_right = false;

  // Move a little and check if we've hit the left wall yet
  if (!_calibrate_left && !_calibrated) {
    setState(WallFinderState::CALIBRATE_LEFT, false);
    sparki.moveLeft(1);
    _calibrate_left = _found_left;
  }

  if (_calibrate_left && !_calibrated) {
    _calibrated = true;
    /*
    if (!_calibrate_right) {
      setState(WallFinderState::CALIBRATE_RIGHT, false);
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
    */
  }

  return _calibrated;
};

void WallFinder::move() {
  sparki.moveForward();
  if (_found_dead_end) {
    // Right turn
    sparki.moveRight(90);
  //} else if (!_found_left && !_found_right && !_found_front) { // All sensors off, finished
  //  setState(WallFinderState::CHECK_FINISH);
  } else if (!_found_left) { // Off of wall
    setState(WallFinderState::FIND);
    sparki.moveForward(0.5);
  }
};

void WallFinder::find() {
  // Search right then left
  if (!findWall(DIR_CW) && !findWall(DIR_CCW)) {
    // Take 90 degree left turn
    sparki.moveForward(7);
    sparki.moveLeft(90);
  }

  // Always go back to move state after wall find
  setState(WallFinderState::MOVE);
};

bool WallFinder::findWall(int dir) {
  int count = 0;
  while (!_found_left && count++ < FIND_TURN_DEGREES) {
    if (dir == DIR_CW) {
      sparki.moveRight(1);
    } else {
      sparki.moveLeft(1);
    }
    read();
  }

  if (!_found_left) { // Couldn't find wall
    // Turn back to original position
    if (dir == DIR_CW) {
      sparki.moveLeft(FIND_TURN_DEGREES);
    } else {
      sparki.moveRight(FIND_TURN_DEGREES);
    }
  }
  
  return _found_left;
};

void WallFinder::checkFinish() {
  sparki.moveForward();
  if (_found_left || _found_right || _found_front) {
    setState(WallFinderState::MOVE);
  } else if (_timer >= FINISH_TIME) {
    setState(WallFinderState::FINISH);
  }
};

bool WallFinder::setState(WallFinderState state, bool reset) {
  if (isState(state)) return false;
  
  switch (state) {
    case WallFinderState::CALIBRATE:
      _calibrated = false;
      _calibrate_left = _calibrate_right = false;
      break;
    case WallFinderState::FINISH:
      if (!isState(WallFinderState::CHECK_FINISH)) {
        state = WallFinderState::CHECK_FINISH;
      } else if (_timer < FINISH_TIME) {
        return false;
      }
      break;
  }

  _state = state;
  if (reset) {
    sparki.moveStop();
    _timer = 0;
    _millis = millis();
  }

  return true;
};
