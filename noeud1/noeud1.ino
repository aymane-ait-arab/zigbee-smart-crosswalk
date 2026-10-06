// ==========================================
// NODE 1: ESP32-S3 - COORDINATEUR (9600 BAUD)
// ==========================================

#define RX1_PIN 18
#define TX1_PIN 17

bool busy = false; // Empêche un nouveau déclenchement pendant une traversée en cours

void setup() {
  Serial1.begin(9600, SERIAL_8N1, RX1_PIN, TX1_PIN);
}

void loop() {
  if (Serial1.available() > 0) {
    char command = Serial1.read();

    // Un des deux boutons a été pressé, et le système n'est pas déjà occupé
    if ((command == 'X' || command == 'Z') && !busy) {
      busy = true;

      Serial1.print('Y');   // Jaune : avertissement sur les 2 nœuds
      delay(2000);

      Serial1.print('R');   // Rouge : piéton traverse (+ matrice sur Nœud 2)
      delay(7000);

      Serial1.print('G');   // Vert : circulation reprise
      delay(3000);          // Cooldown de sécurité

      busy = false;
    }
  }
}