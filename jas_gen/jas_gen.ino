
// below we set the frequency - for two signals in this case 2*30 + (delay between sginals) 2*1  = 62,
// also change state takes about 1 microsecs and we have 4 then we have about 66 microsecs delay in total
const int delayInMicrosecs = 30; //as above (30 * 2) + 2 + 4 = 66, 1000000 / 66 = 15151 Hz
const int statDel = 1;  // delay between impulses (in microseconds)
const int sig1Pin = 7;  // pin 1 signal. //32 doit 13,14,15
const int sig2Pin = 9;  // pin 2 signal //mini 7,8,6
const int inputSwitchPin = 6; // pin for input, everything works when HIGH, in LOW waiting mode
int inpSig = 0;
int lastSig = 0;


void setup() {
  pinMode(sig1Pin, OUTPUT);
  pinMode(sig2Pin, OUTPUT);
  pinMode(inputSwitchPin, INPUT);
  digitalWrite(sig1Pin, LOW);
  digitalWrite(sig2Pin, LOW);
}

void loop() {
  inpSig = digitalRead(inputSwitchPin);
  if (inpSig == LOW) {
    digitalWrite(sig1Pin, HIGH);
    delayMicroseconds(delayInMicrosecs);
    digitalWrite(sig1Pin, LOW);
  }
  inpSig = digitalRead(inputSwitchPin);
  if (inpSig == LOW) {
    digitalWrite(sig2Pin, HIGH);
    delayMicroseconds(delayInMicrosecs);
    digitalWrite(sig2Pin, LOW);
  }
}


  // if (inpSig > lastSig) {
  //   gen(10);
  //   gen(5);
  // } else if (lastSig > inpSig) {
  //   gen(5);
  //   gen(10);
  // }
  // lastSig = inpSig;



void gen(int delayTime) {
  digitalWrite(sig1Pin, HIGH);
  delayMicroseconds(delayTime);
  digitalWrite(sig1Pin, LOW);
  delayMicroseconds(statDel);
  digitalWrite(sig2Pin, HIGH);
  delayMicroseconds(delayTime);
  digitalWrite(sig2Pin, LOW);
  delayMicroseconds(statDel);
}


