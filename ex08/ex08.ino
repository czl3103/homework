#include <WiFi.h>
#include <WebServer.h>

// ESP32‑AP 参数
const char AP_SSID[] = "ESP32‑Alarm";
const char AP_PWD[] = "12345678";

#define TOUCH_PIN 4
#define LED_PIN 2
#define THRESHOLD 400
#define DEBOUNCE_TIME 20

WebServer server(80);
bool isArm = false;
bool isAlarm = false;
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
<h2>触摸布防报警系统(AP模式)</h2>
<button onclick="arm()">布防</button>
<button onclick="disarm()">撤防</button>
<p id="status">当前状态：未布防</p >

<script>
function refreshStatus(){
  fetch("/getstate")
  .then(res => res.json())
  .then(data => {
    let stateText = "";
    if(!data.armed){
      stateText = "当前状态：未布防，已重置";
    }else if(data.alarm){
      stateText = "当前状态：已布防，触发警戒！";
    }else{
      stateText = "当前状态：已布防，警戒中";
    }
    document.getElementById("status").innerText = stateText;
  })
}
setInterval(refreshStatus, 200);

function arm(){fetch("/arm");}
function disarm(){fetch("/disarm");}
</script>
</body>
)HTML";
  server.send(200, "text/html", html);
}

void handleGetState()
{
  String json = "{\"armed\":" + String(isArm) + ",\"alarm\":" + String(isAlarm) + "}";
  server.send(200, "application/json", json);
}

void handleArm()
{
  isArm = true;
  isAlarm = false;
  digitalWrite(LED_PIN, LOW);
  lastTouchVal = 100;
  lastTouchTime = 0;
  server.send(200, "text/plain", "armed");
}

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

  // 配置为AP热点模式
  WiFi.softAP(AP_SSID, AP_PWD);
  IPAddress myIP = WiFi.softAPIP();
  Serial.print("ESP32‑AP IP: ");
  Serial.println(myIP);

  server.on("/", handleIndex);
  server.on("/arm", handleArm);
  server.on("/disarm", handleDisarm);
  server.on("/getstate", handleGetState);
  server.begin();
}

void loop()
{
  server.handleClient();
  int touchVal = touchRead(TOUCH_PIN);
  unsigned long now = millis();

  if(!isArm)
  {
    digitalWrite(LED_PIN, LOW);
    lastTouchVal = touchVal;
    delay(10);
    return;
  }

  if (now - lastTouchTime > DEBOUNCE_TIME) {
    if (touchVal < THRESHOLD && lastTouchVal >= THRESHOLD) {
      isAlarm = true;
      lastTouchTime = now;
    }
  }
  lastTouchVal = touchVal;

  if(isAlarm)
  {
    digitalWrite(LED_PIN, HIGH);
    delay(80);
    digitalWrite(LED_PIN, LOW);
    delay(80);
  }
  delay(10);
}
