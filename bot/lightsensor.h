/*
SAE 15 Challenge 3
Florian PISTOLET
Ramire MOLDES-NAAL
*/

#define LIGHTSENSOR_H

/*
Class pour gérer les capteurs de lumière:
PIN: A0 droite, A1 centre, A2 gauche
*/
class LightSensor {
private:
    uint8_t port; //(A0 droite, A1 centre, A2 gauche)
    int default_sensibility = 650; //en plein jour

public:
    LightSensor(uint8_t portHardware) : port(portHardware) {}

    /*
    Fonction pour détecter les ligne
    */
    bool line(int sensibility) {
        int state = analogRead(port);
        if (sensibility == 0) sensibility == default_sensibility;
        return (state > sensibility);
    }

    /*
    Fonction pour renvoyer la valeur brute enregistré par le capteur
    */
    int readValue() {
        return analogRead(port);
    }
};