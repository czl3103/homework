#define TOUCH_PIN 4
#define LED_PIN 2
#define THRESHOLD 20
#define DEBOUNCE_TIME 20

const int freq = 5000;
const int resolution = 8;

int speedLevel = 1; // 1慢 / 2中 / 3快
int lastTouchVal = 100;
unsigned long lastTouchTime = 0;

void setup() {
  Serial.begin(115200);
  // 新版标准PWM绑定API，无ledcSetup报错
  ledcAttach(LED_PIN, freq, resolution);
}

void loop() {
  // 触摸检测、档位切换逻辑
  int touchVal = touchRead(TOUCH_PIN);
  unsigned long now = millis();
  if (now - lastTouchTime > DEBOUNCE_TIME) {
    if (touchVal < THRESHOLD && lastTouchVal >= THRESHOLD) {
      speedLevel++;
      if(speedLevel > 3) speedLevel = 1;
      Serial.print("当前档位：");
      Serial.println(speedLevel);
      lastTouchTime = now;
    }
  }
  lastTouchVal = touchVal;

  // 根据档位设置呼吸步长，控制速度
  int step;
  switch(speedLevel){
    case 1: step = 1; break; // 慢速呼吸
    case 2: step = 3; break; // 中速呼吸
    case 3: step = 6; break; // 快速呼吸
  }

  // 渐亮
  for(int duty = 0; duty <= 255; duty += step){
    ledcWrite(LED_PIN, duty);
    delay(10);
  }
  // 渐暗
  for(int duty = 255; duty >= 0; duty -= step){
    ledcWrite(LED_PIN, duty);
    delay(10);
  }
}
