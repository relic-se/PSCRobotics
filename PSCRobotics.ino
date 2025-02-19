#include <Sparki.h>

void setup() {
  // Begin serial and wait up to 2 seconds for connection
  Serial.begin(9600);
  int serialDelay = 2000;
  while (!Serial && serialDelay--) {
    delay(1);
  }
}

void loop() {

}
