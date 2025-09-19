//Outboard part program (slave)
#include <esp_now.h>
#include <WiFi.h>

// uint8_t inboardAddress[] = {0x84, 0x1F, 0xE8, 0x07, 0xCD, 0x84};  //TEST WITH ANTENNA
uint8_t inboardAddress[] = {0x3C, 0xE9, 0x0E, 0x6D, 0xE8, 0xD4};  //DESTINY. 

//pins
const int buttonPinOne = 13;
const int potPin = 25;

int incomingPinOne;
int incomingPotValue;
String success;

typedef struct struct_message {
    int pinOne;
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
  // Serial.print("Bytes received: ");
  // Serial.println(len);
  incomingPinOne = incomingReadings.pinOne;
  incomingPotValue = incomingReadings.potValue;
}

void setup() {
  WiFi.mode(WIFI_STA);
  Serial.begin(115200);
  
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  // esp_now_register_send_cb(OnDataSent);
  
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
}

int oldIncomingPinOne = -1;
int oldPotVal = -1;

void loop() {
  if (oldIncomingPinOne != incomingPinOne || oldPotVal != incomingPotValue) {
    incomingPinOne == 0 ? digitalWrite(buttonPinOne, LOW) : digitalWrite(buttonPinOne, HIGH);
    dacWrite(potPin, incomingPotValue);
    oldIncomingPinOne = incomingPinOne;
    oldPotVal = incomingPotValue;
  }
}
