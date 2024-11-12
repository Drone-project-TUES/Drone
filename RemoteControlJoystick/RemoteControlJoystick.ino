#include <esp_now.h>
#include <WiFi.h>

// Define the receiver MAC address (replace with the MAC address of your receiver ESP32)
uint8_t receiverMacAddress[] = { 0xCC, 0xDB, 0xA7, 0x2E, 0x95, 0x9C };

// Data structure to hold the digits
typedef struct struct_message {
  int motor1;
  int motor2;
  int motor3;
  int motor4;
} struct_message;

struct_message myData;

#define Y_LEFT 34
#define X_LEFT 35
#define SWITCH_LEFT 2

void setup() {
  Serial.begin(115200);

  pinMode(X_LEFT, INPUT);    // Set the X-axis pin as input
  pinMode(Y_LEFT, INPUT);    // Set the Y-axis pin as input
  pinMode(SWITCH_LEFT, INPUT);  // Set the button pin as input

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

  // Read joystick values
  readJoystick(&xValLeft, &yValLeft, &buttonValLeft);

  // Set digits based on joystick values
  setDigitsBasedOnJoystick(xValLeft, yValLeft);

  // Print full joystick data
  // printJoystickInfo(xValLeft, yValLeft, buttonValLeft);

  // Send the data
  sendData();

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
  peerInfo.channel = 0;
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
  // Send the current digits to the receiver
  esp_err_t result = esp_now_send(receiverMacAddress, (uint8_t *)&myData, sizeof(myData));

  if (result == ESP_OK) {
    Serial.println("Data sent successfully");
  } else {
    Serial.println("Error sending data");
  }
}

void readJoystick(int* xValLeft, int* yValLeft, bool* buttonValLeft) {
  *xValLeft = analogRead(X_LEFT) / 4;
  *yValLeft = 1023 - analogRead(Y_LEFT) / 4;
  *buttonValLeft = digitalRead(SWITCH_LEFT);
}

void setDigitsBasedOnJoystick(int xValLeft, int yValLeft) {
  // Map the X-axis to -1 to 1 range (middle is 0)
  float xMapped = map(xValLeft, 0, 1023, -1, 1);

    // If X is left (-1), bias to left side (motor1 and motor4)
    // If X is right (1), bias to right side (motor2 and motor3)
  float leftBias = 1 - abs(xMapped);  // Closer to left
  float rightBias = abs(xMapped);    // Closer to right

  myData.motor1 = yValLeft;
  myData.motor2 = yValLeft;
  myData.motor3 = yValLeft;
  myData.motor4 = yValLeft;

  // if (xMapped < 0) {

  //   myData.motor1 = yValLeft * (1+xMapped);
  //   myData.motor2 = yValLeft;
  //   myData.motor3 = yValLeft;
  //   myData.motor4 = yValLeft * (1+xMapped);
  // } else {
  //   myData.motor1 = yValLeft;
  //   myData.motor2 = yValLeft * (1-xMapped);
  //   myData.motor3 = yValLeft * (1-xMapped);
  //   myData.motor4 = yValLeft;
  // }
  
}

void printJoystickInfo(int xValLeft, int yValLeft, bool buttonValLeft) {
  // Print full joystick data: X value, Y value, button state, and the current digits
  Serial.print("Joystick X: ");
  Serial.print(xValLeft);
  Serial.print(", Y: ");
  Serial.print(yValLeft);
  Serial.print(", Button: ");
  Serial.println(buttonValLeft);
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
