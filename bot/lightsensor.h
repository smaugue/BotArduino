#define LIGHTSENSOR_H

class LightSensor {
private:
    uint8_t port; //(A0 droite, A1, A2)

public:
    LightSensor(uint8_t portHardware) : port(portHardware) {}

    bool line() {
        int state = analogRead(port);
        if (state > 600) {
          return true;
        }
        else return false;
    }

    int read() {
        return analogRead(port);
    }
};