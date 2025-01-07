#include "ultrasonic.h"
#include "carMotors.h"
#include"lightsensor.h"
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


void setup() { 
  engine.init(0);
  sensor.init();
  Serial.begin(9600);
  Serial.println("Start...");
  Serial.println();
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
          engine.init(50);
        }  
        irrecv.resume(); // Ready to receive the next value
    }
  
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
  if (dcenter < 200){
    engine.stop();
    delay(1500);
    engine.turnLeft();
    delay(1500);
    engine.stop();

  }
  sensor.turnHead(40);
  delay(50);
  float dleft = sensor.getDistance();
  if (dleft < 200){ 
    engine.stop();
    delay(1500);
    engine.turnLeft();
    delay(1500);
    engine.stop();

  }
  sensor.turnHead(90);
  delay(50);
  dcenter = sensor.getDistance();
  if (dcenter < 200){
    engine.stop();
    delay(1500);
    engine.turnLeft();
    delay(1500);
    engine.stop();

  }
  sensor.turnHead(140);
  delay(50);
  float dright = sensor.getDistance();
  if (dright < 200){
    engine.stop();
    delay(1500);
    engine.turnLeft();
    delay(1500);
    engine.stop();

  }
  else return 2;
}
