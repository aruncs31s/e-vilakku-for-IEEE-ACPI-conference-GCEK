#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <espnow.h>

// `48:3F:DA:47:80:41`
// New mac `C4:D8:D5:12:C6:03`
uint8_t broadcastAddress[] = {0xC4, 0xD8, 0xD5, 0x12, 0xC6, 0x03};

#define BUTTON_PIN D5

int button_pressed_count = 0;
// Structure example to send data
// Must match the receiver structure
typedef struct struct_message {
  int count;
} struct_message;

struct_message myData;

unsigned long lastTime = 0;
unsigned long timerDelay = 2000;

// Callback when data is sent
void OnDataSent(uint8_t *mac_addr, uint8_t sendStatus) {
  Serial.print("Last Packet Send Status: ");
  if (sendStatus == 0) {
    Serial.println("Delivery success");
  } else {
    Serial.println("Delivery fail");
  }
}
void IRAM_ATTR button_isr() {
  Serial.println("Button pressed!");
  button_pressed_count += 1;
}

void setup() {
  // Init Serial Monitor
  Serial.begin(115200);

  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  attachInterrupt(BUTTON_PIN, button_isr, FALLING);
  // Init ESP-NOW
  if (esp_now_init() != 0) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  esp_now_set_self_role(ESP_NOW_ROLE_CONTROLLER);
  esp_now_register_send_cb(OnDataSent);

  // Register peer
  esp_now_add_peer(broadcastAddress, ESP_NOW_ROLE_SLAVE, 1, NULL, 0);
}

void loop() {
  myData.count = button_pressed_count / 2;

  if ((millis() - lastTime) > timerDelay) {

    esp_now_send(broadcastAddress, (uint8_t *)&myData, sizeof(myData));

    lastTime = millis();
  }
  delay(50);
}
