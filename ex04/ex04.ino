#define TOUCH_PIN 4
#define LED_PIN 2
#define THRESHOLD 20
#define DEBOUNCE_TIME 20

bool ledState = false;
int lastTouchVal = 100;
unsigned long lastTouchTime = 0;

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
}

void loop() {
  int touchVal = touchRead(TOUCH_PIN);
  unsigned long now = millis();

  if (now - lastTouchTime > DEBOUNCE_TIME) {
    if (touchVal < THRESHOLD && lastTouchVal >= THRESHOLD) {
      ledState = !ledState;
      digitalWrite(LED_PIN, ledState);
      lastTouchTime = now;
    }
  }
  lastTouchVal = touchVal;
  delay(10);
}
