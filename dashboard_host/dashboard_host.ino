#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

cconst char *ssid = "SSID";
const char *password = "Password";

ESP8266WebServer server(80);

// Static IP for Wemos D1 #2
IPAddress local_IP(192, 168, 1, 203);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);

// HTML Dashboard
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <title>Home Dashboard</title>
  <style>
    body {
      background-color: #0f0f1a;
      color: #00ffcc;
      font-family: 'Courier New', monospace;
      text-align: center;
    }
    .card {
      background: #1a1a2e;
      margin: 1em auto;
      padding: 1em;
      border-radius: 10px;
      width: 90%%;
      max-width: 600px;
      box-shadow: 0 0 10px #00ffcc44;
    }
    img {
      width: 100%%;
      border-radius: 10px;
    }
    .sensor {
      font-size: 1.5em;
      margin: 0.5em 0;
    }
  </style>
</head>
<body>
  <h1>Home Dashboard</h1>
  <div class="card">
    <div class="sensor" id="temp"> Temp: -- °C</div>
    <div class="sensor" id="humidity"> Humidity: -- %%</div>
    <div class="sensor" id="motion"> Motion: --</div>
  </div>
  <div class="card">
    <h3>📷 Live Feed</h3>
    <img src="http://192.168.1.202:81/stream" id="cam">

  </div>

  <script>
    async function updateSensors() {
      try {
        const res = await fetch("http://192.168.1.201/data");
        const data = await res.json();
        document.getElementById("temp").innerHTML = ` Temp: ${data.temperature} °C`;
        document.getElementById("humidity").innerHTML = ` Humidity: ${data.humidity} %%`;
        document.getElementById("motion").innerHTML = ` Motion: ${data.motion ? "Detected" : "None"}`;
      } catch (e) {
        console.log("Failed to fetch sensor data");
      }
    }
    setInterval(updateSensors, 166); // 1/6 second
    updateSensors();
  </script>
</body>
</html>
)rawliteral";

void setup() {
  Serial.begin(115200);
  WiFi.config(local_IP, gateway, subnet);
  WiFi.begin(ssid, password);
  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500); Serial.print(".");
  }
  Serial.println("\n✅ Dashboard online at http://192.168.1.203");

  server.on("/", []() {
    server.send_P(200, "text/html", index_html);
  });

  server.begin();
}

void loop() {
  server.handleClient();
}
