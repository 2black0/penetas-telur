# 🐣 ESP8266-Based Smart Egg Incubator with Web Dashboard

A compact, standalone **egg incubator controller** using an **ESP8266 (NodeMCU)**. This project features a built-in web server that enables monitoring and controlling temperature, humidity, and actuator states through a web dashboard — no external cloud service required.

> Designed for embedded systems learning, small-scale poultry farms, or DIY automation enthusiasts.

---

## 📦 Features

✅ Wi-Fi Access Point mode with captive web dashboard  
✅ DHT11 sensor for temperature and humidity readings  
✅ Control of:
- 🔥 Heating lamp (temperature)
- 💨 Mist maker (humidity)
✅ Manual & automatic mode switching  
✅ EEPROM-based configuration persistence  
✅ HTML UI with real-time sensor display and setpoint adjustment

---

## 🖥️ Live Web Interface

Accessible via browser at: `http://192.168.4.1`  
When connected to the ESP's Wi-Fi hotspot (`EggWifi`)

The dashboard includes:
- 🌡️ Temperature & humidity monitor
- 🧠 Auto/manual control toggle
- 🔧 Setpoint adjustments (Max/Min Temp & Humidity)
- 🔘 Control buttons for lamp & mist
- 🔄 Refresh option

> Preview:
> ![Web UI Preview](https://raw.githubusercontent.com/yourusername/yourrepo/main/preview.png) *(Optional)*

---

## 🔌 Hardware Requirements

| Component            | Description                 |
|---------------------|-----------------------------|
| ESP8266 (NodeMCU)    | Main microcontroller         |
| DHT11 Sensor         | Temperature & humidity input |
| Relay Modules (x2)   | For lamp and mist control    |
| Heating Lamp         | Maintains warmth             |
| Mist Maker           | Regulates humidity           |
| 5V Power Supply      | For ESP and actuators        |

---

## 🧠 Functional Overview

- **Automatic Mode:** Lamp & mist maker are activated based on current sensor values and setpoints.
- **Manual Mode:** User directly toggles lamp and mist maker from the dashboard.
- **Settings Storage:** Temperature and humidity thresholds are stored in EEPROM to persist across restarts.

---

## 📁 File Structure

```

.
├── LICENSE
├── README.md
└── Project
└── main.ino          # Main firmware code

```

---

## 🔧 Configuration Parameters

Inside the code (`main.ino`):

```cpp
const char* ssid = "EggWifi";           // Wi-Fi name
const char* pass = "987654321";         // Wi-Fi password

uint8_t DHTPin = 2;
uint8_t lampPin = 4;
uint8_t mistPin = 5;

int feedtempMin = 36;  // Set your default temperature and humidity range
int feedtempMax = 39;
int feedhumMin  = 55;
int feedhumMax  = 70;
```

You can modify these values via the dashboard as well.

---

## 🚀 Getting Started

1. **Install Libraries:**

   * `ESP8266WiFi`
   * `ESP8266WebServer`
   * `DHT`
   * `EEPROM`

2. **Upload Code:**

   * Open `main.ino` in Arduino IDE
   * Choose the correct board (NodeMCU 1.0) and port
   * Upload the sketch

3. **Connect to Wi-Fi:**

   * Search for Wi-Fi: `EggWifi`
   * Connect using password: `987654321`
   * Open browser → go to `http://192.168.4.1`

---

## 🔐 Security Note

* This version uses **open local-only network**.
* Not encrypted or authenticated — suitable for controlled environments.

---

## 🧑‍💻 Author

**Ardy Seto Priambodo**
📧 [2black0@gmail.com](mailto:2black0@gmail.com)
🔗 [github.com/2black0](https://github.com/2black0)

---

## 📜 License

This project is licensed under the [MIT License](LICENSE).

---

## 🌟 Contribute or Support

If this project helps you, you can:

* ⭐ Star this repository
* 🛠️ Fork and modify for your own incubator
* 🐞 Report issues or improvements

---

## 📷 Coming Soon

* 📊 Real-time chart version
* 📱 Mobile-friendly layout
* 🧪 Logging temperature/humidity trends (SPIFFS or SD card)

Happy hatching! 🐥