#include <esp_now.h>
#include <WiFi.h>

// Define the receiver MAC address (replace with the MAC address of your receiver ESP32)
uint8_t receiverMacAddress[] = { 0xCC, 0xDB, 0xA7, 0x2E, 0x95, 0x9C };

// Data structure to hold the joystick data for both joysticks
typedef struct joystick_message {
  int xValLeft;
  int yValLeft;
  bool buttonValLeft;
  int xValRight;
  int yValRight;
  bool buttonValRight;
} joystick_message;

joystick_message joystickData;

// Define pins for the left joystickdigitalWrite(BUZZER_PIN, LOW);
#define Y_LEFT 34
#define X_LEFT 35
#define SWITCH_LEFT 2

// Define pins for the right joystick
#define Y_RIGHT 32
#define X_RIGHT 33
#define SWITCH_RIGHT 4

// **Buzzer Pin Definition**
#define BUZZER_PIN 12

void setup() {
  Serial.begin(115200);

  // Set pin modes for both joysticks
  pinMode(X_LEFT, INPUT);
  pinMode(Y_LEFT, INPUT);
  
  // **Configure Buttons with Internal Pull-Up Resistors**
  pinMode(SWITCH_LEFT, INPUT_PULLUP);
  
  pinMode(X_RIGHT, INPUT);
  pinMode(Y_RIGHT, INPUT);
  
  // **Configure Right Button with Internal Pull-Up Resistor**
  pinMode(SWITCH_RIGHT, INPUT_PULLUP);
  
  // **Initialize Buzzer Pin as Output**
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW); // Ensure buzzer is off initially

  // Initialize WiFi and ESP-NOW
  initializeWiFi();

  // Print the MAC addresses
  printMacAddresses();

  // Initialize ESP-NOW
  initializeESPNow();

  // Add peer for communication
  addPeer();
}

void loop() {
  int xValLeft = 0, yValLeft = 0;
  bool buttonValLeft = false;
  int xValRight = 0, yValRight = 0;
  bool buttonValRight = false;

  // Read values from both joysticks
  readJoystickLeft(&xValLeft, &yValLeft, &buttonValLeft);
  readJoystickRight(&xValRight, &yValRight, &buttonValRight);

  // Store the values in the joystickData structure
  joystickData.xValLeft = xValLeft;
  joystickData.yValLeft = yValLeft;
  joystickData.buttonValLeft = buttonValLeft;
  joystickData.xValRight = xValRight;
  joystickData.yValRight = yValRight;
  joystickData.buttonValRight = buttonValRight;

  // Print joystick data
  printJoystickInfo(xValLeft, yValLeft, buttonValLeft, xValRight, yValRight, buttonValRight);

  // **Check for Maximum Joystick Values and Buzz if Needed**
  digitalWrite(BUZZER_PIN, HIGH);
  // if (isMaxValue(xValLeft) || isMaxValue(yValLeft) ||
  //     isMaxValue(xValRight) || isMaxValue(yValRight)) {
  //   buzz();
  // }

  // Send the joystick data
  // sendData();

  delay(200); // Wait before sending the next data
}

void initializeWiFi() {
  WiFi.mode(WIFI_STA);  // Set Wi-Fi to Station mode
  delay(100);  // Ensure Wi-Fi is initialized before ESP-NOW
}

void printMacAddresses() {
  // Print the receiver's MAC address
  Serial.print("Receiver MAC Address: ");
  for (int i = 0; i < 6; i++) {
    if (i < 5) {
      Serial.print(receiverMacAddress[i], HEX);
      Serial.print(":");
    } else {
      Serial.println(receiverMacAddress[i], HEX);
    }
  }

  // Print the ESP32's own MAC address
  Serial.print("ESP32 MAC Address: ");
  uint8_t macAddress[6];
  WiFi.macAddress(macAddress);
  for (int i = 0; i < 6; i++) {
    if (i < 5) {
      Serial.print(macAddress[i], HEX);
      Serial.print(":");
    } else {
      Serial.println(macAddress[i], HEX);
    }
  }
}

void initializeESPNow() {
  if (esp_now_init() != ESP_OK) {
    Serial.println("ESP-NOW Initialization failed");
    return;
  }
}

void addPeer() {
  // Initialize peer info and set it to zero
  esp_now_peer_info_t peerInfo;
  memset(&peerInfo, 0, sizeof(peerInfo));
  
  // Set MAC address, channel, and encryption
  memcpy(peerInfo.peer_addr, receiverMacAddress, 6);
  peerInfo.channel = 0;  // Default channel
  peerInfo.encrypt = false;

  // Validate and add peer
  if (isValidMacAddress(receiverMacAddress)) {
    esp_err_t addStatus = esp_now_add_peer(&peerInfo);
    if (addStatus == ESP_OK) {
      Serial.println("Peer added successfully");
    } else {
      Serial.print("Failed to add peer, error code: ");
      Serial.println(addStatus);
    }
  } else {
    Serial.println("Invalid MAC address");
  }
}

void sendData() {
  // Send the current joystick data to the receiver
  esp_err_t result = esp_now_send(receiverMacAddress, (uint8_t *)&joystickData, sizeof(joystickData));

  if (result == ESP_OK) {
    Serial.println("Data sent successfully");
  } else {
    Serial.println("Error sending data");
  }
}

void readJoystickLeft(int* xValLeft, int* yValLeft, bool* buttonValLeft) {
  *xValLeft = analogRead(X_LEFT);
  *yValLeft = analogRead(Y_LEFT);
  // **Read button with INPUT_PULLUP logic (active LOW)**
  *buttonValLeft = !digitalRead(SWITCH_LEFT); // Invert if button is active LOW
}

void readJoystickRight(int* xValRight, int* yValRight, bool* buttonValRight) {
  *xValRight = analogRead(X_RIGHT);
  *yValRight = 4095 - analogRead(Y_RIGHT);
  // **Read button with INPUT_PULLUP logic (active LOW)**
  *buttonValRight = !digitalRead(SWITCH_RIGHT); // Invert if button is active LOW
}

void printJoystickInfo(int xValLeft, int yValLeft, bool buttonValLeft, int xValRight, int yValRight, bool buttonValRight) {
  // Print full joystick data for both joysticks
  Serial.print("Left Joystick - X: ");
  Serial.print(xValLeft);
  Serial.print(", Y: ");
  Serial.print(yValLeft);
  Serial.print(", Button: ");
  Serial.print(buttonValLeft ? "Pressed" : "Released");

  Serial.print(" | Right Joystick - X: ");
  Serial.print(xValRight);
  Serial.print(", Y: ");
  Serial.print(yValRight);
  Serial.print(", Button: ");
  Serial.println(buttonValRight ? "Pressed" : "Released");
}

// Function to check if a MAC address is valid (non-zero address)
bool isValidMacAddress(uint8_t *macAddress) {
  for (int i = 0; i < 6; i++) {
    if (macAddress[i] == 0) {
      return false;
    }
  }
  return true;
}

// **Function to Buzz the Buzzer**
void buzz() {
  digitalWrite(BUZZER_PIN, HIGH); // Turn buzzer on
  delay(100);                     // Buzz for 100 milliseconds
  digitalWrite(BUZZER_PIN, LOW);  // Turn buzzer off
}

// **Helper Function to Check for Maximum Joystick Value**
bool isMaxValue(int value) {
  return value >= 4095; // Adjust threshold if necessary
}
