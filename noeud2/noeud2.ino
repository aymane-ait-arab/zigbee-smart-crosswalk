// ==========================================
// NODE 2: ARDUINO UNO - FEU + BOUTON + MATRICE (9600 BAUD)
// ==========================================

#include <SoftwareSerial.h>
#include "LedControl.h"

// XBee via SoftwareSerial (garde le port USB libre)
SoftwareSerial xbee(2, 3); // RX, TX

const int BUTTON  = 4;
const int LED_RED = 5;
const int LED_YEL = 6;
const int LED_GRN = 7;

// Matrice MAX7219 : DIN, CLK, CS
LedControl lc = LedControl(10, 11, 12, 4);

// Animation "piéton qui marche" (2 frames)
byte person1[8] = {
  0b00011000,
  0b00011000,
  0b00011000,
  0b01111110,
  0b00011000,
  0b00100100,
  0b01000010,
  0b10000001
};
byte person2[8] = {
  0b00011000,
  0b00011000,
  0b00011000,
  0b00111100,
  0b00011000,
  0b00011000,
  0b00100100,
  0b01000010
};

bool crossing = false;      // true pendant le rouge (matrice active)
unsigned long lastFrame = 0;
bool frameToggle = false;

// Anti-rebond bouton
unsigned long lastPress = 0;

void setup() {
  pinMode(BUTTON, INPUT_PULLUP);
  pinMode(LED_RED, OUTPUT);
  pinMode(LED_YEL, OUTPUT);
  pinMode(LED_GRN, OUTPUT);

  xbee.begin(9600);

  lc.shutdown(0, false);
  lc.setIntensity(0, 8);
  lc.clearDisplay(0);

  setLights(LOW, LOW, HIGH); // Vert au départ
}

void loop() {
  if (digitalRead(BUTTON) == LOW) {
  Serial.println("BOUTON PRESSE"); // Utilise Serial (USB), pas xbee, juste pour tester
}
  // --- Lecture du bouton avec anti-rebond ---
  if (digitalRead(BUTTON) == LOW && millis() - lastPress > 300) {
    lastPress = millis();
    xbee.print('X'); // Envoie au coordinateur
  }

  // --- Réception des commandes du coordinateur ---
  if (xbee.available() > 0) {
    char command = xbee.read();

    if (command == 'Y') {
      setLights(LOW, HIGH, LOW);
    } else if (command == 'R') {
      setLights(HIGH, LOW, LOW);
      crossing = true; // Démarre l'animation
    } else if (command == 'G') {
      setLights(LOW, LOW, HIGH);
      crossing = false;
      lc.clearDisplay(0); // Éteint la matrice
    }
  }

  // --- Animation non-bloquante de la matrice pendant le rouge ---
  if (crossing && millis() - lastFrame > 200) {
    lastFrame = millis();
    frameToggle = !frameToggle;
    byte* frame = frameToggle ? person1 : person2;
    for (int row = 0; row < 8; row++) {
      lc.setRow(0, row, frame[row]);
    }
  }
}

void setLights(int red, int yellow, int green) {
  digitalWrite(LED_RED, red);
  digitalWrite(LED_YEL, yellow);
  digitalWrite(LED_GRN, green);
}