
#include <esp_now.h>
#include <WiFi.h>

uint8_t inboardAddress[] = {0xD4, 0xD4, 0xDA, 0xCE, 0xE3, 0x08};

//pins
const int buttonPinOne = 13;
const int buttonPinTwo = 14;
const int buttonPinThree = 15;
const int speedPin = 16;
const int potPin = 33;

int incomingPinOne;
int incomingPinTwo;
int incomingPinThree;
int incomingSpeed;
int incomingPotValue;
String success;

typedef struct struct_message {
    int pinOne;
    int pinTwo;
    int pinThree;
    int speed;
    int potValue;
} struct_message;

struct_message incomingReadings;

esp_now_peer_info_t peerInfo;

void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
  if (status ==0){
    success = "Delivery Success :)";
  }
  else{
    success = "Delivery Fail :(";
  }
}

void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  memcpy(&incomingReadings, incomingData, sizeof(incomingReadings));
  Serial.print("Bytes received: ");
  Serial.println(len);
  incomingPinOne = incomingReadings.pinOne;
  incomingPinTwo = incomingReadings.pinTwo;
  incomingPinThree = incomingReadings.pinThree;
  incomingSpeed = incomingReadings.speed;
  incomingPotValue = incomingReadings.potValue;
}

void setup() {
  WiFi.mode(WIFI_STA);
  Serial.begin(115200);
  
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  esp_now_register_send_cb(OnDataSent);
  
  // Register peer
  memcpy(peerInfo.peer_addr, inboardAddress, 6);
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;
  
  // Add peer        
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }

  esp_now_register_recv_cb(esp_now_recv_cb_t(OnDataRecv));

  pinMode(buttonPinOne, OUTPUT);
  pinMode(buttonPinTwo, OUTPUT);
  pinMode(buttonPinThree, OUTPUT);
  pinMode(speedPin, OUTPUT);
  pinMode(potPin, OUTPUT);
}

void loop() {
  incomingPinOne == 0 ? digitalWrite(buttonPinOne, LOW) : digitalWrite(buttonPinOne, HIGH);
  incomingPinTwo == 0 ? digitalWrite(buttonPinTwo, LOW) : digitalWrite(buttonPinTwo, HIGH);
  incomingPinThree == 0 ? digitalWrite(buttonPinThree, LOW) : digitalWrite(buttonPinThree, HIGH);
  incomingSpeed == 0 ? digitalWrite(speedPin, LOW) : digitalWrite(speedPin, HIGH);
  Serial.println(incomingPotValue);
  analogWrite(potPin, incomingPotValue);
  delay(10);
}
