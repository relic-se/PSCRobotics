#include <Sparki.h>
#include "WallFinder.hpp"

WallFinder wallFinder;

void setup() {
  // Startup beep
  sparki.beep(440, 300); // A4, 0.3s
  delay(300);
  sparki.beep(880, 500); // A5, 0.5s
}

void loop() {
  wallFinder.update();
  wallFinder.debug();

  // Handle finish state
  if (wallFinder.isState(WallFinderState::FINISH)) {
    finish();
  }
}

void finish() {
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
  // TODO: Detect remote
  while (true) {
    sparki.moveStop();
    delay(1000);
  }

  // TODO: Reset state
}
