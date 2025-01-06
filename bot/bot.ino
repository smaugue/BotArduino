#include "ultrasonic.h"
#include "carMotors.h"
#include"lightsensor.h"
#define BASE_SPEED 50

CarMotors engine;
LightSensor right(A0);
LightSensor center(A1);
LightSensor left(A2);
UltraSonic sensor;

void setup() {
sensor.init();
engine.init(BASE_SPEED);
Serial.begin(9600);
Serial.println("Start...");
Serial.println();
}

void loop() {
  Serial.println(right.readValue());
  //engine.stop();
  int b = checkwall();
  int a = checkline();
  int c = 0;
  Serial.println(a);
  Serial.println(b);
  if ( a != 0 && a == b) {
    c = b;
  };
  if (a==2) c=b;
  if (b==2) c=a;
  Serial.println(c);
  //engine.setSpeed(BASE_SPEED);
  if (c == 0) {
    engine.goBackward();
    delay(100);
    engine.turnLeft();
    delay(100);
  };
  if (c == 1) {
    engine.goBackward();
    delay(100);
    engine.turnRight();
    delay(100);
  };
  if (c == 2) {
    engine.goForward();
  };
}

int checkline() {
  if (right.line() && left.line()!=true) {
    engine.stop();
    Serial.println("not ok");
    return 0;
  };
  if (left.line() && right.line()!=true) {
    engine.stop();
    Serial.println("not ok");
    return 1;
  };
  if (left.line() && right.line()) {
    engine.stop();
    Serial.println("not ok");
    return 0;
  };
  if (left.line() && right.line() && center.line()) {
    engine.stop();
    Serial.println("not ok");
    return 0;
  };
  return 2;
}

int checkwall() {
  sensor.turnHead(90);
  delay(50);
  float dcenter = sensor.getDistance();
  if (dcenter < 200) engine.stop();
  sensor.turnHead(40);
  delay(50);
  float dleft = sensor.getDistance();
  if (dleft < 200) engine.stop();
  sensor.turnHead(90);
  delay(50);
  dcenter = sensor.getDistance();
  if (dcenter < 200) engine.stop();
  sensor.turnHead(140);
  delay(50);
  float dright = sensor.getDistance();
  if (dcenter < 200) {
    if (dleft > 200) return 0;
    return 1;
  };
  if (dleft < 200) return 1;
  if (dright < 200) return 0;
  return 2;
}