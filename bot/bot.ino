#include "ultrasonic.h"
#include "carMotors.h"
#include "lightsensor.h"
#include <IRremote.h>       // infrared Library
#include <FastLED.h>        // RGB Led Library

CarMotors engine;
LightSensor right(A0);
LightSensor center(A1);
LightSensor left(A2);
UltraSonic sensor;

const int RGBPin = 4;       // RGB Led pin
const int receiverPin = 9;  // infrared signal receiving pin
const int numLEDs = 1;
CRGB leds[numLEDs];
uint32_t tabColors[]= {0xFF0000,0xFB4C0D,0x00FF00};

IRrecv irrecv(receiverPin); // initialization of the IRrecv object
decode_results results;     // define structure type

unsigned long previousWallCheckTime = 0; // Pour le timer de checkwall
const unsigned long wallCheckInterval = 100; // Intervalle d'exécution de checkwall (ms)

void setup() {
  sensor.init();
  engine.init(0);
  Serial.begin(9600);
  Serial.println("Robot Start...");
  irrecv.enableIRIn();    // Start receiving
  FastLED.addLeds<NEOPIXEL, RGBPin>(leds, numLEDs);
  FastLED.setBrightness(20);
  FastLED.showColor(tabColors[1]);
}

void loop() {

  if (irrecv.decode(&results)) { // if a code is available, write it in result
        unsigned long int codeC = results.value; // extracts code from results
        Serial.print(codeC); Serial.print(" ");
        Serial.print(codeC,BIN); Serial.print(" ");
        Serial.println(codeC,HEX);
        
        if (results.value == 0xFF02FD) {
          FastLED.showColor(tabColors[2]);
          engine.init(85);
        }  
        irrecv.resume(); // Ready to receive the next value
    }

  // Appel périodique de checkwall
  unsigned long currentTime = millis();
  if (currentTime - previousWallCheckTime >= wallCheckInterval) {
    previousWallCheckTime = currentTime;
    checkwall(); // Appeler checkwall périodiquement
  }

  // Appel des autres fonctionnalités
  int result = check();
  Serial.println(result);
  if (result == -1) {
    avoidObstacle(); // Appel de la procédure d'évitement
  } 
  if (result == 1) engine.drive(1, 0.15);
}

int check() {
  Serial.println(center.read());
  Serial.println(center.line());
  if (center.line() || left.line() || right.line()) {
    engine.stop();
    return -1;
  }
  if (sensor.wall()) {
    engine.stop();
    return -1;
  }
  return 1;
}

void checkwall() {
  static int state = 0;             // Pour suivre l'état de la fonction
  static unsigned long lastTime = 0; // Pour gérer les temporisations entre les états
  float dcenter, dleft, dright;

  switch (state) {
    case 0: // Vérifier l'obstacle au centre
      sensor.turnHead(90);
      dcenter = sensor.getDistance();
      if (dcenter < 200) {
        engine.stop();
        state = 1; // Passer à l'état pour éviter l'obstacle
        lastTime = millis();
      } else {
        state = 3; // Si pas d'obstacle, passer à la vérification à gauche
      }
      break;

    case 1: // Éviter l'obstacle (reculer)
      if (millis() - lastTime > 300) { // Temps pour reculer
        engine.goBackward();
        lastTime = millis();
        state = 2;
      }
      break;

    case 2: // Tourner à droite
      if (millis() - lastTime > 800) { // Temps pour tourner à droite
        engine.turnLeft();
        lastTime = millis();
        state = 0; // Revenir à l'état initial après l'évitement
      }
      break;

    case 3: // Vérifier l'obstacle à gauche
      sensor.turnHead(40);
      dleft = sensor.getDistance();
      if (dleft < 200) {
        engine.stop();
        state = 1; // Passer à l'état pour éviter l'obstacle
        lastTime = millis();
      } else {
        state = 4; // Si pas d'obstacle, passer à la vérification à droite
      }
      break;

    case 4: // Vérifier l'obstacle à droite
      sensor.turnHead(140);
      dright = sensor.getDistance();
      if (dright < 200) {
        engine.stop();
        state = 1; // Passer à l'état pour éviter l'obstacle
        lastTime = millis();
      } else {
        state = 0; // Revenir à l'état initial si pas d'obstacle
      }
      break;
  }
}

// Fonction pour gérer la procédure d'évitement dans le loop
void avoidObstacle() {
  engine.goBackward();
  delay(300);
  engine.turnRight();
  delay(500);
}
