#include <esp_now.h>
#include <WiFi.h>

// Data structure to hold the joystick data for both joysticks
typedef struct joystick_message {
  int xValLeft;
  int yValLeft;
  bool buttonValLeft;
  int xValRight;
  int yValRight;
  bool buttonValRight;
} joystick_message;

// Create a variable to hold received data
volatile bool dataReceived = false;
joystick_message receivedData;

// Variable to track the last time data was received
unsigned long lastReceiveTime = 0;

// Callback function to handle incoming data
void onDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len) {
  if (len == sizeof(joystick_message)) {
    memcpy(&receivedData, incomingData, sizeof(receivedData));
    dataReceived = true;
    lastReceiveTime = millis();
  }
}

void setup() {
  Serial.begin(115200);
  delay(1000); // Allow time for the Serial Monitor to initialize

  // Initialize WiFi in station mode
  WiFi.mode(WIFI_STA);
  Serial.println("ESP-NOW Receiver Initialized");

  // Initialize ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("ESP-NOW Initialization Failed");
    while (1) {
      delay(1000); // Halt if ESP-NOW fails to initialize
    }
  }

  // Register the receive callback
  esp_now_register_recv_cb(onDataRecv);
}

void loop() {
  const unsigned long currentTime = millis();
  const unsigned long timeout = 2000; // 2 seconds timeout

  if (dataReceived) {
    // Print the received joystick data
    Serial.print("Left Joystick - X: ");
    Serial.print(receivedData.xValLeft);
    Serial.print(", Y: ");
    Serial.print(receivedData.yValLeft);
    Serial.print(", Button: ");
    Serial.print(receivedData.buttonValLeft ? "Pressed" : "Released");

    Serial.print(" | Right Joystick - X: ");
    Serial.print(receivedData.xValRight);
    Serial.print(", Y: ");
    Serial.print(receivedData.yValRight);
    Serial.print(", Button: ");
    Serial.println(receivedData.buttonValRight ? "Pressed" : "Released");

    dataReceived = false; // Reset the flag after processing
  }

  // Check if data has not been received within the timeout period
  if (currentTime - lastReceiveTime > timeout) {
    Serial.println("No data received");
    lastReceiveTime = currentTime; // Reset the timer to prevent continuous printing
  }

  delay(500); // Adjust the delay as needed
}
