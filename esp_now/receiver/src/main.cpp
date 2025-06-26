#include <Arduino.h>
#include <WiFi.h>
#include <esp_now.h>

const int NUM_LEDS = 5;
const int led_pins[NUM_LEDS] = {23, 22, 21, 19,
                                18}; // Example GPIO pins for ESP32

typedef struct struct_message {
  int count = 0;
} struct_message;

volatile struct_message myData;
volatile bool newDataAvailable = false;
// Callback function that will be executed when ESP-NOW data is received
void OnDataRecv(const uint8_t *mac_addr, const uint8_t *incomingData, int len) {
  // Create a temporary, non-volatile structure to copy the data into first.
  struct_message temp_data;
  memcpy(&temp_data, incomingData, sizeof(temp_data));

  // Now, assign the temporary data to the volatile global variable.
  // This is a safe operation.
  myData.count = temp_data.count;
  Serial.print("Data received. Count: ");
  Serial.println(myData.count);
  Serial.print("From MAC Address: ");
  char macStr[18];
  snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x", mac_addr[0],
           mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
  Serial.println(macStr);

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
  Serial.println("ESP32 ESP-NOW Relay Receiver");

  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

  // Get and print the MAC address of this receiver.
  // You MUST use this address in the sender's code.
  Serial.print("Receiver MAC Address: ");
  Serial.println(WiFi.macAddress());

  // Use a loop to initialize the pins.
  for (int i = 0; i < NUM_LEDS; i++) {
    pinMode(led_pins[i], OUTPUT);
    digitalWrite(led_pins[i], HIGH); // Initialize all relays to OFF.
  }

  // Initialize ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Register the receive callback function.
  // The role is automatically determined. No need for esp_now_set_self_role()
  // on ESP32.
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
