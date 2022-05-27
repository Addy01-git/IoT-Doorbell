// PubNub MQTT example using ESP8266.
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Servo.h>

// Connection info.
const char* ssid = "WiFi SSID";                   // Enter your WiFi SSID
const char* password =  "Wifi Password";          // Enter your WiFi Password
const char* mqttServer = "mqtt.pndsn.com";
const int mqttPort = 1883;
const char* clientID = "Publish/Subscribe/UUID";  // Keys from PubNub
const char* channelName = "name";                 // PubNub channel name
WiFiClient MQTTclient;
Servo servo;
PubSubClient client(MQTTclient);

void openDoor(){
  Serial.println("Door is open");
  servo.write(90);
  delay(5000);
  Serial.println("Door is closed");
  servo.write(0);
}

void callback(char* topic, byte* payload, unsigned int length) {
  String payload_buff="";
  for (int i=0;i<length;i++) {
    if (payload[i] != 34){
      payload_buff = payload_buff+String((char)payload[i]);
    }
  }

  if (payload_buff == "OpeningTheDoor"){
    openDoor();
  }
}

long lastReconnectAttempt = 0;
boolean reconnect() {
  if (client.connect(clientID)) {
    client.subscribe(channelName); // Subscribe.
  }
  return client.connected();
}

void setup() {
  Serial.begin(9600);
  servo.attach(2); //D4
  servo.write(0); // 0 is the angle
  //servo.write(90);
  Serial.println("Attempting to connect...");
  WiFi.begin(ssid, password); // Connect to WiFi.
  if(WiFi.waitForConnectResult() != WL_CONNECTED) {
      Serial.println("Couldn't connect to WiFi.");
      while(1) delay(100);
  }
  client.setServer(mqttServer, mqttPort); // Connect to PubNub.
  client.setCallback(callback);
  lastReconnectAttempt = 0;
  Serial.println("All systems go! Everything working successfully");
}
void loop() {
  if (!client.connected()) {
    long now = millis();
    if (now - lastReconnectAttempt > 5000) { // Try to reconnect.
      lastReconnectAttempt = now;
      if (reconnect()) { // Attempt to reconnect.
        lastReconnectAttempt = 0;
      }
    }
  } else { // Connected.
    client.loop();
    //client.publish(channelName,"Hello world!"); // Publish message.
    delay(1000);
  }
}
