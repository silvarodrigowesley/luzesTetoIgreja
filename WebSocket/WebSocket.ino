#include <WiFi.h>
#include <WebServer.h>
#include <WebSocketsServer.h>
#include <ArduinoJson.h>

const char* ssid = "Não Conectado";
const char* password = "staff@2024";
bool statusWifi = false;
//  LUZ DA PLACA
#define relayPin 2  // Use o pino que você conectou o relé
// Configurações de Wi-Fi
IPAddress ip(192, 168, 68, 135);
IPAddress gateway(192, 168, 68, 1);
IPAddress subnet(255, 255, 255, 0);


/*int gpiosCount = 8;
byte gpios[8] = { 32, 33, 25, 26, 27, 14, 13, 15 };*/
int gpiosCount = 20;
byte gpios[20] = {0, 25, 26, 23, 22, 27, 14, 12, 13, 21, 19, 18, 17, 16, 4, 33, 32, 3, 15};
WebSocketsServer webSocket = WebSocketsServer(81);  // Servidor WebSocket na porta 81
void handleRelayOn(uint8_t clientID) {
  Serial.println("Relé LIGADO");
  for (byte i = 0; i < gpiosCount; i++) {
    digitalWrite(gpios[i], LOW);
    delay(40);
    Serial.println(gpios[i], DEC);
  }
  webSocket.sendTXT(clientID, "Relé ligado");
}
void handleRelayOff(uint8_t clientID) {
  for (byte i = 0; i < gpiosCount; i++) {
    digitalWrite(gpios[i], HIGH);
    Serial.println(gpios[i], DEC);
  }
  webSocket.sendTXT(clientID, "Relé Desligado");
}
void handleWebSocketMessage(uint8_t clientID, WStype_t type, uint8_t * payload, size_t length)  {
  if (type == WStype_TEXT) {
    String jsonMessage = String((char*)payload);
    StaticJsonDocument<300> doc;
    DeserializationError error = deserializeJson(doc, jsonMessage);
    Serial.print("jsonMessage");
    Serial.println(jsonMessage);

    if (!error) {
      const char* relayState = doc["relay"];
      if (String(relayState) == "on") {
        // Aciona o relé 
        Serial.println("ON");
        handleRelayOn(clientID);
      } else if (String(relayState) == "off") {
         Serial.println("off");
        // Desliga o relé
        handleRelayOff(clientID);
      }
    }
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(relayPin, OUTPUT);
  digitalWrite(relayPin, HIGH);

  for (byte i = 0; i < gpiosCount; i++) {
    pinMode(gpios[i], OUTPUT);
    digitalWrite(gpios[i], HIGH);
    Serial.println(gpios[i], DEC);
  }
  WiFi.begin(ssid, password);
WiFi.config(ip, gateway, subnet);
  while (WiFi.status() != WL_CONNECTED) {
    statusWifi = !statusWifi;
    digitalWrite(relayPin, statusWifi);
    delay(1000);
    Serial.println("Conectando ao WiFi...");
  }
  digitalWrite(relayPin, HIGH);
  Serial.println("Conectado ao WiFi");
  Serial.print("Endereço IP: ");
  Serial.println(WiFi.localIP());

  webSocket.begin();
  webSocket.onEvent(handleWebSocketMessage);
}

void loop() {
  webSocket.loop();  // Processa a comunicação WebSocket
}
