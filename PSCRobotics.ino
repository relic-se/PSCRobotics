#include <Sparki.h>

// for sensors
int edgeLeftOuter, edgeLeftInner;
int edgeRightOuter, edgeRightInner;
int threshold;

// for calibrating
bool foundLeftWall = false, calibrated = false, rightWall = false;

void setup() {
  // Begin serial and wait up to 2 seconds for connection
  Serial.begin(9600);
  int serialDelay = 2000;
  while (!Serial && serialDelay--) {
    delay(1);
  }
}

void loop() {
  threshold = 700;

  readSensors();

  calibrateSparki();

  //sparki.moveForward();

  sparki.clearLCD();
  sparki.print("Left: ");
  sparki.println(edgeLeftOuter);
  sparki.print("Right: ");
  sparki.println(edgeRightOuter);
  sparki.updateLCD();
}

void readSensors() {
  edgeLeftOuter = sparki.edgeLeft();
  edgeLeftInner = sparki.lineLeft();
  edgeRightOuter = sparki.edgeRight();
  edgeRightInner = sparki.lineRight();
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
