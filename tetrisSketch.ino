#include "engine.h"
#define joySw A0
#define joyY A1
#define joyX A2

int currentTime;

void setup() {
  // put your setup code here, to run once:
//  Serial.begin(9600);
  Serial.begin(9875);
//  Serial.println("Hello World!");
  randomSeed(analogRead(3));
//  randomSeed(millis());
  currentTime = millis();
}

TetrisEngine tetrisEngine = TetrisEngine();
bool oneLoop = true;

void loop() {
  // put your main code here, to run repeatedly:
  int xValue = analogRead(joyX); // > 766 = right, <256 = left
  int yValue = analogRead(joyY); // 511 by default, >766 = down, <256 = up
  int swValue = analogRead(joySw); // 0 when pushed, random otherwise

  struct Controls controls = {
    xValue > 766,
    xValue < 256,
    yValue < 256,
    yValue > 766,
    swValue == 0,
    false,
    false,
    false,
  };

  if (oneLoop) {
//    oneLoop = false;
    Serial.println("");
    tetrisEngine.loop(controls);
    delay(1000);
  }
  //  Serial.println();
}
