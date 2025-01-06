#include "ultrasonic.h"
#include "carMotors.h"
#include"lightsensor.h"

CarMotors engine;
LightSensor right(A0);
LightSensor center(A1);
LightSensor left(A2);
UltraSonic sensor;

void setup() {
sensor.init();
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
  engine.turnLeft();
  };
  if (c == 1) {
  engine.turnRight();
  };
  if (c == 2) {
    engine.goForward();
  }
  delay(500);
}

int check() {
  float distance = sensor.getDistance(); // Mesure la distance
  if (distance < 600) return 0;
  if (right.line() && left.line()!=true) return 0;
  if (left.line() && right.line()!=true) return 1;
  if (left.line() && right.line()) return 0;
  if (left.line() && right.line() && center.line()) return 0;
  return 2;
}

//void loop() {
//  // Balayage à différents angles
//  for (int angle = 0; angle <= 180; angle += 30) {
//    sensor.turnHead(angle); // Déplace le servo à l'angle
//    delay(100); // Attend que le servo se déplace
//
//    float distance = sensor.getDistance(); // Mesure la distance
//    if (distance < 600) {
//      Serial.println("condition vérifié");
//      };
//    Serial.print("Angle: ");
//    Serial.print(angle);
//    Serial.print("° - Distance: ");
//    Serial.print(distance);
//    Serial.println(" mm");
//
//    delay(50); // Temps entre deux mesures
//  }
//  delay(100); // Temps d'attente avant de recommencer le balayage
//}