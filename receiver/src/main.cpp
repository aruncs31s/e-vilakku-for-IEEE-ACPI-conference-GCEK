#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <cstdint>
#include <espnow.h>

const int NUM_LEDS = 5;
const int led_pins[NUM_LEDS] = {D1, D2, D5, D6, D7};

typedef struct struct_message {
  int count = 0;
} struct_message;
volatile bool newDataAvailable = false;

volatile struct_message myData;

void OnDataRecv(uint8_t *mac, uint8_t *incomingData, uint8_t len) {
  memcpy((void *)&myData, incomingData, sizeof(myData));

  Serial.print("Data received. Count: ");
  Serial.println(myData.count);

  newDataAvailable = true;
}

void updateRelays() {
  Serial.print("Updating relays for state: ");
  Serial.println(myData.count);

  // First, turn all relays OFF (by sending a HIGH signal).
  // This ensures a clean slate for every command.
  for (int i = 0; i < NUM_LEDS; i++) {
    digitalWrite(led_pins[i], HIGH);
  }

  // The logic is now "turn on lights from 1 up to the received count".
  // For example, if count is 3, lights 1, 2, and 3 will turn on.
  if (myData.count > 0 && myData.count <= NUM_LEDS) {
    for (int i = 0; i < myData.count; i++) {
      digitalWrite(led_pins[i], LOW); // Turn relay ON (LOW signal)

      Serial.println("  -> Turning ON relay " + String(i + 1));
    }
  } else if (myData.count > NUM_LEDS) {
    // A special case: if the count is higher than our number of relays,
    // we can assume it means "turn them all on".
    for (int i = 0; i < NUM_LEDS; i++) {
      digitalWrite(led_pins[i], LOW);
    }
    Serial.println("  -> Turning ON ALL relays (count > NUM_LEDS).");
  }
  // If myData.count is 0, all relays will remain off from the initial loop.
}

void setup() {
  Serial.begin(115200);
  Serial.println("ESP-NOW Relay Receiver");

  // Get and print the MAC address of this receiver.
  // You MUST use this address in the sender's code.
  Serial.print("Receiver MAC Address: ");
  Serial.println(WiFi.macAddress());

  // Use a loop to initialize the pins. It's cleaner.
  for (int i = 0; i < NUM_LEDS; i++) {
    pinMode(led_pins[i], OUTPUT);
    digitalWrite(led_pins[i], HIGH); // Initialize all relays to OFF.
  }

  WiFi.mode(WIFI_STA);

  // Initialize ESP-NOW
  if (esp_now_init() != 0) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Set the role and register the receive callback function.
  esp_now_set_self_role(ESP_NOW_ROLE_SLAVE);
  esp_now_register_recv_cb(OnDataRecv);
}

void loop() {
  // The main loop just checks if new data has arrived.
  if (newDataAvailable) {
    newDataAvailable = false; // Reset the flag immediately.
    updateRelays();           // Call the function to update the relays.
  }
  // No delay is needed. The logic is event-driven.
}
