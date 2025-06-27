#include <Arduino.h>
#include <WiFi.h>
#include <esp_now.h>

const int NUM_LEDS = 5;
const int led_pins[NUM_LEDS] = {23, 22, 21, 19, 18};

// Same as Sender
typedef struct struct_message {
  int count = 0;
} struct_message;

volatile struct_message myData;
volatile bool newDataAvailable = false;
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

  // set flag to indicate that, new data is available
  newDataAvailable = true;
}

void updateRelays() {
  Serial.print("Updating relays for state: ");
  Serial.println(myData.count);

  // First, turn all relays ON (by sending  LOW signal).
  /* NOTE: Why initially turning them on.
   * The light is to be kept ON until the program ends, which is about 9 hours.
   * The battery will not last that long, so when the esp32 dies we have to keep
   * the relays ON.
   */

  // This ensures a clean slate for every command.
  for (int i = 0; i < NUM_LEDS; i++) {
    digitalWrite(led_pins[i], LOW);
  }

  // Turn on the led in an incremental order
  if (myData.count > 0 && myData.count <= NUM_LEDS) {
    for (int i = 0; i < myData.count; i++) {
      digitalWrite(led_pins[i], HIGH); // Turn OFF the relay (NC mode)
      Serial.println("  -> Turning OFF relay " + String(i + 1));
    }
  } else if (myData.count > NUM_LEDS) {
    // if count is > total number of relays, then turn them all on.
    for (int i = 0; i < NUM_LEDS; i++) {
      digitalWrite(led_pins[i], HIGH);
    }
    Serial.println("  -> Turning ON ALL relays (count > NUM_LEDS).");
  }
}

void setup() {
  Serial.begin(115200);
  Serial.println("ESP32 ESP-NOW Relay Receiver");

  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

  Serial.print("This Mac: ");
  Serial.println(WiFi.macAddress());

  // Use a loop to initialize the pins.
  for (int i = 0; i < NUM_LEDS; i++) {
    pinMode(led_pins[i], OUTPUT);
    digitalWrite(led_pins[i], LOW); // Initialize all relays to OFF.
  }
  delay(50);
  // Initialize ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  esp_now_register_recv_cb(OnDataRecv);
}

void loop() {
  // Check if new data is available.
  if (newDataAvailable) {
    newDataAvailable = false;
    updateRelays();
  }
}
