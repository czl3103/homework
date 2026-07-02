#include <WiFi.h>
#include <WebServer.h>

// ESP32 AP热点配置
const char AP_SSID[] = "ESP32-Dimmer";
const char AP_PWD[] = "12345678";

const int LED_PIN = 2;
const int PWM_FREQ = 5000;
const int PWM_RES = 8;

WebServer server(80);

void webIndex()
{
  String page = R"HTML(
<!DOCTYPE html>
<html>
<head>
<meta charset="UTF-8">
<title>LED Dimmer</title>
</head>
<body style="text-align:center;margin-top:50px;">
<h2>Brightness Slider</h2>
<input type="range" min="0" max="255" value="0" id="slide">
<p>Value: <span id="num">0</span></p >
<script>
let s = document.getElementById("slide");
let n = document.getElementById("num");
s.oninput = function(){
  n.innerText = this.value;
  fetch("/set?val="+this.value);
}
</script>
</body>
)HTML";
  server.send(200, "text/html", page);
}

void setBrightness()
{
  int val = server.arg("val").toInt();
  if(val >= 0 && val <= 255)
  {
    ledcWrite(LED_PIN, val);
  }
  server.send(200, "text/plain", "OK");
}

void setup()
{
  Serial.begin(115200);
  ledcAttach(LED_PIN, PWM_FREQ, PWM_RES);
  ledcWrite(LED_PIN, 0);

  // 切换为AP模式
  WiFi.disconnect(true, true);
  WiFi.mode(WIFI_AP);
  WiFi.softAP(AP_SSID, AP_PWD);

  IPAddress apIP = WiFi.softAPIP();
  Serial.println("ESP32 AP 已开启");
  Serial.print("访问地址：http://");
  Serial.println(apIP);

  server.on("/", webIndex);
  server.on("/set", setBrightness);
  server.begin();
}

void loop()
{
  server.handleClient();
}
