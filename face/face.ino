#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include "Face.h"

// WiFi配置
const char* ssid = "your";  // 替换为您的WiFi名称
const char* password = "your";  // 替换为您的WiFi密码

// Web服务器对象
ESP8266WebServer server(80);

// 面部对象
Face *face;

// 初始化WiFi连接
void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("连接到 ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi已连接");
  Serial.println("IP地址: ");
  Serial.println(WiFi.localIP());
}

// 处理根路径的HTTP请求
void handleRoot() {
  String html = "<html><head><meta charset='UTF-8'></head><body><h1>表情控制台</h1>"
                "<p><a href=\"/set-emotion?emotion=Normal\">正常</a></p>"
                "<p><a href=\"/set-emotion?emotion=Angry\">生气</a></p>"
                "<p><a href=\"/set-emotion?emotion=Glee\">快乐</a></p>"
                "<p><a href=\"/set-emotion?emotion=Happy\">高兴</a></p>"
                "<p><a href=\"/set-emotion?emotion=Sad\">悲伤</a></p>"
                "<p><a href=\"/set-emotion?emotion=Worried\">担忧</a></p>"
                "<p><a href=\"/set-emotion?emotion=Focused\">专注</a></p>"
                "<p><a href=\"/set-emotion?emotion=Annoyed\">烦躁</a></p>"
                "<p><a href=\"/set-emotion?emotion=Surprised\">惊讶</a></p>"
                "<p><a href=\"/set-emotion?emotion=Skeptic\">怀疑</a></p>"
                "<p><a href=\"/set-emotion?emotion=Frustrated\">沮丧</a></p>"
                "<p><a href=\"/set-emotion?emotion=Unimpressed\">无感</a></p>"
                "<p><a href=\"/set-emotion?emotion=Sleepy\">困倦</a></p>"
                "<p><a href=\"/set-emotion?emotion=Suspicious\">怀疑</a></p>"
                "<p><a href=\"/set-emotion?emotion=Squint\">眯眼</a></p>"
                "<p><a href=\"/set-emotion?emotion=Furious\">狂怒</a></p>"
                "<p><a href=\"/set-emotion?emotion=Scared\">害怕</a></p>"
                "<p><a href=\"/set-emotion?emotion=Awe\">敬畏</a></p>"
                "</body></html>";
  server.send(200, "text/html", html);
}

// 处理设置表情的HTTP请求
void handleSetEmotion() {
  String emotion = server.arg("emotion");
  if (emotion == "Normal") {
    face->Expression.GoTo_Normal();
  } else if (emotion == "Angry") {
    face->Expression.GoTo_Angry();
  } else if (emotion == "Glee") {
    face->Expression.GoTo_Glee();
  } else if (emotion == "Happy") {
    face->Expression.GoTo_Happy();
  } else if (emotion == "Sad") {
    face->Expression.GoTo_Sad();
  } else if (emotion == "Worried") {
    face->Expression.GoTo_Worried();
  } else if (emotion == "Focused") {
    face->Expression.GoTo_Focused();
  } else if (emotion == "Annoyed") {
    face->Expression.GoTo_Annoyed();
  } else if (emotion == "Surprised") {
    face->Expression.GoTo_Surprised();
  } else if (emotion == "Skeptic") {
    face->Expression.GoTo_Skeptic();
  } else if (emotion == "Frustrated") {
    face->Expression.GoTo_Frustrated();
  } else if (emotion == "Unimpressed") {
    face->Expression.GoTo_Unimpressed();
  } else if (emotion == "Sleepy") {
    face->Expression.GoTo_Sleepy();
  } else if (emotion == "Suspicious") {
    face->Expression.GoTo_Suspicious();
  } else if (emotion == "Squint") {
    face->Expression.GoTo_Squint();
  } else if (emotion == "Furious") {
    face->Expression.GoTo_Furious();
  } else if (emotion == "Scared") {
    face->Expression.GoTo_Scared();
  } else if (emotion == "Awe") {
    face->Expression.GoTo_Awe();
  }
  server.send(200, "text/html", "<html><head><meta charset='UTF-8'></head><body><h1>表情已设置为 " + emotion + "</h1>"
                                "<p><a href=\"/\">返回主页面</a></p></body></html>");
}

void setup() {
  Serial.begin(115200);
  
  // 初始化WiFi连接
  setup_wifi();
  
  // 创建一个新的面部对象，设置屏幕宽度、高度和眼睛大小
  face = new Face(/* screenWidth = */ 128, /* screenHeight = */ 64, /* eyeSize = */ 40);
  // 设置当前表情为正常
  face->Expression.GoTo_Normal();

  // 设置每种情绪的权重（目前被注释掉）
  //face->Behavior.SetEmotion(eEmotions::Normal, 1.0);
  //face->Behavior.SetEmotion(eEmotions::Angry, 1.0);
  //face->Behavior.SetEmotion(eEmotions::Sad, 1.0);
  // 自动在不同的行为之间切换（基于分配给每种情绪的权重随机选择新的行为）
  face->RandomBehavior = true;

  // 自动眨眼
  face->RandomBlink = true;
  // 设置眨眼的间隔时间
  face->Blink.Timer.SetIntervalMillis(4000);

  // 自动选择新的随机方向看
  face->RandomLook = false;

  // 配置Web服务器路由
  server.on("/", handleRoot);
  server.on("/set-emotion", handleSetEmotion);
  server.begin();

  Serial.println("HTTP服务器已启动");
}

void loop() {
  // 处理Web服务器客户端请求
  server.handleClient();

  // 更新面部显示
  face->Update();

}
