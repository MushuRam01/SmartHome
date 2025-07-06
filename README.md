# 🏠 Local Home Monitoring System

A self-contained home monitoring setup using ESP microcontrollers — **no internet needed**. It includes:

- 📷 ESP32-CAM video stream
- 🌡️ Temperature + 💧 Humidity via DHT11
- 🚶 Motion detection via PIR sensor
- 📺 Sci-fi themed local dashboard served from Wemos D1
- 📡 All devices connected via Wi-Fi with **static IPs**

---

## 🔧 Components

| Device            | Function                    | Static IP        |
|-------------------|-----------------------------|------------------|
| **ESP32-CAM**     | Streams camera feed         | `192.168.1.202`  |
| **Wemos D1 #1**   | Reads DHT11 + PIR sensors   | `192.168.1.201`  |
| **Wemos D1 #2**   | Hosts dashboard web page    | `192.168.1.203`  |

---
## 🌐 Dashboard (Wemos D1 #2)

- Visit: [`http://192.168.1.203`](http://192.168.1.203) while on the same local network as the microcontrollers 
- Updates every **1/6th of a second**
- Shows:
  -  Temperature (`DHT11`)
  -  Humidity (`DHT11`)
  -  Motion (`PIR`)
  -  Live ESPCamera feed (`ESP32-CAM`)

---

## 📷 ESP32-CAM


### Custom Changes:
- Set board: `AI Thinker ESP32-CAM`
- Set static IP:

```cpp
IPAddress local_IP(192, 168, 1, 202);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);
WiFi.config(local_IP, gateway, subnet);

