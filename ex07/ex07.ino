#include <WiFi.h>
#include <WebServer.h>

// 修改为你的iPhone热点名称、密码
const char WIFI_SSID[] = "你的热点名";
const char WIFI_PWD[] = "你的热点密码";

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

  WiFi.disconnect(true, true);
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PWD);

  Serial.print("Connect WiFi ");
  int count = 0;
  while(WiFi.status() != WL_CONNECTED && count < 40)
  {
    delay(500);
    Serial.print(".");
    count++;
  }

  if(WiFi.status() == WL_CONNECTED)
  {
    Serial.println("\nConnected! IP: " + WiFi.localIP().toString());
  }
  else
  {
    Serial.println("\nConnect failed, restart...");
    ESP.restart();
  }

  server.on("/", webIndex);
  server.on("/set", setBrightness);
  server.begin();
}

void loop()
{
  server.handleClient();
}
