#include <WiFi.h>
#include <WebServer.h>

// 替换成你的苹果热点
const char WIFI_SSID[] = "你的热点名";
const char WIFI_PWD[] = "你的热点密码";

#define TOUCH_PIN 4
#define LED_PIN 2
#define THRESHOLD 400       // 完全沿用你能亮的400
#define DEBOUNCE_TIME 20    // 沿用你的消抖时间

WebServer server(80);
bool isArm = false;    // 未布防/已布防
bool isAlarm = false;  // 报警锁定
int lastTouchVal = 100;
unsigned long lastTouchTime = 0;

void handleIndex()
{
  String html = R"HTML(
<!DOCTYPE html>
<html>
<head>
<meta charset="UTF-8">
<title>Touch Alarm System</title>
<style>
body{text-align:center;margin-top:60px;font-family:Arial;}
button{font-size:22px;padding:10px 30px;margin:10px;}
#status{font-size:20px;margin-top:20px;font-weight:bold;}
</style>
</head>
<body>
<h2>触摸布防报警系统</h2>
<button onclick="arm()">布防</button>
<button onclick="disarm()">撤防</button>
<p id="status">当前状态：未布防</p >

<script>
function arm(){
  fetch("/arm").then(r=>r.text()).then(s=>document.getElementById("status").innerText="当前状态：已布防，警戒中");
}
function disarm(){
  fetch("/disarm").then(r=>r.text()).then(s=>document.getElementById("status").innerText="当前状态：未布防，已重置");
}
</script>
</body>
)HTML";
  server.send(200, "text/html", html);
}

// 布防
void handleArm()
{
  isArm = true;
  isAlarm = false;
  digitalWrite(LED_PIN, LOW);
  lastTouchVal = 100;
  lastTouchTime = 0;
  server.send(200, "text/plain", "armed");
}

// 撤防（唯一解除报警）
void handleDisarm()
{
  isArm = false;
  isAlarm = false;
  digitalWrite(LED_PIN, LOW);
  lastTouchVal = 100;
  lastTouchTime = 0;
  server.send(200, "text/plain", "disarmed");
}

void setup()
{
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  // WiFi稳定逻辑
  WiFi.disconnect(true, true);
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PWD);

  Serial.print("Connect WiFi ");
  int cnt = 0;
  while(WiFi.status() != WL_CONNECTED && cnt < 40)
  {
    delay(500);
    Serial.print(".");
    cnt++;
  }
  if(WiFi.status() != WL_CONNECTED) ESP.restart();
  Serial.println("\nIP: " + WiFi.localIP().toString());

  server.on("/", handleIndex);
  server.on("/arm", handleArm);
  server.on("/disarm", handleDisarm);
  server.begin();
}

void loop()
{
  server.handleClient();
  int touchVal = touchRead(TOUCH_PIN);
  unsigned long now = millis();

  // 1. 未布防：触摸完全无反应
  if(!isArm)
  {
    digitalWrite(LED_PIN, LOW);
    lastTouchVal = touchVal;
    delay(10);
    return;
  }

  // ========== 100%复刻你 ex04 成功的触摸触发代码 ==========
  if (now - lastTouchTime > DEBOUNCE_TIME) {
    if (touchVal < THRESHOLD && lastTouchVal >= THRESHOLD) {
      isAlarm = true; // 直接锁定报警
      lastTouchTime = now;
    }
  }
  lastTouchVal = touchVal;
  // ======================================================

  // 2. 报警：高频闪烁，只有撤防能关闭
  if(isAlarm)
  {
    digitalWrite(LED_PIN, HIGH);
    delay(80);
    digitalWrite(LED_PIN, LOW);
    delay(80);
  }

  delay(10);
}
