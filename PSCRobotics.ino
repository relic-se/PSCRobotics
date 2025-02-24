#include <Sparki.h>

// for sensors
int edgeLeftOuter, edgeLeftInner;
int edgeRightOuter, edgeRightInner;
int edgeFront;
int threshold;

// for wall finding
bool foundLWall, foundRWall, foundFront;
int wallFindType = 0;
int turnCount = 0;
bool foundWall, foundDeadEnd;

// for calibrating
bool foundLeftWall = false, calibrated = false, rightWall = false;

void setup() {
  // Begin serial and wait up to 2 seconds for connection
  Serial.begin(9600);
  int serialDelay = 2000;
  while (!Serial && serialDelay--) {
    delay(1);
  }

  // Startup beep
  sparki.beep(440, 300); // A4, 0.3s
  delay(300);
  sparki.beep(880, 500); // A5, 0.5s
}

void loop() {
  threshold = 700;

  readSensors();

  if (!calibrated) {
    calibrateSparki();
  } else {
    if (foundDeadEnd && wallFindType == 0) {
      sparki.moveRight(90);
      sparki.moveForward();
    } else if (foundLWall) {
      wallFindType = 0;
      sparki.moveForward();
      sparki.println("Found wall.");
    } else if (!foundLWall && !foundRWall && !foundFront) {
      sparki.moveStop();
      sparki.println("Exited maze.");
      finish();
    } else if (!foundLWall && wallFindType == 0) {
      sparki.moveStop();
      sparki.println("Didn't find wall.");
      sparki.updateLCD();
      wallFindType = 1; // enter wall finding routine
      sparki.moveForward(0.5);
    }
    
    if (wallFindType > 0) {
      wallFind();
    }
    
    sparki.updateLCD();
  }
}

void readSensors() {
  edgeLeftOuter = sparki.edgeLeft();
  edgeLeftInner = sparki.lineLeft();
  foundLWall = edgeLeftOuter < threshold;
  
  edgeRightOuter = sparki.edgeRight();
  edgeRightInner = sparki.lineRight();
  foundRWall = edgeRightOuter < threshold;

  edgeFront = sparki.lineCenter();
  foundFront = edgeFront < threshold;

  if (foundLWall && foundFront) {
    foundLWall = false;
    foundDeadEnd = true;
  }
}

void calibrateSparki() {
  rightWall = false;

  if (!foundLeftWall && !calibrated) {
    sparki.moveLeft(1);
    if (edgeLeftOuter < threshold) {
      foundLeftWall = true;
    }
  }

  if (foundLeftWall && !calibrated) {
    if (!rightWall) {
      sparki.motorRotate(MOTOR_LEFT, DIR_CCW, 15);
      sparki.motorRotate(MOTOR_RIGHT, DIR_CW, 55);
    }

    if (edgeLeftOuter < threshold && edgeRightOuter < threshold) {
      rightWall = true;
      sparki.moveForward();
      delay(1000);
      sparki.moveRight(24);
      sparki.moveBackward();
      delay(1000);
      sparki.motorStop(MOTOR_LEFT);
      sparki.motorStop(MOTOR_RIGHT);
      calibrated = true;
    }
  }
}

void wallFind() {
  switch (wallFindType) {
    
    case 1: // off track, search around 5 degrees
      sparki.println("Searching right");
      while (!foundLWall && turnCount++ < 5) {
        sparki.moveRight();
        readSensors();
      }
      if (turnCount >= 5) { // couldn't find wall
        sparki.moveLeft(5); // turn back to original position
        sparki.println("Done searching right");
      } else { // found wall
        wallFindType = 0;
        turnCount = 0;
        return;
      }
      turnCount = 0;

      sparki.println("Searching left");
      while (!foundLWall && turnCount++ < 5) {
        sparki.moveLeft();
        readSensors();
      }
      if (turnCount >= 5) { // couldn't find wall
        sparki.moveRight(5); // turn back to original position
        wallFindType = 2; // move on to next wall finding type
        sparki.println("Done searching left");
      } else { // found a wall
        sparki.println(turnCount);
        wallFindType = 0;
        turnCount = 0;
        return;
      }
      turnCount = 0;
      break;
      
    case 2: // check for 90 degree turn
      sparki.moveForward(7);
      sparki.moveLeft(90);
      sparki.moveStop();
      sparki.println("Search level 2");
      wallFindType = 0;
      break;
      
    default: // idk what happened
      break;
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
