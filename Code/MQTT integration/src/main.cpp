#include <Arduino.h>

#include <Adafruit_Sensor.h>
#include <DHT.h>

#define DHTPIN D7         // Pin which is connected to the DHT sensor.
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);
float temp=0;
float humidity=0;
const int time = 300; //time in sec for update dht value over mqtt
unsigned long lastTime = 0;

#include <ESP8266WiFi.h>

#include <WiFiClient.h>
const char* ssid = "Dreierpack";
const char* password = "1840031413996665";
WiFiClient net;

#include <PubSubClient.h>
const char* mqtt_server = "192.168.1.3";
PubSubClient mqtt(net);

#define switchWaterHeaterPin D0

#define signalWaterHeaterPin D5
int signalWaterHeater;

#define SerialOn

void connect();
void callback();

void setup() {
  #ifdef SerialOn
    Serial.begin(115200);
    delay(10);
    Serial.println("init dht sensor");
  #endif

  dht.begin();

  #ifdef SerialOn
    Serial.println();
    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(ssid);
  #endif

  WiFi.mode(WIFI_AP_STA);
  WiFi.begin(ssid, password);
  connect()

  pinMode(switchWaterHeaterPin,OUTPUT);
  digitalWrite(switchWaterHeaterPin, LOW);

  pinMode(signalWaterHeaterPin,INPUT);
  signalWaterHeater = digitalRead(signalWaterHeaterPin);
}

void loop() {
    if (!mqtt.connected()) {
    connect();
  }
  mqtt.loop();

  if (signalWaterHeater != digitalRead(signalWaterHeaterPin)){ //check for update in WaterHeater State
    signalWaterHeater = digitalRead(signalWaterHeaterPin);
    mqtt.publish("/Kueche/waterHeater", String(signalWaterHeater));
  }

  temp = dht.readTemperature();
  humidity = dht.readHumidity();
  if (millis() - lastTime > time*1000){
    if (!isnan(humidity) || !isnan(temp)) {
      mqtt.publish("/Kueche/temp", String(temp));
      mqtt.publish("/Kueche/humidity", String(humidity));
    }
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
}

void reconnect() {
  while (!mqtt.connected()) {
    #ifdef SerialOn
      Serial.print("Attempting MQTT connection...");
      if (mqtt.connect(mqtt.c_ clientId.c_str())) {
        Serial.println("connected");
      } else {
        Serial.print("failed, rc=");
        Serial.print(mqtt.state());
        Serial.println(" try again in 5 seconds");
    #endif
    delay(5000);
    }
  }
}
