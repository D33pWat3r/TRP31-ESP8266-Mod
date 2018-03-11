#include <Arduino.h>

#include <Adafruit_Sensor.h>
#include <DHT.h>

#define DHTPIN D7         // Pin which is connected to the DHT sensor.
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

const int time = 60; //time in sec for update dht value over mqtt
unsigned long lastTime = 0;

#include <ESP8266WiFi.h>

#include <WiFiClient.h>
const char* ssid = "YourSSID";
const char* password = "YourWLANPassword";
WiFiClient net;

#include <PubSubClient.h>
const char* mqtt_server = "192.168.1.3";
PubSubClient mqtt(net);

#define switchWaterHeaterPin D0
#define redLEDPin D6

#define signalWaterHeaterPin D5
int signalWaterHeater = 0;

#define SerialOn

void connect();
void callback(char* topic, byte* payload, unsigned int length);
void reconnect();

void setup() {
  #ifdef SerialOn
    Serial.begin(115200);
    delay(1000);
    Serial.println();
    Serial.println();
    Serial.println("init dht sensor");
  #endif

  dht.begin();

  #ifdef SerialOn
    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(ssid);
  #endif

  WiFi.mode(WIFI_AP_STA);
  WiFi.begin(ssid, password);
  connect();

  pinMode(switchWaterHeaterPin,OUTPUT);
  pinMode(redLEDPin,OUTPUT);
  digitalWrite(switchWaterHeaterPin, LOW);
  digitalWrite(redLEDPin, LOW);

  pinMode(signalWaterHeaterPin,INPUT_PULLUP);
  signalWaterHeater = digitalRead(signalWaterHeaterPin);
  #ifdef SerialOn
    Serial.println();
    Serial.print("finish with setup");
    Serial.println();
  #endif
}

void loop() {
    if (!mqtt.connected()) {
      reconnect();
    }
  mqtt.loop();
  char signal[10];

  if (signalWaterHeater != digitalRead(signalWaterHeaterPin)){ //check for update in WaterHeater State
    signalWaterHeater = digitalRead(signalWaterHeaterPin);
    snprintf(signal, 10, "%d",signalWaterHeater);
    mqtt.publish("/Kueche/waterHeater", signal);
    #ifdef SerialOn
      Serial.println();
      Serial.print("signalWaterHeater: ");
      Serial.print(signal);
      Serial.println();
    #endif
  }

  if (millis() - lastTime > time*1000){
      snprintf(signal, 10, "%d",dht.readTemperature());
      mqtt.publish("/Kueche/temp", signal);
      snprintf(signal, 10, "%d",dht.readHumidity());
      mqtt.publish("/Kueche/humidity", signal);
    lastTime = millis();
  }
}

void connect() {

  // WiFi
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    WiFi.begin(ssid, password);
    #ifdef SerialOn
      Serial.println("WiFi connection failed. Retry.");
      Serial.println(WiFi.localIP());
    #endif
  }
  #ifdef SerialOn
    Serial.print("Wifi connection successful - IP-Address: ");
    Serial.println(WiFi.localIP());
  #endif

  // MQTT
  mqtt.setServer(mqtt_server, 1883);
  mqtt.setCallback(callback);
}

void callback(char* topic, byte* payload, unsigned int length) {
  #ifdef SerialOn
    Serial.print("Message arrived [");
    Serial.print(topic);
    Serial.print("] ");

    for (int i = 0; i < length; i++) {
      Serial.print((char)payload[i]);
    }
    Serial.println();
  #endif
  if ((char)payload[0] == '1'){
    digitalWrite(switchWaterHeaterPin, HIGH);
    digitalWrite(redLEDPin, HIGH);
  }
  if ((char)payload[0] == '0') {
    digitalWrite(switchWaterHeaterPin, LOW);
    digitalWrite(redLEDPin, LOW);
  }
}

void reconnect() {
  while (!mqtt.connected()) {
    #ifdef SerialOn
      Serial.print("Attempting MQTT connection...");
      if (mqtt.connect("ESPonGasTherme")) {
        Serial.println("connected");
        mqtt.subscribe("/Kueche/Gastherme");
    #endif
      } else {
      #ifdef SerialOn
        Serial.print("failed, rc=");
        Serial.print(mqtt.state());
        Serial.println(" try again in 5 seconds");
    #endif
    delay(5000);
    }
  }
}
