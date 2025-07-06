#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <DHT.h>

// Wi-Fi setup
const char *ssid = "SSID";
const char *password = "Password";

ESP8266WebServer server(80);

// Sensor setup
#define DHTPIN D4
#define DHTTYPE DHT11
#define PIRPIN D3

DHT dht(DHTPIN, DHTTYPE);

void handleData() {
  float temp = dht.readTemperature();
  float hum = dht.readHumidity();
  int motion = digitalRead(PIRPIN);

  String json = "{\"temperature\":" + String(temp) +
                ",\"humidity\":" + String(hum) +
                ",\"motion\":" + String(motion) + "}";

  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.send(200, "application/json", json);
}

void setup() {
  Serial.begin(115200);
  pinMode(PIRPIN, INPUT);
  dht.begin();

  WiFi.config(IPAddress(192,168,1,201), IPAddress(192,168,1,1), IPAddress(255,255,255,0));
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) delay(500);

  server.on("/data", handleData);
  server.begin();
}

void loop() {
  server.handleClient();
}
