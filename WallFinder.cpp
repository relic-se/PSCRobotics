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
      calibrate();
      break;

    case WallFinderState::MOVE:
      move();
      break;

    case WallFinderState::FIND:
      find();
      break;

    case WallFinderState::TURN_LEFT:
    case WallFinderState::TURN_RIGHT:
      turn();
      break;

    case WallFinderState::CHECK_FINISH:
      checkFinish();
      break;
  }

  // Update ticks
  if (_ticks > 0) {
    _ticks--;
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
  // State
  sparki.print("State: ");
  sparki.println(getStateName());

  // Sensors
  sparki.print("L: ");
  sparki.print(getBoolName(_found_left));
  sparki.print(" F: ");
  sparki.print(getBoolName(_found_front));
  sparki.print(" R: ");
  sparki.print(getBoolName(_found_right));
  sparki.print(" D: ");
  sparki.println(getBoolName(_found_dead_end));
};

const char *WallFinder::getStateName() {
  switch (_state) {
    case WallFinderState::CALIBRATE:
      return "Calibrate";
    case WallFinderState::MOVE:
      return "Move";
    case WallFinderState::FIND:
      return "Find Wall";
    case WallFinderState::TURN_LEFT:
      return "Turning Left";
    case WallFinderState::TURN_RIGHT:
      return "Turning Right";
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

void WallFinder::calibrate() {
  // Move a little and check if we've hit the left wall
  sparki.moveLeft(1);
  if (_found_left) {
    setState(WallFinderState::MOVE);
  }
};

void WallFinder::move() {
  sparki.moveForward();
  if (_found_dead_end) {
    // Right turn
    setState(WallFinderState::TURN_RIGHT);
  //} else if (!_found_left && !_found_right && !_found_front) { // All sensors off, finished
  //  setState(WallFinderState::CHECK_FINISH);
  } else if (!_found_left) { // Off of wall
    setState(WallFinderState::FIND);
    sparki.moveForward(0.5);
  }
};

void WallFinder::find() {
  // Search right then left
  if (findWall(DIR_CW) || findWall(DIR_CCW)) {
    // If we find the wall, go back to move state
    setState(WallFinderState::MOVE);
  }

  // If we can't find the wall, take a left turn
  setState(WallFinderState::TURN_LEFT);
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

void WallFinder::turn() {
  if (isState(WallFinderState::TURN_LEFT)) { // left turn
    if (_ticks > 90) {
      sparki.moveForward(1);
    } else {
      sparki.moveLeft(1);
    }
  } else { // right turn
    sparki.moveRight(1);
  }

  // If we've completed the turn, return to move state
  if (!_ticks) {
    setState(WallFinderState::MOVE);
  }
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
    case WallFinderState::TURN_LEFT:
      _ticks = TURN_LEFT_MOVE + 90;
      break;
    case WallFinderState::TURN_RIGHT:
      _ticks = 90;
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
