#define LEDA_PIN 2
#define LEDB_PIN 18
const int freq = 5000;
const int resolution = 8;

void setup() {
  Serial.begin(115200);
  ledcAttach(LEDA_PIN, freq, resolution);
  ledcAttach(LEDB_PIN, freq, resolution);
}

void loop() {
  for(int dutyA = 0; dutyA <= 255; dutyA++) {
    int dutyB = 255 - dutyA;
    ledcWrite(LEDA_PIN, dutyA);
    ledcWrite(LEDB_PIN, dutyB);
    delay(6);
  }
  for(int dutyA = 255; dutyA >= 0; dutyA--) {
    int dutyB = 255 - dutyA;
    ledcWrite(LEDA_PIN, dutyA);
    ledcWrite(LEDB_PIN, dutyB);
  }
}
