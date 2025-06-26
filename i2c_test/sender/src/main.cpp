#include <Arduino.h>
#include <Wire.h> // I2C Communication Library

// The I2C address of the slave ESP8266. This must match the slave's code.
#define SLAVE_ADDRESS 8

// Pin for the button.
#define BUTTON_PIN D5

// Volatile variables used in the Interrupt Service Routine (ISR).
volatile int button_press_count = 0;
volatile bool button_pressed_flag = false;
unsigned long last_interrupt_time = 0;

// This is the Interrupt Service Routine (ISR) that runs when the button is
// pressed. It should be as fast as possible.
void IRAM_ATTR button_isr() {
  // Debounce the button: only register a press if enough time has passed.
  if (millis() - last_interrupt_time > 200) {
    button_press_count++;
    // Cycle the count from 0 to 5.
    if (button_press_count > 5) {
      button_press_count = 0;
    }
    button_pressed_flag = true; // Set a flag for the main loop to handle.
    last_interrupt_time = millis();
  }
}

void setup() {
  Serial.begin(115200);
  Serial.println("I2C Master Sender");

  // Initialize I2C communication as Master.
  // Default I2C pins for ESP8266 are D1 (SCL) and D2 (SDA).
  Wire.begin();

  // Set the button pin and attach the interrupt.
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), button_isr, FALLING);
}

void loop() {
  // Check if the button ISR has set the flag.
  if (button_pressed_flag) {
    button_pressed_flag = false; // Reset the flag.

    Serial.print("Button pressed. Sending count: ");
    Serial.println(button_press_count);

    // Begin transmission to the slave device.
    Wire.beginTransmission(SLAVE_ADDRESS);
    // Send the count value as a single byte.
    Wire.write(button_press_count);
    // End the transmission.
    Wire.endTransmission();
  }
  // Small delay to keep the system stable.
  delay(50);
}
