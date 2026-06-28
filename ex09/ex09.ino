#include <WiFi.h>
#include <WebServer.h>

const char WIFI_SSID[] = "你的热点名称";
const char WIFI_PWD[] = "你的热点密码";

#define TOUCH_PIN 4
int touchVal = 0;
WebServer server(80);

void rootPage(){
  String html="<!DOCTYPE html><html><meta charset=utf-8><body style='text-align:center;margin-top:80px;'>";
  html+="<h2>触摸传感器实时仪表盘</h2>";
  html+="<p style='font-size:64px;color:red' id='num'>0</p >";
  html+="<p>手靠近引脚数值变小，离开恢复</p >";
  html+="<script>function update(){fetch('/getdata').then(r=>r.text()).then(t=>document.getElementById('num').innerText=t);}setInterval(update,100);</script>";
  html+="</body></html>";
  server.send(200,"text/html",html);
}

void sendData(){
  server.send(200,"text/plain",String(touchVal));
}

void setup(){
  Serial.begin(115200);
  WiFi.disconnect(true);
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID,WIFI_PWD);
  int wait=0;
  while(WiFi.status()!=WL_CONNECTED && wait<40){
    delay(500);
    wait++;
  }
  // 仅连接成功后打印IP，不会出现0.0.0.0
  Serial.println("WiFi连接成功！");
  Serial.print("访问地址：http://");
  Serial.println(WiFi.localIP());
  server.on("/",rootPage);
  server.on("/getdata",sendData);
  server.begin();
}

void loop(){
  server.handleClient();
  touchVal = touchRead(TOUCH_PIN);
  delay(10);
}
