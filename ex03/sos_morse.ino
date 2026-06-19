const int ledPin = 2;
const unsigned long DOT_ON = 200;
const unsigned long DASH_ON = 600;
const unsigned long SYMBOL_GAP = 200;
const unsigned long LETTER_GAP = 500;
const unsigned long SOS_GAP = 2000;

enum State {
  S_DOT, S_GAP, S_TO_O_GAP,
  O_DASH, O_GAP, O_TO_S_GAP,
  LAST_S_DOT, LAST_S_GAP, SOS_WAIT
};

State currentState = S_DOT;
unsigned long previousMillis = 0;
unsigned long stateStartTime = 0;
int dotCount = 0;
int dashCount = 0;
bool ledOn = false;

void setup() {
  Serial.begin(115200);
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);
}

void loop() {
  unsigned long currentMillis = millis();
  switch (currentState) {
    case S_DOT:
      if (!ledOn) { stateStartTime = currentMillis; ledOn = true; digitalWrite(ledPin, HIGH); }
      if (currentMillis - stateStartTime >= DOT_ON) {
        digitalWrite(ledPin, LOW); ledOn = false; dotCount++;
        previousMillis = currentMillis; currentState = S_GAP;
      }
      break;
    case S_GAP:
      if (currentMillis - previousMillis >= SYMBOL_GAP) {
        if (dotCount < 3) currentState = S_DOT;
        else { dotCount = 0; previousMillis = currentMillis; currentState = S_TO_O_GAP; }
      }
      break;
    case S_TO_O_GAP:
      if (currentMillis - previousMillis >= LETTER_GAP) currentState = O_DASH;
      break;
    case O_DASH:
      if (!ledOn) { stateStartTime = currentMillis; ledOn = true; digitalWrite(ledPin, HIGH); }
      if (currentMillis - stateStartTime >= DASH_ON) {
        digitalWrite(ledPin, LOW); ledOn = false; dashCount++;
        previousMillis = currentMillis; currentState = O_GAP;
      }
      break;
    case O_GAP:
      if (currentMillis - previousMillis >= SYMBOL_GAP) {
        if (dashCount < 3) currentState = O_DASH;
        else { dashCount = 0; previousMillis = currentMillis; currentState = O_TO_S_GAP; }
      }
      break;
    case O_TO_S_GAP:
      if (currentMillis - previousMillis >= LETTER_GAP) currentState = LAST_S_DOT;
      break;
    case LAST_S_DOT:
      if (!ledOn) { stateStartTime = currentMillis; ledOn = true; digitalWrite(ledPin, HIGH); }
      if (currentMillis - stateStartTime >= DOT_ON) {
        digitalWrite(ledPin, LOW); ledOn = false; dotCount++;
        previousMillis = currentMillis; currentState = LAST_S_GAP;
      }
      break;
    case LAST_S_GAP:
      if (currentMillis - previousMillis >= SYMBOL_GAP) {
        if (dotCount < 3) currentState = LAST_S_DOT;
        else { dotCount = 0; previousMillis = currentMillis; currentState = SOS_WAIT; Serial.println("一轮SOS发送完成"); }
      }
      break;
    case SOS_WAIT:
      if (currentMillis - previousMillis >= SOS_GAP) currentState = S_DOT;
      break;
  }
}
