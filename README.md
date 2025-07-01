# E-Vilaku
1. Find MAC
```cpp
#include <Arduino.h>
#if defined(ESP8266)
#include <ESP8266WiFi.h>
#endif
#if defined(ESP32)
#include <WiFi.h>
#endif
// #include <esp_wifi.h>
#include <ESP8266WiFi.h>

void setup() {
  Serial.begin(115200);
  Serial.println();
  Serial.print("ESP Board MAC Address:  ");
  Serial.println(WiFi.macAddress());
}

void loop() {}
```
2. Sender can be neither esp8266 or esp32
3. Receiver must be esp32 , because esp8266 behaves differently when used as a receiver , also only 5 pins are usable. 
## Schematics
![](./Screenshot%20(37).png?raw=true)