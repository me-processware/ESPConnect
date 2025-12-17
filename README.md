# ESPConnect Flask - Complete Package

**Author**: Processware  
**Version**: 3.0.0  
**Description**: Flask-powered ESPConnect v1.09 with integrated ESP32 diagnostic tools

---

## 🚀 What's New in v3.0

**Major Update: Merged with upstream ESPConnect v1.09!**

### Upstream Features (v1.06-v1.09)
✅ **Fixed flash size determination** - More reliable flash operations  
✅ **CH340 baud auto-lower** - Automatically uses 460,800 for CH340 chips  
✅ **Pause/Resume Serial Monitor** - Freeze output temporarily  
✅ **More chip support** - ESP32-C2, C5, C6, C61, H2, P4, P4R3  
✅ **Espressif documentation links** - Direct links to datasheets and TRM  
✅ **Better USB bridge detection** - Improved hardware compatibility  
✅ **Higher baud rates** - Faster flashing options  
✅ **LittleFS folder navigation** - Browse folders in LittleFS  
✅ **WebSerial_ESPTool updates** - Latest flashing improvements  

### Processware Features (Preserved)
✅ **Flask Backend** - Serves ESPConnect on port 8080  
✅ **Diagnostic Tab** - Hardware analysis with auto-flash  
✅ **Auto-Capture** - Automatic diagnostic data collection  
✅ **Pre-compiled Firmware** - Ready-to-flash diagnostic bins  
✅ **I2C Scanner** - Detect and identify I2C devices  
✅ **REST API** - Programmatic access to diagnostics  

---

## 📋 What Was in v2.0

Complete rewrite with powerful diagnostic capabilities:

✅ **Flask Backend** - Serves ESPConnect Vue.js frontend on port 8080  
✅ **Diagnostic API** - REST endpoints for ESP32 hardware analysis  
✅ **Pre-compiled Firmware** - Ready-to-flash `.bin` files (ESP32, S2, S3, C3)  
✅ **I2C Scanner** - Detect and identify connected I2C devices  
✅ **Web Integration** - Beautiful diagnostic display in browser  
✅ **JSON Output** - Structured data for easy parsing  

---

## ⚡ Quick Start (3 Minutes)

```bash
# Install dependencies
pip install -r requirements.txt

# Run server
python app.py

# Open browser → http://localhost:8080
# Flash firmware → bin/diagnostic-esp32.bin
# View diagnostics → Serial Monitor (115200 baud)
```

---

## 📦 Package Contents

```
espconnect-flask/
├── app.py                      # Flask server with diagnostic API
├── static/                     # ESPConnect Vue.js frontend (9.2 MB)
├── bin/                        # Pre-compiled diagnostic firmware (1.1 MB)
│   ├── diagnostic-esp32.bin    # ESP32 (291 KB)
│   ├── diagnostic-esp32-s2.bin # ESP32-S2 (250 KB)
│   ├── diagnostic-esp32-s3.bin # ESP32-S3 (278 KB)
│   └── diagnostic-esp32-c3.bin # ESP32-C3 (271 KB)
├── firmware/                   # Diagnostic tool source code
│   └── esp32-diagnostic-tool/
│       ├── src/main.cpp        # Complete firmware (500+ lines)
│       └── platformio.ini
├── docs/                       # Documentation
│   ├── INTEGRATION_GUIDE.md
│   └── DiagnosticTab.vue
├── Dockerfile
├── docker-compose.yml
└── DEPLOYMENT.md
```

---

## 🔥 Key Features

### ESPConnect (Original)
- Flash ESP32 firmware via browser
- Read/write flash memory
- Partition management
- Serial monitor
- File system management
- No drivers needed

### Diagnostic Tool (New!)
- ✅ Chip info (model, cores, frequency, MAC)
- ✅ Memory analysis (Heap, PSRAM, Flash)
- ✅ GPIO pinout with warnings
- ✅ **I2C scanner** with device identification
- ✅ System info (SDK, uptime)
- ✅ JSON output for web integration

### Flask Backend (New!)
- ✅ REST API for diagnostic data
- ✅ CORS support
- ✅ Health check endpoint
- ✅ I2C device database
- ✅ Docker ready

---

## 📡 Diagnostic API

### POST /api/diagnostic/parse
Parse serial output and extract JSON

### GET /api/diagnostic/list
List all stored diagnostics

### GET /api/diagnostic/<device_id>
Get specific diagnostic data

### POST /api/diagnostic/upload
Upload diagnostic JSON

### DELETE /api/diagnostic/<device_id>
Delete diagnostic data

### GET /api/diagnostic/i2c-devices
Get I2C device database

**Full API docs**: `docs/INTEGRATION_GUIDE.md`

---

## 🔧 Flash Diagnostic Firmware

### Step 1: Choose Your Board

| Board | Firmware File |
|-------|---------------|
| ESP32 DevKit, WROOM | `bin/diagnostic-esp32.bin` |
| ESP32-S2 | `bin/diagnostic-esp32-s2.bin` |
| ESP32-S3 | `bin/diagnostic-esp32-s3.bin` |
| ESP32-C3 | `bin/diagnostic-esp32-c3.bin` |

### Step 2: Flash via ESPConnect

1. Open **http://localhost:8080**
2. **Connect** → Select ESP32
3. **Flash Firmware** tab
4. Choose `.bin` file
5. Offset: **0x10000**
6. Click **Program**

### Step 3: View Results

1. **Serial Monitor** tab
2. Baud: **115200**
3. Press **Reset**
4. Copy JSON output

---

## 📊 Diagnostic Output Example

```json
{
  "diagnostic_tool": {
    "chip": {
      "model": "ESP32",
      "cores": 2,
      "frequency_mhz": 240,
      "mac_address": "24:6F:28:XX:XX:XX"
    },
    "memory": {
      "heap": { "total_kb": 327, "free_kb": 298 },
      "flash": { "size_mb": 4, "speed_mhz": 40 }
    },
    "i2c_scan": {
      "devices": [
        { "address": "0x3c", "likely": "OLED Display (SSD1306)" },
        { "address": "0x68", "likely": "MPU6050 IMU" }
      ]
    }
  }
}
```

---

## 🐳 Docker Deployment

```bash
docker-compose up -d
```

Access at **http://localhost:8080**

---

## 🌐 Production Deployment

### With Gunicorn
```bash
pip install gunicorn
gunicorn -w 4 -b 0.0.0.0:8080 wsgi:app
```

### With Nginx
```nginx
server {
    listen 80;
    location / {
        proxy_pass http://localhost:8080;
    }
}
```

**Full guide**: `DEPLOYMENT.md`

---

## 🎯 Supported Boards

| Board | Status |
|-------|--------|
| ESP32 (WROOM, DevKit) | ✅ Tested |
| ESP32-S2 | ✅ Tested |
| ESP32-S3 | ✅ Tested |
| ESP32-C3 | ✅ Tested |
| ESP32-C6 | ⚠️ Arduino support pending |

---

## 🌐 Browser Requirements

**Chromium-based only:**
- ✅ Chrome, Edge, Brave, Opera
- ❌ Firefox, Safari (no Web Serial API)

---

## 🛠️ Build Custom Firmware

```bash
cd firmware/esp32-diagnostic-tool
# Edit src/main.cpp
pio run -e esp32dev
# Binary: .pio/build/esp32dev/firmware.bin
```

---

## 🛠️ Troubleshooting

**Can't connect:** Use Chromium browser, check USB cable  
**Flash fails:** Hold BOOT button, check offset is 0x10000  
**I2C empty:** Check wiring, add 4.7kΩ pull-ups  
**API errors:** Check Flask server logs  

---

## 📚 Documentation

- `docs/INTEGRATION_GUIDE.md` - Web integration tutorial
- `DEPLOYMENT.md` - Production deployment
- `firmware/esp32-diagnostic-tool/FLASH_NOW.md` - Firmware guide
- `bin/README.md` - Pre-compiled firmware docs

---

## 🔄 Version History

### v2.0.0 (December 2025)
- Flask backend with diagnostic API
- Pre-compiled firmware for all ESP32 variants
- I2C scanner with device identification
- Web integration components
- Docker support

### v1.0.0 (Original)
- Vue.js frontend
- Web Serial API
- Flash programming

---

## 📄 License

MIT License

---

## 🙏 Credits

- **Author**: Processware
- **Original ESPConnect**: thelastoutpostworkshop
- **Framework**: Flask + Vue.js + PlatformIO

---

## 🚀 Get Started!

```bash
git clone https://github.com/me-processware/ESPConnect.git
cd ESPConnect
pip install -r requirements.txt
python app.py
# Open http://localhost:8080
```

**Built with ❤️ by Processware**
