#pragma once

#include <Arduino.h>

template <typename T>
class Component
{
public:
  Component() { };

  virtual void setup() {
    reset();
  };
  
  virtual ~Component() = default;
  
  void update() {
    // Update timer
    unsigned long current = millis();
    _timer += current - _millis;
    _millis = current;

    // Handle state specific logic
    updateState(_state);

    // Update ticks
    if (_ticks > 0) {
        _ticks--;
    }
  };

  virtual void debug() {
    // State
    sparki.print("State: ");
    sparki.println(getStateName(_state));
  };
  
  T getState() {
    return _state;
  };

  bool isState(T state) {
    return _state == state;
  };

  void reset() {
    setState((T)0);
  };

protected: 
  bool setState(T state) {
    // Make sure we're not assigning to the current state
    if (isState(state)) return false;

    // State initialization logic
    state = changeState(state, _state);
    if (isState(state)) return false; // Check again

    // Update current state
    _state = state;

    // Reset timer
    _timer = 0;
    _millis = millis();

    // Indicate that we've successfully updated
    return true;
  };

  virtual T changeState(T next, T previous) = 0;
  virtual void updateState(T state) = 0;
  virtual const char *getStateName(T state) {
    return "unknown";
  };

  const char *getBoolName(bool value) {
    return value ? "y" : "n";
  };

  unsigned long getTimer() {
    return _timer;
  };

  void setTicks(unsigned long value) {
    _ticks = value;
  };

  unsigned long getTicks() {
    return _ticks;
  };

  bool noTicks() {
    return !_ticks;
  };
  
private:
  T _state;
  unsigned int _timer, _millis, _ticks;
};
