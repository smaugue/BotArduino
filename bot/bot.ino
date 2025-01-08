/*
SAE 15 Challenge 3
Florian PISTOLET
Ramire MOLDES-NAAL
*/

// Importation des class bibliothèques nécessaires
#include "ultrasonic.h"
#include "carMotors.h"
#include "lightsensor.h"
#include <IRremote.h>       
#include <FastLED.h>
    
#define BASE_SPEED 80 // Vitesse de base des moteurs

// Variables principales
const int RGBPin = 4; // Broche de connexion pour la LED RGB       
const int receiverPin = 9; // Broche de connexion pour le récepteur IR
const int numLEDs = 1; // Nombre de LEDs
CRGB leds[numLEDs];
uint32_t tabColors[]= {0xFF0000,0xFB4C0D,0x00FF00}; // Couleurs pour les LEDs (rouge, orange, vert)
decode_results results;
unsigned long previousWallCheckTime = 0; // Dernière exécution de checkwall
const unsigned long wallCheckInterval = 75; // Intervalle pour exécuter checkwall (ms)
bool isButtonOkPressed = false; // État du bouton "OK" (activé ou non)
int sensibility = 0; // Sensibilité des capteurs de lumière
bool obs = false; // Indicateur de présence d'un obstacle utiliser par le checkwall pour neutraliser les autres processus

// Déclaration des objets
CarMotors engine; // Contrôle des moteurs
LightSensor right(A0); // Capteur de lumière à droite
LightSensor center(A1); // Capteur de lumière au centre
LightSensor left(A2); // Capteur de lumière à gauche
UltraSonic sensor; // Capteur ultrasonique pour détection de distance
IRrecv irrecv(receiverPin); // Récepteur infrarouge

void setup() {
  // Initialisation des capteurs et moteurs
  sensor.init();
  engine.init(0);
  
  // Initialisation de la communication série
  Serial.begin(9600);
  Serial.println("Robot Start...");
  
  // Initialisation du récepteur infrarouge et des LEDs
  irrecv.enableIRIn();
  FastLED.addLeds<NEOPIXEL, RGBPin>(leds, numLEDs);
  FastLED.setBrightness(20);
  FastLED.showColor(tabColors[0]); // Affichage de la couleur rouge par défaut
}

// Gestion de la télécommande et boucle de régulation
void loop() {
  // Gestion de la télécommande
  if (irrecv.decode(&results)) { // Décoder les signaux infrarouges
    unsigned long int codeC = results.value;
    if (codeC == 0xFF02FD) { // Code correspondant au bouton "OK"
      isButtonOkPressed = true; // Activation de la boucle principale
      engine.setSpeed(BASE_SPEED); // Lancer les moteurs
      FastLED.showColor(tabColors[2]); // Afficher la couleur verte
    }
    if (codeC == 0xff42bd && !isButtonOkPressed) { // Code pour la calibration avec le bouton "*"
      FastLED.showColor(tabColors[1]); // Afficher la couleur orange pendant la calibration
      // Lecture des valeurs des capteurs de lumière
      int c = center.readValue();
      int r = right.readValue();
      int l = left.readValue();
      sensibility = (c + r + l) / 3 - 175; // Calcul de la sensibilité avec une marge
      FastLED.showColor(tabColors[0]); // Revenir à la couleur rouge après calibration
    }
    irrecv.resume(); // Réinitialiser le récepteur IR
  }

  // Boucle de régulation uniquement si le bouton "OK" est activé
  if (isButtonOkPressed) {
    float turnCoef = 0.15;//utiliser pour empècher le robot de tourner en rond
    // Appel périodique de checkwall pour vérifier les murs
    unsigned long currentTime = millis();
    if (currentTime - previousWallCheckTime >= wallCheckInterval) {
      previousWallCheckTime = currentTime;
      checkwall(); // Vérification des obstacles autour
    }

    // Si une ligne est détectée
    if (checkline()) {
      turnCoef = 0.15; // réinitialisation du coef
      avoidObstacle(); // Contourner l'obstacle
    }
    // Sinon, continuer d'avancer
    else if (!obs) engine.drive(1, turnCoef); turnCoef += 0.02; // Avancer en tournant et augmentation du coeff
  }
}

// Vérifie la présence d'une ligne
bool checkline() {
  if (center.line(sensibility) || left.line(sensibility) || right.line(sensibility)) {
    engine.stop(); // Arrêter le moteur
    return true; // Ligne détectée
  }
  else {
    return false;
  } // Pas de ligne détectée
}

// Vérifie la présence d'un mur
void checkwall() {
  static int state = 0; // État actuel de la vérification des murs
  static unsigned long lastTime = 0; // Dernière exécution de l'état actuel
  float dcenter, dleft, dright; // Distances mesurées

  switch (state) {
    case 0: // Vérifier l'obstacle au centre
      sensor.turnHead(90);
      dcenter = sensor.getDistance();
      if (dcenter < 200) { // Si un obstacle est détecté à moins de 200 mm
        engine.stop(); // Arrêter le moteur
        obs = true; // Marquer la présence d'un obstacle
        state = 1; // Passer à l'état pour éviter l'obstacle
        lastTime = millis();
      } else {
        state = 3; // Sinon, vérifier le côté gauche
      }
      break;

    case 1: // Éviter l'obstacle en reculant
      if (millis() - lastTime > 200) { // Reculer pendant 300 ms
        engine.goBackward();
        lastTime = millis();
        state = 2; // Passer à l'état suivant
      }
      break;

    case 2: // Tourner à gauche pour éviter l'obstacle
      if (millis() - lastTime > 1500) { // Tourner pendant 1,5 seconde
        engine.turnRight();
        lastTime = millis();
        obs = false; // Obstacle évité
        state = 0; // Revenir à l'état initial
      }
      break;

    case 3: // Vérifier l'obstacle à gauche
      sensor.turnHead(40);
      dleft = sensor.getDistance();
      if (dleft < 200) { // Si un obstacle est détecté à gauche
        obs = true; // Marquer la présence d'un obstacle
        engine.stop();
        state = 1; // Passer à l'état pour éviter l'obstacle
        lastTime = millis();
      } else {
        state = 4; // Sinon, vérifier le côté droit
      }
      break;

    case 4: // Vérifier l'obstacle à droite
      sensor.turnHead(140);
      dright = sensor.getDistance();
      if (dright < 200) { // Si un obstacle est détecté à droite
        obs = true; // Marquer la présence d'un obstacle
        engine.stop();
        state = 1; // Passer à l'état pour éviter l'obstacle
        lastTime = millis();
      } else {
        state = 0; // Sinon, revenir à l'état initial
      }
      break;
  }
}

// Fonction pour gérer l'évitement des obstacles
void avoidObstacle() {
  engine.stop(); // Arrêter les moteurs
  engine.goBackward(); // Reculer
  delay(300); // Attendre 300 ms
  engine.turnRight(); // Tourner à droite
  delay(150); // Attendre 150 ms
}
