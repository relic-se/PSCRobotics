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
Void remote(){

// /------^-----
// |            |
// | 69  70  71 |
// | 68  64  67 |
// |  7  21   9 |
// | 22  25  13 |
// | 12  24  94 |
// |  8  28  90 |
// | 66  82  74 |
// ____________/


{int code = sparki.readIR();
 
  if(code != -1){
    sparki.print("Received code: ");
    sparki.println(code);
  }

  switch (code){

   // case 12: calibrateSparki();break;
    case 64: sparki.moveStop();break; //square button;
    case 24: sparki.clear();break
    
  }
}
  // TODO: Reset state
}
