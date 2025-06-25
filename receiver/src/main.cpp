#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <cstdint>
#include <espnow.h>

const int led_pins[5] = {D1, D2, D5, D6, D7};
bool led_state[5] = {false, false, false, false, false};

// Structure example to receive data
// Must match the sender structure
typedef struct struct_message {
  int count = 0;
} struct_message;

// Create a struct_message called myData
struct_message myData;

// Callback function that will be executed when data is received
void OnDataRecv(uint8_t *mac, uint8_t *incomingData, uint8_t len) {
  memcpy(&myData, incomingData, sizeof(myData));
  Serial.print("Button Press Count: ");
  Serial.println(myData.count);
}

void setup() {
  // Initialize Serial Monitor
  Serial.begin(115200);

  pinMode(led_pins[0], OUTPUT);
  // Turn off all LEDs initially
  pinMode(led_pins[0], HIGH);
  Serial.print("LED Pin " + String(0 + 1) + " initialized ");
  pinMode(led_pins[1], OUTPUT);
  // Turn off all LEDs initially
  pinMode(led_pins[1], HIGH);
  Serial.print("LED Pin " + String(1 + 1) + " initialized ");
  pinMode(led_pins[2], OUTPUT);
  // Turn off all LEDs initially
  pinMode(led_pins[2], HIGH);
  Serial.print("LED Pin " + String(2 + 1) + " initialized ");
  pinMode(led_pins[3], OUTPUT);
  // Turn off all LEDs initially
  pinMode(led_pins[3], HIGH);
  Serial.print("LED Pin " + String(3 + 1) + " initialized ");
  pinMode(led_pins[4], OUTPUT);
  // Turn off all LEDs initially
  pinMode(led_pins[4], HIGH);
  Serial.print("LED Pin " + String(4 + 1) + " initialized ");
  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

  // Init ESP-NOW
  if (esp_now_init() != 0) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  esp_now_set_self_role(ESP_NOW_ROLE_SLAVE);
  esp_now_register_recv_cb(OnDataRecv);
}

// TODO: Check if Reset Button Is needed
void loop() {

  switch (myData.count) {
  case 0:
    Serial.println("No Lights are on");
    break;
  case 1:
    Serial.println("Light 1 is on");
    Serial.println("Pin: " + String(D1) + " is LOW");
    if (led_state[0] == false) {
      digitalWrite(led_pins[myData.count - 1], LOW);
      led_state[0] = true;
    }
    break;
  case 2:
    Serial.println("Light 2 is on");
    Serial.println("Pin: " + String(D2) + " is LOW");
    if (led_state[1] == false) {
      digitalWrite(led_pins[myData.count - 1], LOW);
      led_state[1] = true;
    }
    break;
  case 3:
    Serial.println("Light 3 is on");

    Serial.println("Pin: " + String(D5) + " is LOW");
    if (led_state[2] == false) {
      digitalWrite(led_pins[myData.count - 1], LOW);
      led_state[2] = true;
    }
    break;
  case 4:
    Serial.println("Light 4 is on");
    Serial.println("Pin: " + String(D6) + " is LOW");
    if (led_state[3] == false) {
      digitalWrite(led_pins[myData.count - 1], LOW);
      led_state[3] = true;
    }
    break;
  case 5:
    Serial.println("Light All lights are  on");
    Serial.println("Pin: " + String(D7) + " is LOW");
    if (led_state[4] == false) {
      digitalWrite(led_pins[myData.count - 1], LOW);
      led_state[4] = true;
    }
    break;
  default:
    Serial.println("Invalid count");
    break;
  }
  delay(50);
}
