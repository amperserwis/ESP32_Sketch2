
// Necessary Libraries
#include "painlessMesh.h"
#include <ArduinoJson.h>
#include <SPI.h>
#include <Wire.h>
#include <WiFi.h>
#include <WiFiClient.h>


// Serial2 pins of ESP32
#define RXD2 16
#define TXD2 17

// WiFi Credentials for Mesh Networking
#define   MESH_PREFIX     "whateverYouLike"
#define   MESH_PASSWORD   "somethingSneaky"
#define   MESH_PORT       5555


// Variables
int led;
int led_status;
int board_status;
int board;
int pin;
int pin_status;
bool messageReady = false;
bool message_received = false;
String message = "";


Scheduler userScheduler; // to control your personal task
painlessMesh  mesh;


// User stub
void sendMessage() ; // Prototype so PlatformIO doesn't complain
//void send_request() ;

Task taskSendMessage( TASK_SECOND * 1 , TASK_FOREVER, &sendMessage );
//Task taskSendRequest( TASK_SECOND * 1 , TASK_FOREVER, &send_request );

void sendMessage()
{

DynamicJsonDocument doc(1024);
  doc["board"] = board_status;
  doc["pin"] = led;
  doc["status"] =  led_status;
  String msg ;
  serializeJson(doc, msg);
  mesh.sendBroadcast( msg );
  Serial.print("Difusión a la malla: - "); Serial.println(msg);

 // taskSendMessage.setInterval((TASK_SECOND * 1));
}


// Needed for painless library
void receivedCallback( uint32_t from, String &msg ) {}

void newConnectionCallback(uint32_t nodeId) {
  //Serial.printf("--> startHere: New Connection, nodeId = %u\n", nodeId);
}

void changedConnectionCallback() {
  //Serial.printf("Changed connections\n");
}

void nodeTimeAdjustedCallback(int32_t offset) {
  //Serial.printf("Adjusted time %u. Offset = %d\n", mesh.getNodeTime(), offset);
}

void setup() {
  Serial.begin(115200);  // For Debugging purpose
  Serial2.begin(9600, SERIAL_8N1, RXD2, TXD2); // For sending data to another ESP32

  mesh.init( MESH_PREFIX, MESH_PASSWORD, &userScheduler, MESH_PORT );
  mesh.onReceive(&receivedCallback);
  mesh.onNewConnection(&newConnectionCallback);
  mesh.onChangedConnections(&changedConnectionCallback);
  mesh.onNodeTimeAdjusted(&nodeTimeAdjustedCallback);

  userScheduler.addTask( taskSendMessage );
//  userScheduler.addTask( taskSendRequest );
  taskSendMessage.enable();
//  taskSendRequest.enable();
  // timer.setInterval(1000L, send_request);
}

void loop()
{

  while (Serial2.available())
  {
    message = Serial2.readString();
    Serial.println(message);
    messageReady = true;
    Serial.print("Mensaje recibido "); Serial.println(message);
  }
  DynamicJsonDocument doc(1024);
  DeserializationError error = deserializeJson(doc, message);
  board_status = doc["board_status"];
  led = doc["led"];
  led_status = doc["status"];

  messageReady = false;

  // it will run the user scheduler as well
  mesh.update();
  //timer.run();
}
