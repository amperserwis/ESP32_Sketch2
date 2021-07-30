
// Necesaary Libraries
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <WebSocketsServer.h>
#include <SPIFFS.h>
#include <ArduinoJson.h>

// Serial2 pins of ESP32
#define RXD2 16
#define TXD2 17

// Variables
int board;
int pin;
int pin_status;
String message = "";
bool messageReady = false;

AsyncWebServer server(80); 
WebSocketsServer websockets(81);

void notFound(AsyncWebServerRequest *request)
{
  request->send(404, "text/plain", "Página no encontrada");
}

void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {

  switch (type) 
  {
    case WStype_DISCONNECTED:
      Serial.printf("[%u] ¡Desconectado!\n", num);
      break;
    case WStype_CONNECTED: {
        IPAddress ip = websockets.remoteIP(num);
        Serial.printf("[%u] Conectado en %d.%d.%d.%d url: %s\n", num, ip[0], ip[1], ip[2], ip[3], payload);
      }
      break;
    case WStype_TEXT:
      Serial.printf("[%u] Texto: %s\n", num, payload);
      String mensaje = String((char*)( payload));
      Serial.println(mensaje);
      
      DynamicJsonDocument doc(1024);
      DeserializationError error = deserializeJson(doc, mensaje);
      if (error) {
        Serial.print("deserializeJson() failed: ");
        Serial.println(error.c_str());
        return;
        }
      board = doc["board"];
      pin= doc["pin"];
      pin_status = doc["pin_status"]; 

      Serial.print("board: ");
      Serial.println(board);
      Serial.print("pin: ");
      Serial.println(pin);
      Serial.print("estado del pin: ");
      Serial.println(pin_status);
      
      enviarDatos();
  }
}

void enviarDatos(){
      String mensaje;
      DynamicJsonDocument doc(1024);
      doc["board_status"] = board;
      doc["led"] = pin;
      doc["status"] = pin_status;
      serializeJson(doc, mensaje); // Sending data to another ESP32
      Serial.print("Enviando datos ");
      Serial2.println(mensaje);
      delay(2000);  
  }

void setup()
{
  // Debug console
  Serial.begin(115200); // For Debugging purpose
  Serial2.begin(9600, SERIAL_8N1, RXD2, TXD2); // For sending data to another ESP32
  
  WiFi.begin("studiomiranda", "88888888");
  Serial.print("Conectando");
  while (WiFi.status() != WL_CONNECTED)
    {
      delay(500);
      Serial.print(".");
    }
    Serial.println();
    Serial.print("Conectado, dirección IP: ");
    Serial.println(WiFi.localIP());
  
  if(!SPIFFS.begin(true)){
    Serial.println("A ocurrido un error al montar SPIFFS");
    return;
  }

   server.on("/", HTTP_GET, [](AsyncWebServerRequest * request)
  { 
   request->send(SPIFFS, "/index.html", "text/html");
  });

  server.onNotFound(notFound);
  server.begin();
  
  websockets.begin();
  websockets.onEvent(webSocketEvent);
}

void loop()
{   
  websockets.loop(); 
}
