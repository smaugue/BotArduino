#include <Servo.h>

Servo myServo;

class UltraSonic {
  private:
  const int servo_pin = 10; // port for the servo-motor
  const int echo_pin = 12; // port for reading 
  const int trig_pin = 13; // port for writing
  public:
  /* Initialization of ports */
    UltraSonic() {
           
    }

    void init() {
      pinMode(echo_pin, INPUT);    
      pinMode(trig_pin, OUTPUT);  

      myServo.attach(servo_pin,500,2400);
      turnHead(90);
    }

  /* Returns the distance of the obstacle in millimeters
   */
    float getDistance() {
      digitalWrite(trig_pin,LOW);   
      delayMicroseconds(2);
      digitalWrite(trig_pin,HIGH);  
      delayMicroseconds(10);
      digitalWrite(trig_pin,LOW);   
      float d = pulseIn(echo_pin, HIGH)/5.8f;   
      return d;
    }

    // turn the head to reach angle position in degree
    // angle: in degree (0=left, 90=middle, 180=right)
    void turnHead(unsigned int angle) {
      myServo.attach(servo_pin);
      myServo.write(angle); // sets the servo position / 90 (middle)
      delay(450);
      myServo.detach();
    }

    bool obstacl() {
      float d = getDistance()/10;
      if (d > 5) {
        return true;
      };
      return false;
    }
};
