#include "painlessMesh.h"
// 引用舵机库
#include <Servo.h>
//引用ws2812库
#include <Adafruit_NeoPixel.h>
// 引用Json库
#include "ArduinoJson.h"

// 类型切换：舵机或者彩灯
#define TYPE_SERVO

// #define TYPE_LED

// mesh网络配置信息
#define MESH_PREFIX "FrogMaker"
#define MESH_PASSWORD "KeepLeaning"
#define MESH_PORT 5555

Scheduler userScheduler;  // 创建调度器，用于调度发送光敏数据的任务
painlessMesh mesh;        // 创建mesh

#ifdef TYPE_LED
// 定义RGB灯珠颜色
uint8_t colors[3][3] = {{128, 0, 0}, {0, 128, 0}, {0, 0, 128}};
// 随机颜色
uint8_t colorIndex = random(3);
// 创建灯珠控制类
Adafruit_NeoPixel pixels(1, D7, NEO_GRB + NEO_KHZ800);
#endif

#ifdef TYPE_SERVO
// 创建舵机类
Servo servo;
#endif

// 消息回调
void receivedCallback(uint32_t from, String &msg) {
  DynamicJsonDocument doc(1024);  // 构建一个JSON对象
  deserializeJson(doc, msg);      // 反序列化JSON消息
  uint8_t bid = doc["bid"];       // 取得bid（按钮Id）

#ifdef TYPE_SERVO
  // 根据不同按钮Id来定义舵机转动角度
  servo.write(bid == 1 ? 180 : 0);
#endif

#ifdef TYPE_LED
  // 根据按钮设置灯珠亮度
  pixels.setBrightness(bid == 1 ? 255 : 0);
  // 设置颜色为随机颜色
  pixels.setPixelColor(
      0, pixels.Color(colors[colorIndex][0], colors[colorIndex][1],
                      colors[colorIndex][2]));
  // 显示灯珠
  pixels.show();
  Serial.printf("Received from %u msg: %s\n", from, msg.c_str());
#endif
}

void setup() {
  Serial.begin(115200);

#ifdef TYPE_SERVO
  // 绑定舵机在D7口
  servo.attach(D7);
#endif

#ifdef TYPE_LED
  // 初始化灯珠
  pixels.begin();
  // 清除灯珠所有设置
  pixels.clear();
  // 点亮灯珠
  pixels.setBrightness(100);
  pixels.setPixelColor(
      0, pixels.Color(colors[colorIndex][0], colors[colorIndex][1],
                      colors[colorIndex][2]));
  pixels.show();
  // 1秒后熄灭灯珠，预示初始化完成
  delay(1000);
  pixels.setBrightness(0);
  pixels.setPixelColor(0, pixels.Color(0, 0, 0));
  pixels.show();
#endif

  // 设置调试信息类型，需要在mesh.init()之前设置
  // ERROR | MESH_STATUS | CONNECTION | SYNC | COMMUNICATION | GENERAL |
  // MSG_TYPES | REMOTE
  mesh.setDebugMsgTypes(ERROR | STARTUP | CONNECTION);

  // 初始化mesh
  mesh.init(MESH_PREFIX, MESH_PASSWORD, MESH_PORT);
  // 收到消息事件
  mesh.onReceive(&receivedCallback);
}

void loop() {
  // 执行所有计划任务
  mesh.update();
}