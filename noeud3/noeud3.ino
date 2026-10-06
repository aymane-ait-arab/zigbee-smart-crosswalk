// ==========================================
// NODE 3: ARDUINO UNO - FEU + BOUTON (9600 BAUD)
// ==========================================

#include <SoftwareSerial.h>

SoftwareSerial xbee(2, 3); // RX, TX

const int BUTTON  = 4;
const int LED_RED = 5;
const int LED_YEL = 6;
const int LED_GRN = 7;

unsigned long lastPress = 0;

void setup() {
  pinMode(BUTTON, INPUT_PULLUP);
  pinMode(LED_RED, OUTPUT);
  pinMode(LED_YEL, OUTPUT);
  pinMode(LED_GRN, OUTPUT);

  xbee.begin(9600);

  setLights(LOW, LOW, HIGH); // Vert au départ
}

void loop() {
  
  if (digitalRead(BUTTON) == LOW && millis() - lastPress > 300) {
    lastPress = millis();
    xbee.print('Z'); // Envoie au coordinateur
  }

  if (xbee.available() > 0) {
    char command = xbee.read();

    if (command == 'Y') {
      setLights(LOW, HIGH, LOW);
    } else if (command == 'R') {
      setLights(HIGH, LOW, LOW);
    } else if (command == 'G') {
      setLights(LOW, LOW, HIGH);
    }
  }
}

void setLights(int red, int yellow, int green) {
  digitalWrite(LED_RED, red);
  digitalWrite(LED_YEL, yellow);
  digitalWrite(LED_GRN, green);
}