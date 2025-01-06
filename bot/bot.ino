#include "ultrasonic.h"
#include "carMotors.h"
#include"lightsensor.h"

CarMotors engine;
LightSensor right(A0);
LightSensor center(A1);
LightSensor left(A2);
UltraSonic ultrasonic;

void setup() {
ultrasonic.init();
engine.init(50);
Serial.begin(9600);
Serial.println("Start...");
Serial.println();
}

void loop() {
  Serial.println(right.readValue());
  int c = check();
  Serial.println(c);
  if (c == 0) {
    Serial.println("ok ta mère");
  engine.turnLeft();
  };
  if (c == 1) {
  engine.turnRight();
  };
  if (c == 2) {
    engine.goForward();
  }
  Serial.println("ntm bot de con");
  delay(500);
}

int check() {
//if (ultrasonic.obstacl()) return 0; pas bon du tt
if (right.line() && left.line()!=true) return 0;
if (left.line() && right.line()!=true) return 1;
if (left.line() && right.line()) return 0;
return 2;
}
