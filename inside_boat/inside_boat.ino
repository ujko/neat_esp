/* inboard part (master) */
#include <esp_now.h>
#include <WiFi.h>

// uint8_t outboardAddress[] = {0x80, 0xF3, 0xDA, 0x40, 0xEE, 0xC0};  //testowy z antenka
uint8_t outboardAddress[] = {0xD4, 0xD4, 0xDA, 0xCE, 0xE3, 0x08};  //destiny

const int center = 113;
const int gap = 5;

//pins
const int potPin = 34;
const int okLedPin = 13;
const int notOkLedPin = 33;
const int middlePot = 32;

const int maxValue = 255;

int potValue;

bool isOkConnection = false;

String success;

esp_now_peer_info_t peerInfo;

typedef struct struct_message {
    int pinOne;
    int potValue;
} struct_message;

struct_message PinReadings;

typedef struct struct_message_chk {
    int test;
} struct_message_chk;

struct_message_chk incomingReadings;

int pin1State = 0;

void OnDataSent(const wifi_tx_info_t* wifiInfo, esp_now_send_status_t status) {
  Serial.print("\r\nLast Packet Send Status:\t");
  if (status == ESP_NOW_SEND_SUCCESS) {
    Serial.println("Delivery succeess");
    isOkConnection = true;
  } else {
    Serial.println("Delivery failed");
    isOkConnection = false;
  }
}

void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  memcpy(&incomingReadings, incomingData, sizeof(incomingReadings));
  Serial.print("Outboard status checked ");
}

void setup() {
  WiFi.mode(WIFI_STA);
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  pinMode(okLedPin, OUTPUT);
  pinMode(notOkLedPin, OUTPUT);
  pinMode(middlePot, OUTPUT);

  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  esp_now_register_send_cb(OnDataSent);

  esp_now_register_recv_cb(esp_now_recv_cb_t(OnDataRecv));

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

void loop() {
    //data to send
    int potState = analogRead(potPin);
    int pv = potState / 16;
    int step = 1;
    int sendVal;
    Serial.print("Sending data: ");
    if (pv > center + gap) {
      PinReadings.pinOne = 1;
      sendVal = (pv - center) * 2;
      Serial.print("BACKWARD ");
      digitalWrite(middlePot, LOW);    
    } else if (pv < center - gap) {
      PinReadings.pinOne = 0;
      sendVal = 255 - (pv * 2);
      Serial.print("FORWARD ");
      digitalWrite(middlePot, LOW);
    } else {
      sendVal = 0;
      Serial.print("STOP ENGINE ");
      digitalWrite(middlePot, HIGH);
    }
    if (sendVal > maxValue) {
      sendVal = maxValue;
    }
    PinReadings.potValue = sendVal;
    Serial.println(PinReadings.potValue);
    esp_err_t result = esp_now_send(outboardAddress, (uint8_t *) &PinReadings, sizeof(PinReadings));

    if (isOkConnection) {
      digitalWrite(okLedPin, HIGH);
      digitalWrite(notOkLedPin, LOW);
    } else {
      digitalWrite(okLedPin, LOW);
      digitalWrite(notOkLedPin, HIGH);
    }
    delay(100);
}

/* inboard part (master) */
