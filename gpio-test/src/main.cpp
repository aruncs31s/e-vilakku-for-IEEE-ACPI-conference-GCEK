#include <Arduino.h>
void setup() {
  // Initialize serial communication at 9600 baud rate
  Serial.begin(115200);
}

void loop() {
  Serial.println(analogRead(A0)); // Corrected typo from Serail to Serial
  delay(100);
}
