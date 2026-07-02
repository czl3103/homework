#define TOUCH_PIN 4
#define LED_PIN 2
#define THRESHOLD 400
#define DEBOUNCE_TIME 20

const int freq = 5000;
const int resolution = 8;

int speedLevel = 1; // 1慢 / 2中 / 3快
int lastTouchVal = 100;
unsigned long lastTouchTime = 0;

// 非阻塞呼吸全局变量
int duty = 0;
int dir = 1; // 1变亮 -1变暗
unsigned long lastBreathTime = 0;
int breathDelay; // 每步间隔，由档位控制

void setup() {
  Serial.begin(115200);
  ledcAttach(LED_PIN, freq, resolution);
}

void loop() {
  // 1. 触摸检测逻辑（完全保留原有触摸判断）
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

  // 根据档位更新呼吸单步延时（控制速度）
  switch(speedLevel){
    case 1: breathDelay = 10; break; // 慢速
    case 2: breathDelay = 3; break;  // 中速
    case 3: breathDelay = 1; break;  // 快速
  }

  // 2. 非阻塞呼吸，不会卡住触摸检测
  if(now - lastBreathTime >= breathDelay){
    lastBreathTime = now;
    duty += dir;

    // 亮度边界反转方向
    if(duty >= 255){
      dir = -1;
    }else if(duty <= 0){
      dir = 1;
    }
    ledcWrite(LED_PIN, duty);
  }
}
