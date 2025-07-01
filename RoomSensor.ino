#include <ESP8266WiFi.h>
#include <DHT.h>

#define DHTPIN D4
#define PIRPIN D5
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);

const char* ssid = "SSID";         // Replace with SSID
const char* password = "Password"; // Replace with password

IPAddress local_IP(192, 168, 1, 200);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);

WiFiServer server(80);

void setup() {
  Serial.begin(115200);
  dht.begin();
  pinMode(PIRPIN, INPUT);

  if (!WiFi.config(local_IP, gateway, subnet)) {
    Serial.println("⚠️ Static IP config failed.");
  }

  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\n✅ Connected to WiFi");
  Serial.print("🌐 Access: http://");
  Serial.println(WiFi.localIP());

  server.begin();
}

void loop() {
  WiFiClient client = server.available();
  if (!client) return;

  Serial.println("📥 Client connected");

  String req = client.readStringUntil('\r');
  client.readStringUntil('\n');
  Serial.println("🔹 Request: " + req);

  float temp = dht.readTemperature();
  float hum = dht.readHumidity();
  bool motion = digitalRead(PIRPIN);

  if (req.indexOf("/data") != -1) {
    String json = "{\"temperature\":";
    json += (isnan(temp) ? -999 : temp);
    json += ",\"humidity\":";
    json += (isnan(hum) ? -999 : hum);
    json += ",\"motion\":";
    json += (motion ? "true" : "false");
    json += "}";

    client.println("HTTP/1.1 200 OK");
    client.println("Content-Type: application/json");
    client.print("Content-Length: ");
    client.println(json.length());
    client.println("Connection: close");
    client.println();
    client.print(json);

    client.flush();
    delay(1);
    client.stop();
    Serial.println("📤 Sent JSON data");
  } else {
    client.println("HTTP/1.1 200 OK");
    client.println("Content-Type: text/html");
    client.println("Connection: close");
    client.println();

    client.println(R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <meta charset="UTF-8">
  <title>Door Sensor</title>
  <style>
    body { font-family: Arial; background: #f2f2f2; text-align: center; padding-top: 40px; }
    .card {
      background: white; border-radius: 12px; box-shadow: 0 4px 10px rgba(0,0,0,0.1);
      display: inline-block; padding: 30px; width: 280px; margin-top: 10px;
    }
    h1 { margin-bottom: 15px; }
    p { font-size: 1.3em; }
    .motion { color: green; font-weight: bold; }
    .nomotion { color: gray; font-weight: bold; }
  </style>
</head>
<body>
  <div class="card">
    <h1>Door Sensor</h1>
    <p>🌡 Temp: <span id="temp">--</span> °C</p>
    <p>💧 Humidity: <span id="hum">--</span> %</p>
    <p>🕴 Motion: <span id="motion" class="nomotion">--</span></p>
  </div>
<script>
  function update() {
    fetch("/data")
      .then(res => res.json())
      .then(data => {
        document.getElementById("temp").textContent = data.temperature;
        document.getElementById("hum").textContent = data.humidity;
        const motionEl = document.getElementById("motion");
        if (data.motion) {
          motionEl.textContent = "Motion detected!";
          motionEl.className = "motion";
        } else {
          motionEl.textContent = "No motion";
          motionEl.className = "nomotion";
        }
      })
      .catch(err => console.error("Fetch error:", err));
  }

  setInterval(update, 5000);
  update();
</script>
</body>
</html>
)rawliteral");

    client.flush();
    delay(1);
    client.stop();
  }
}
