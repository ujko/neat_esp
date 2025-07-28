
#include <esp_now.h>
#include <WiFi.h>

uint8_t outboardAddress[] = {0x3C, 0xE9, 0x0E, 0x6D, 0xE8, 0xD4};

const int criticalCurrent = 10;

//pins
const int buttonPinOne = 13;
const int buttonPinTwo = 14;
const int buttonPinThree = 15;
const int speedPin = 16;
const int potPin = 34;
const int switchPin = 27;
const int genSwitchPin = 26;
const int ammeterPin = 35;
const int relayPin = 25;

const int criticCurrent = 1850;

const float vcc    = 5.00;// supply voltage 5V or 3.3V
const float factor = 0.02;// 20mV/A is the factor

float voltage;


int pinOne;
int pinTwo;
int pinThree;
int speed;
int potValue;
int switchValue;
String success;

bool criticCurrentExceed = false;

esp_now_peer_info_t peerInfo;

typedef struct struct_message {
    int pinOne;
    int pinTwo;
    int pinThree;
    int speed;
    int potValue;
} struct_message;

struct_message PinReadings;

int pin1State = 0;
int pin2State = 0;
int pin3State = 0;
int pin4State = 0;
int potState = 0;

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

void setup() {
  WiFi.mode(WIFI_STA);
  Serial.begin(115200);
  
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  esp_now_register_send_cb(OnDataSent);
  
  // Register peer
  memcpy(peerInfo.peer_addr, outboardAddress, 6);
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;
  
  // Add peer        
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }

  pinMode(buttonPinOne, INPUT_PULLUP);
  pinMode(buttonPinTwo, INPUT_PULLUP);
  pinMode(buttonPinThree, INPUT_PULLUP);
  pinMode(speedPin, INPUT_PULLUP);
  pinMode(switchPin, INPUT_PULLUP);
  pinMode(ammeterPin, INPUT_PULLUP);
  pinMode(genSwitchPin, OUTPUT);
  pinMode(relayPin, OUTPUT);

  digitalWrite(relayPin, LOW);
}

void loop() {

  if ( criticCurrentExceed == false) {
  //ammeter
    int ameterPinR = analogRead(ammeterPin);
    int rawAdc = (5.0 / 1023.0) * ameterPinR;
    float voltage = rawAdc - (vcc * 0.5) + 0.07;
    float current = voltage / factor;

    Serial.print("current: ");
    Serial.print(current);
    Serial.print(" , nb: ");
    Serial.println(ameterPinR);

    //switch on / off
    switchValue = digitalRead(switchPin);
    switchValue == HIGH ? digitalWrite(genSwitchPin, HIGH) : digitalWrite(genSwitchPin, LOW);
    if ( ameterPinR > criticCurrent) {
      digitalWrite(genSwitchPin, HIGH);
      digitalWrite(relayPin, HIGH);
      criticCurrentExceed = true;
    }

    //data to send
    pin1State = digitalRead(buttonPinOne);
    pin2State = digitalRead(buttonPinTwo);
    pin3State = digitalRead(buttonPinThree);
    pin4State = digitalRead(speedPin);
    potState = analogRead(potPin);
    pin1State == HIGH ? PinReadings.pinOne = 1 : PinReadings.pinOne = 0;
    pin2State == HIGH ? PinReadings.pinTwo = 1 : PinReadings.pinTwo = 0;
    pin3State == HIGH ? PinReadings.pinThree = 1 : PinReadings.pinThree = 0;
    pin4State == HIGH ? PinReadings.speed = 1 : PinReadings.speed = 0;
    PinReadings.potValue = potState;
    esp_err_t result = esp_now_send(outboardAddress, (uint8_t *) &PinReadings, sizeof(PinReadings));

  }
}
