#include <Arduino.h>
#include <Wire.h> // I2C Communication Library

// The I2C address for this slave device. Must match the master's code.
#define SLAVE_ADDRESS 8

// CRITICAL PIN CHANGE:
// Pins D1 and D2 are now used for I2C communication (SCL and SDA).
// We cannot use them for relays anymore. The pins have been moved.
const int NUM_LEDS = 5;
const int led_pins[NUM_LEDS] = {D3, D4, D5, D6, D7};

// This flag will be set to true when new data arrives from the master.
// 'volatile' is crucial because this is modified by an interrupt (I2C event).
volatile bool newDataAvailable = false;
volatile int received_count = 0;

// This function is an event handler that runs when data is received from the
// Master. It is called by an interrupt, so it should be fast.
void receiveEvent(int howMany) {
  if (Wire.available() > 0) {
    received_count = Wire.read(); // Read the byte from the I2C buffer.
    newDataAvailable = true;      // Set the flag for the main loop.
  }
}

// This function updates the relays based on the current count.
void updateRelays() {
  Serial.print("Updating relays for state: ");
  Serial.println(received_count);

  // First, turn all relays OFF (by sending a HIGH signal).
  for (int i = 0; i < NUM_LEDS; i++) {
    digitalWrite(led_pins[i], HIGH);
  }

  // Turn on lights from 1 up to the received count.
  if (received_count > 0 && received_count <= NUM_LEDS) {
    for (int i = 0; i < received_count; i++) {
      digitalWrite(led_pins[i], LOW); // Turn relay ON (LOW signal)
      Serial.println("  -> Turning ON relay " + String(i + 1));
    }
  } else if (received_count > NUM_LEDS) {
    // If count is higher than our number of relays, turn them all on.
    for (int i = 0; i < NUM_LEDS; i++) {
      digitalWrite(led_pins[i], LOW);
    }
    Serial.println("  -> Turning ON ALL relays (count > NUM_LEDS).");
  }
}

void setup() {
  Serial.begin(115200);
  Serial.println("I2C Slave Receiver");

  // Use a loop to initialize the new relay pins.
  for (int i = 0; i < NUM_LEDS; i++) {
    pinMode(led_pins[i], OUTPUT);
    digitalWrite(led_pins[i], HIGH); // Initialize all relays to OFF.
  }

  // Initialize I2C communication as Slave and join the I2C bus.
  Wire.begin(SLAVE_ADDRESS);
  // Register the event handler function that will be called on data reception.
  Wire.onReceive(receiveEvent);
}

void loop() {
  // The main loop just checks if new data has arrived.
  if (newDataAvailable) {
    newDataAvailable = false; // Reset the flag.
    updateRelays();           // Call the function to update the relays.
  }
  // No delay needed. The logic is event-driven.
}
