#include "painlessMesh.h"
// 引用Json库
#include "ArduinoJson.h"

// mesh网络配置信息
#define MESH_PREFIX "FrogMaker"
#define MESH_PASSWORD "Leaning"
#define MESH_PORT 5555

#define BUTTON_A_PIN 15  // 按钮A针脚
#define BUTTON_B_PIN 13  // 按钮B针脚
#define BUTTON_A_ID 1    // 按钮A Id
#define BUTTON_B_ID 2    // 按钮B Id

painlessMesh mesh;  // 创建mesh

void newConnectionCallback(uint32_t nodeId) {
  // 新节点连接到mesh网路
  Serial.printf("New Connection, nodeId = %u\n", nodeId);
}

void droppedConnectionCallback(uint32_t nodeId) {
  // 节点断开mesh网络
  Serial.printf("Connection is dropped, nodeId = %u\n", nodeId);
}

unsigned long timeA = 0;
ICACHE_RAM_ATTR void buttonAClick() {
  if (millis() - timeA > 300) {
    Serial.println("Button A is clicked");
    DynamicJsonDocument doc(1024);
    doc["nid"] = mesh.getNodeId();  // 节点Id
    doc["bid"] = BUTTON_A_ID;       // 按钮Id
    String msg;
    serializeJson(doc, msg);
    // 广播消息
    mesh.sendBroadcast(msg);
  }
  timeA = millis();
}

unsigned long timeB = 0;
ICACHE_RAM_ATTR void buttonBClick() {
  if (millis() - timeB > 300) {
    Serial.println("Button B is clicked");
    DynamicJsonDocument doc(1024);
    doc["nid"] = mesh.getNodeId();  // 节点Id
    doc["bid"] = BUTTON_B_ID;       // 按钮Id
    String msg;
    serializeJson(doc, msg);
    // 广播消息
    mesh.sendBroadcast(msg);
  }
  timeB = millis();
}

void setup() {
  Serial.begin(115200);

  pinMode(BUTTON_A_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(BUTTON_A_PIN), buttonAClick, RISING);

  pinMode(BUTTON_B_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(BUTTON_B_PIN), buttonBClick, RISING);

  // 设置调试信息类型，需要在mesh.init()之前设置
  // ERROR | MESH_STATUS | CONNECTION | SYNC | COMMUNICATION | GENERAL |
  // MSG_TYPES | REMOTE
  mesh.setDebugMsgTypes(ERROR | STARTUP | CONNECTION);

  // 初始化mesh
  mesh.init(MESH_PREFIX, MESH_PASSWORD, MESH_PORT);
  // 新节点连接事件，用于观测是否有新的节点连接到mesh网络
  mesh.onNewConnection(&newConnectionCallback);
  // 节点连接断开事件，用于观测是否有节点断开mesh网络
  mesh.onDroppedConnection(&droppedConnectionCallback);
}

void loop() {
  // 执行所有计划任务
  mesh.update();
}