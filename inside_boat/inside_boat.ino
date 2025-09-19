//inboard part (master)
#include <esp_now.h>
#include <WiFi.h>

// uint8_t outboardAddress[] = {0x80, 0xF3, 0xDA, 0x40, 0xEE, 0xC0};  //testowy z antenka
uint8_t outboardAddress[] = {0xD4, 0xD4, 0xDA, 0xCE, 0xE3, 0x08};  //destiny

const int center = 125;
const int gap = 5;

//pins
const int potPin = 34;

int potValue;

String success;

esp_now_peer_info_t peerInfo;

typedef struct struct_message {
    int pinOne;
    int potValue;
} struct_message;

struct_message PinReadings;

int pin1State = 0;

void OnDataSent(const uint8_t mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}

void setup() {
  WiFi.mode(WIFI_STA);
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);

  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Register peer
  memcpy(peerInfo.peer_addr, outboardAddress, 6);
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;
  
  // Add peer        
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }
}

int oldPv = -1;

void loop() {
    //data to send
    int potState = analogRead(potPin);
    int pv = potState / 16;
    int step = 1;
    if (oldPv > pv + step || oldPv < pv - step) {
      if (pv > center + gap) {
        PinReadings.pinOne = 1;
        PinReadings.potValue = (pv - center - gap) * 2;
      } else if (pv < center - gap) {
        PinReadings.pinOne = 0;
        PinReadings.potValue = 255 - (pv * 2);
      } else {
        PinReadings.potValue = 0;
      }
      esp_err_t result = esp_now_send(outboardAddress, (uint8_t *) &PinReadings, sizeof(PinReadings));
      oldPv = pv;
    }
    delay(100);
}
