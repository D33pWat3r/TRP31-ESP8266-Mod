#include <ESP8266WebServer.h>
#include <ArduinoOTA.h>
const char* ssid = "Dreierpack";          //Zugangsdaten deines Routers einfügen
const char* password = "1840031413996665";
const int LEDPIN1 = D0; // Oneboard Led Nodemcu
const int LEDPIN2 = D6; // Oneboard Led Esp8266
const int statusTherme = D5;
bool ledState = LOW;
ESP8266WebServer server(80);

void handleRoot() {                  //Html Startseite
  if (server.hasArg("ledoff")) {
    digitalWrite(LEDPIN1, 1);        
    digitalWrite(LEDPIN2, 1);        
    ledState = HIGH;
    Serial.println("LED aus");
  }
  if (server.hasArg("ledon")) {
    digitalWrite(LEDPIN1, 0);      
    digitalWrite(LEDPIN2, 0);        
    ledState = LOW;
    Serial.println("LED ein");
  }
  String content = "<!DOCTYPE html><html lang='de'><head><meta charset='UTF-8'><meta name= viewport content='width=device-width, initial-scale=1.0,' user-scalable=yes>";
  if (digitalRead(statusTherme))content += "<title>Onboard Led</title>\r\n<style type='text/css'><!-- .button1 {height:40px; width:120px; background-color: #268FFF; font-size:16px} .button2 {height:40px; width:120px; background-color: #FE0320; font-size:16px} --></style></head>\r\n<body><h2>Therme ist an</h2><p>";
  else content += "<title>Onboard Led</title>\r\n<style type='text/css'><!-- .button1 {height:40px; width:120px; background-color: #268FFF; font-size:16px} .button2 {height:40px; width:120px; background-color: #FE0320; font-size:16px} --></style></head>\r\n<body><h2>Therme ist aus</h2><p>";
  if (ledState == HIGH) {
    content += "LED ist aus<p><form action='/' method='POST'><input class='button1' name='ledon' type='submit' value='LED Ein'></form></html>";
  }
  else {
    content += "LED ist an<p><form action='/' method='POST'><input class='button2' name='ledoff' type='submit' value='LED Aus'></form></html>";
  }
  server.send(200, "text/html", content);
}

void setup(void) {
  pinMode(LEDPIN1, OUTPUT);
  pinMode(LEDPIN2, OUTPUT);
  pinMode(statusTherme, INPUT);
  digitalWrite(LEDPIN1, LOW);
  digitalWrite(LEDPIN2, LOW);
  Serial.begin(250000);
  WiFi.mode(WIFI_STA);         //Stationst-Modus --> Esp8266 im Heimnetzwerk einloggen
  // WiFi.mode(WIFI_AP_STA);      //Stations- und Soft-Access-Point-Modus --> Access-Point Adresse http://192.168.4.1/
  WiFi.begin(ssid, password);
  Serial.println("");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("SSID: ");
  Serial.println(ssid);
  Serial.print("Passwort: ");
  Serial.println(password);
  server.on("/", handleRoot);
  server.onNotFound([]() {
    server.sendContent("HTTP/1.1 303 OK\r\nLocation: /\r\nCache-Control: no-cache\r\n\r\n");
  });
  server.begin();
  Serial.println("HTTP server started");
  ArduinoOTA.begin();                      // https://arduino-esp8266.readthedocs.io/en/latest/ota_updates/readme.html
}

void loop(void) {
  ArduinoOTA.handle();
  server.handleClient();
  // Füge hier deinen Code ein:
}
