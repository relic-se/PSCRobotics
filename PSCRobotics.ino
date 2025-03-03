#include <Sparki.h>
#include "WallFinder.hpp"

WallFinder wallFinder;

bool paused = false;

void setup() {
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

  wallFinder.update();

  // Debugging
  sparki.clearLCD();
  wallFinder.debug();
  sparki.updateLCD();

  // Handle finish state
  if (wallFinder.isState(WallFinderState::FINISH)) {
    finish();
  }
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

  // Wait
  while (sparki.readIR() != 69) { // rotate left
    sparki.moveStop();
    delay(1000);
  }

  // Reset
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

    case 12: // 1
      wallFinder.reset();
      break;

    case 64: // square button
      sparki.moveStop();
      paused = !paused;
      break;
    
  }
}
