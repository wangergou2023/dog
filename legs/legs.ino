#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <PubSubClient.h>
#include <Wire.h>
#include <PCA9685.h>

const char* ssid = "your";  // 替换为您的WiFi名称
const char* password = "your";  // 替换为您的WiFi密码
const char* mqtt_server = "your"; // 替换为您的MQTT代理地址
const char* mqtt_user = "your"; // 替换为您的MQTT用户名
const char* mqtt_password = "your"; // 替换为您的MQTT密码

WiFiClient espClient;
PubSubClient client(espClient);
ESP8266WebServer server(80);
PCA9685 pca9685;

const int numMotors = 4;
int motorChannels[numMotors] = {4, 7, 8, 11}; // 电机通道映射
int motorAngles[numMotors] = {0, 0, 0, 0}; // 电机角度初始化为0

void setup() {
  Serial.begin(9600);
  Wire.begin(0,2);
  pca9685.setupSingleDevice(Wire, 0x40);
  pca9685.setToServoFrequency();

  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

  server.on("/", handleRoot);
  for (int i = 0; i < numMotors; i++) {
    String path = "/set-motor" + String(i) + "-angle";
    server.on(path.c_str(), [i]() { handleSetMotorAngle(i); });
  }
  server.begin();
}

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

void reconnect() {
  while (!client.connected()) {
    Serial.print("尝试MQTT连接...");
    if (client.connect("ESP8266Client", mqtt_user, mqtt_password)) {
      Serial.println("已连接");
      client.subscribe("motor/control");
    } else {
      Serial.print("连接失败, rc=");
      Serial.print(client.state());
      Serial.println(" 5秒后重试");
      delay(5000);
    }
  }
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  server.handleClient();
}

void handleRoot() {
  String html = "<html><head><meta charset='UTF-8'></head><body><h1>电机控制台</h1>";
  for (int i = 0; i < numMotors; i++) {
    html += "<p>电机" + String(i) + "角度: <a href='/set-motor" + String(i) + "-angle?angle=0'>0度</a> | ";
    html += "<a href='/set-motor" + String(i) + "-angle?angle=90'>90度</a> | ";
    html += "<a href='/set-motor" + String(i) + "-angle?angle=180'>180度</a></p>";
  }
  html += "</body></html>";
  server.send(200, "text/html", html);
}

void handleSetMotorAngle(int motorIndex) {
  if (server.hasArg("angle")) {
    int angle = server.arg("angle").toInt();
    if (angle >= 0 && angle <= 180) {
      motorAngles[motorIndex] = angle;
      int pulseWidth = map(angle, 0, 180, 500, 2500); // 将角度映射到脉冲宽度
      pca9685.setChannelServoPulseDuration(motorChannels[motorIndex], pulseWidth);
      String message = "电机" + String(motorIndex) + "角度设置为" + String(angle) + "度";
      Serial.println(message);  // 添加调试信息
      server.send(200, "text/html", "<html><head><meta charset='UTF-8'></head><body><h1>" + message + "</h1>"
                                    "<p><a href=\"/\">返回主页面</a></p></body></html>");
    } else {
      server.send(400, "text/html", "<html><head><meta charset='UTF-8'></head><body><h1>无效的角度值</h1>"
                                    "<p><a href=\"/\">返回主页面</a></p></body></html>");
    }
  } else {
    server.send(400, "text/html", "<html><head><meta charset='UTF-8'></head><body><h1>缺少角度参数</h1>"
                                  "<p><a href=\"/\">返回主页面</a></p></body></html>");
  }
}

void callback(char* topic, byte* payload, unsigned int length) {
  String message;
  for (int i = 0; i < length; i++) {
    message += (char)payload[i];
  }

  if (String(topic) == "motor/control") {
    int motorIndex = message.charAt(0) - '0'; // 获取电机索引
    int angle = message.substring(1).toInt(); // 获取角度
    if (motorIndex >= 0 && motorIndex < numMotors && angle >= 0 && angle <= 180) {
      motorAngles[motorIndex] = angle;
      int pulseWidth = map(angle, 0, 180, 500, 2500); // 将角度映射到脉冲宽度
      pca9685.setChannelServoPulseDuration(motorChannels[motorIndex], pulseWidth);
      String response = "电机" + String(motorIndex) + "角度设置为" + String(angle) + "度";
      Serial.println(response);  // 添加调试信息
      client.publish("motor/status", response.c_str());
    }
  }
}
