#include <Sparki.h>
#include "WallFinder.hpp"
#include "Grabber.hpp"

WallFinder wallFinder;
Grabber grabber;

bool paused = false;

void setup() {
  // Initiate objects
  sparki.RGB(RGB_YELLOW);
  sparki.beep(220, 300); // A3, 0.3s
  wallFinder.setup();
  grabber.setup();
  
  // Startup beep
  sparki.beep(440, 300); // A4, 0.3s
  delay(300);
  sparki.beep(880, 500); // A5, 0.5s
}

void loop() {
  remote();

  if (paused) {
    sparki.RGB(RGB_RED);
    return;
  }
  sparki.RGB(RGB_GREEN);

  if (false && wallFinder.isState(WallFinderState::CALIBRATE)) {
    wallFinder.update();
  } else {
    grabber.update();
    if (grabber.isState(GrabberState::SEARCH) || grabber.isState(GrabberState::HOLD)) {
      wallFinder.update();
    } else {
      sparki.moveStop();
    }
  }

  // Debugging
  sparki.clearLCD();
  wallFinder.debug();
  grabber.debug();
  sparki.updateLCD();
}

void finish() {
  sparki.RGB(RGB_BLUE);

  sparki.moveStop();
  delay(500);

  // melody
  sparki.beep(523, 300); // C5
  delay(300);
  sparki.beep(659, 300); // E5
  delay(300);
  sparki.beep(784, 500); // G5

  // 360 spin
  sparki.moveRight(360);
  delay(2000);

  // Let go of object
  grabber.release(true);

  // Wait
  while (sparki.readIR() != 69) { // rotate left
    sparki.moveStop();
    delay(1000);
  }

  // Reset
  grabber.reset();
  wallFinder.reset();
}

void remote() {

  // /------^-----\
  // |            |
  // | 69  70  71 |
  // | 68  64  67 |
  // |  7  21   9 |
  // | 22  25  13 |
  // | 12  24  94 |
  // |  8  28  90 |
  // | 66  82  74 |
  // \____________/


  int code = sparki.readIR();
  if (code < 0) return;
 
  sparki.print("Received code: ");
  sparki.println(code);

  switch (code) {

    // Reset
    case 69: // rotate left
      wallFinder.reset();
      grabber.reset();
      paused = false;
      break;

    // Pause
    case 64: // square button
      sparki.moveStop();
      sparki.gripperStop();
      paused = !paused;
      break;

    // Finish
    case 67: // rotate right
      if (grabber.isState(GrabberState::HOLD) && !wallFinder.isState(WallFinderState::CALIBRATE)) {
        finish();
      } else {
        // Error beep
        sparki.RGB(RGB_ORANGE);
        sparki.beep(220, 300); // A3, 0.3s
        delay(300);
        sparki.beep(110, 500); // A2, 0.5s
        // NOTE: RGB is reset in loop
      }
      break;
    
  }
}
