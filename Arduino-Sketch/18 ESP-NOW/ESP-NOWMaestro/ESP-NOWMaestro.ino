/*********
  Rui Santos
  Complete project details at https://RandomNerdTutorials.com/esp-now-one-to-many-esp32-esp8266/
  
  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files.
  
  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.
*********/

#include <esp_now.h>
#include <WiFi.h>

// Introduce la dirección MAC de las placas de desarrollo receptoras
uint8_t broadcastAddress1[] = {0x08, 0x3A, 0xF2, 0x85, 0xE1, 0x68};

String mensaje;

// Función enviar paquete
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  char macStr[18];
  Serial.print("Paquete para: ");
  // Direccion MAC del receptor
  snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
           mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
  Serial.print(macStr);
  Serial.print(" estadp del envío:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Entregado" : "Fallo de entrega");
}
 
void setup() {
  Serial.begin(115200);
 
  WiFi.mode(WIFI_STA);
 
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error al iniciar ESP-NOW");
    return;
  }
  
  esp_now_register_send_cb(OnDataSent);
   
  // Registro de envío
  esp_now_peer_info_t peerInfo;
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;
  // register first peer  
  memcpy(peerInfo.peer_addr, broadcastAddress1, 6);
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Fallo de registro");
    return;
  }

}
 
void loop() {
  
  mensaje = "Hola Mundo";
 
  esp_err_t result = esp_now_send(0, (uint8_t *) &mensaje, sizeof(mensaje));
   
  if (result == ESP_OK) {
    Serial.println("Entregado");
  }
  else {
    Serial.println("Error de entrega");
  }
  delay(2000);
}
