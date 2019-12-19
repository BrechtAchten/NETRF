/*

*/
#include "WiFi.h"
#include <PubSubClient.h>
#include <EEPROM.h>

#define EEPROM_SIZE 3

const char* ssid = "DreamTeam";
const char* password = "TomBrecht";

const char* mqtt_server = "192.168.4.1";
const char* mainSetupTopic = "iot/setup/";
const char* mainTopic = "iot/data/";
char lightTopic[255];
const char* lightTopicId = "/0";

uint32_t thingId = 1;
char thingIdBuffer[10];
int thingIdAddress = 0;
bool setupComplete = false;
long lastMsg;

void callback(char* topic, byte* payload, unsigned int length) {
  if(!setupComplete){
    String message = "";
    for(int i = 0; i < length; i++){
      message += (char) payload[i];
    }
    if(message == "OK"){
      setupComplete = true;
      EEPROM.write(2, setupComplete);
      EEPROM.commit();
    }
    else if(message == "NOK"){
      thingId = 0;
      setup_MQTT();
    }
    Serial.println(message);
  }
  else{
    
  }
}

WiFiServer server(80);

WiFiClient espClient;
PubSubClient client(espClient);

void connect_WiFi() {
  WiFi.begin(ssid, password);
  Serial.print("Connecting to wifi...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.println("Connected to WiFi network...");
  Serial.println(WiFi.macAddress());
}

void connect_MQTT() {
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  Serial.print("Connecting to mqtt broker...");
  while (!client.connected()) {
    delay(500);
    if (!client.connect("ESP32Client")) {
      Serial.print("failed, rc=");
      Serial.println(client.state());
    }
  }
  Serial.println();
  Serial.println("Connected to mqtt broker");
}

void setup_MQTT(){
  thingId = 0;
  char setupTopic[14];
  if(!setupComplete){
      if(thingId == 0){
        thingId = random(32,767);
        EEPROM.write(0, (byte) thingId);
        EEPROM.write(1, (byte) thingId >> 8);
        EEPROM.commit();
      }
      char charThingId[15];
      sprintf(charThingId, "Thing id: %X", thingId);
      Serial.println(charThingId);
      sprintf(setupTopic, "%S%04X", mainSetupTopic, thingId);
      client.publish(setupTopic, "setup");
      sprintf(lightTopic, "%S%X/0", mainTopic, thingId);
      delay(1000);
  }
  Serial.println(setupTopic);
  client.subscribe(setupTopic);
}

void setup()
{
  thingId = (int) EEPROM.read(0) + (EEPROM.read(1) << 8);
  setupComplete = EEPROM.read(2);
  Serial.println("Thing id: " + String(thingId));
  Serial.begin(115200);
  connect_WiFi();
  connect_MQTT();
  client.subscribe("test");
  setup_MQTT();
  Serial.println("Setup");
  pinMode(13, OUTPUT);
  digitalWrite(13, HIGH);
  pinMode(34, INPUT);

  EEPROM.begin(EEPROM_SIZE);
}

void sendValues() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi connection lost");
    connect_WiFi();
  }
  if (!client.connected()) {
    Serial.println("Connection to MQTT lost");
    connect_MQTT();
  }
  int lightValue = analogRead(34);
  String lightString = String(lightValue);
  char lightBuffer[10];
  lightString.toCharArray(lightBuffer, 10);
  Serial.println(lightBuffer);
  client.publish(lightTopic, lightBuffer);
}

void loop()
{
  //setup_MQTT();
  client.loop();

  long now = millis();
  if(thingId > 0 && setupComplete && now - lastMsg > 5000){
    lastMsg = now;
    sendValues();
  }
}
