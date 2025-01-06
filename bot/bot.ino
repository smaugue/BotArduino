#include "ultrasonic.h"
#include "carMotors.h"
#include "lightsensor.h"

#define BASE_SPEED 50
#define WALL_DISTANCE_THRESHOLD 200 // Seuil de distance pour détecter un mur
#define GRID_SIZE 34                // Taille de la grille
#define CELL_SIZE 10                // Taille d'une cellule en cm
#define SPEED_CONVERSION 0.36       // Conversion vitesse → distance (cm/s)

CarMotors engine;
LightSensor right(A0);
LightSensor center(A1);
LightSensor left(A2);
UltraSonic sensor;

byte grid[GRID_SIZE][GRID_SIZE] = {0}; // Tableau pour la grille (1 octet par cellule)
int robotX = GRID_SIZE / 2; // Position initiale (centre de la grille)
int robotY = GRID_SIZE / 2;

void setup() {
    sensor.init();
    engine.init(BASE_SPEED);
    Serial.begin(9600);
    Serial.println("Start...");
}

void loop() {
    int lineState = checkLine();
    int wallState = checkWall();
    int action = determineAction(lineState, wallState);

    updateGrid();     // Mise à jour de la grille
    executeAction(action); // Exécution de l'action
}

int checkLine() {
    bool rightLine = right.line();
    bool leftLine = left.line();
    bool centerLine = center.line();

    if (rightLine && !leftLine) return 0; // Tourner à gauche
    if (leftLine && !rightLine) return 1; // Tourner à droite
    return 2; // Avancer si aucune ligne détectée
}

int checkWall() {
    float distances[3];

    sensor.turnHead(40); // Capteur à gauche
    delay(50);
    distances[0] = sensor.getDistance();

    sensor.turnHead(90); // Capteur au centre
    delay(50);
    distances[1] = sensor.getDistance();

    sensor.turnHead(140); // Capteur à droite
    delay(50);
    distances[2] = sensor.getDistance();

    if (distances[1] < WALL_DISTANCE_THRESHOLD) {
        markObstacle(distances[1], 0); // Obstacle devant
        return distances[0] > WALL_DISTANCE_THRESHOLD ? 0 : 1; // Gauche ou droite
    }
    if (distances[0] < WALL_DISTANCE_THRESHOLD) {
        markObstacle(distances[0], -1); // Obstacle à gauche
        return 1; // Tourner à droite
    }
    if (distances[2] < WALL_DISTANCE_THRESHOLD) {
        markObstacle(distances[2], 1); // Obstacle à droite
        return 0; // Tourner à gauche
    }

    return 2; // Pas de mur détecté
}

int determineAction(int lineState, int wallState) {
    if (lineState == 2) return wallState;
    if (wallState == 2) return lineState;
    return wallState; // Par défaut, suivre la détection murale
}

void executeAction(int action) {
    switch (action) {
        case 0: // Tourner à gauche
            engine.goBackward();
            delay(100);
            engine.turnLeft();
            delay(100);
            break;
        case 1: // Tourner à droite
            engine.goBackward();
            delay(100);
            engine.turnRight();
            delay(100);
            break;
        case 2: // Avancer
            engine.goForward();
            updatePosition(BASE_SPEED * SPEED_CONVERSION); // Mise à jour position
            break;
    }
}

void updatePosition(float distance) {
    int steps = (int)(distance / CELL_SIZE); // Cellules parcourues
    robotY -= steps; // Avancer dans l'axe Y

    robotX = constrain(robotX, 0, GRID_SIZE - 1);
    robotY = constrain(robotY, 0, GRID_SIZE - 1);
}

void updateGrid() {
    grid[robotY][robotX] = 0; // Position actuelle libre
}

void markObstacle(float distance, int direction) {
    int offsetX = (direction == -1) ? -1 : (direction == 1) ? 1 : 0;
    int offsetY = (direction == 0) ? -1 : 0;

    int obstacleX = robotX + offsetX;
    int obstacleY = robotY + offsetY;

    if (obstacleX >= 0 && obstacleX < GRID_SIZE && obstacleY >= 0 && obstacleY < GRID_SIZE) {
        grid[obstacleY][obstacleX] = 1; // Marquer obstacle
    }
}
