const int ledPin = 2;
unsigned long previousMillis = 0;
const unsigned long interval = 500;
bool ledState = LOW;

void setup() {
  Serial.begin(115200);
  pinMode(ledPin, OUTPUT);
}

void loop() {
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    ledState = !ledState;
    digitalWrite(ledPin, ledState);
    Serial.print("LED状态: ");
    Serial.println(ledState ? "ON" : "OFF");
  }
}
