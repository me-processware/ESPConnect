# ESP32 WiFi Diagnostic Tool

**No Serial Monitor Needed - Just Connect to WiFi!**

A comprehensive hardware diagnostic tool for ESP32 that creates its own WiFi Access Point and serves a beautiful web interface for viewing all hardware information.

---

## 🌟 Features

### WiFi Access Point
- **SSID**: `ESP32-Diagnostics`
- **Password**: `diagnostic123`
- **Auto-redirect** to diagnostic page (captive portal)
- **mDNS**: Access via `http://esp32-diag.local`

### Web Interface
- 📱 **Responsive design** - Works on phone, tablet, desktop
- 🎨 **Beautiful UI** - Modern gradient design
- 📊 **Real-time data** - Refresh button updates all info
- 💾 **Export JSON** - Download diagnostic data
- 🔄 **Auto-refresh** - No page reload needed

### Diagnostic Information

**Chip Information:**
- Model (ESP32, S2, S3, C3, C6, H2)
- Revision
- CPU cores
- Frequency
- MAC address
- Features (WiFi, Bluetooth, BLE)

**Memory Analysis:**
- Heap size, free, used
- Heap usage percentage with progress bar
- PSRAM (if available)
- Flash size and speed

**GPIO Configuration:**
- I2C SDA/SCL pins
- Default pin assignments

**I2C Bus Scan:**
- Automatic device detection
- Common device identification
- Address and name table

**System Information:**
- SDK version
- Uptime counter

---

## 🚀 Quick Start

### 1. Flash Firmware

**Via ESPConnect:**
1. Open ESPConnect web interface
2. Connect your ESP32
3. Go to Flash Firmware tab
4. Select `diagnostic-wifi-esp32.bin` (or your chip variant)
5. Offset: `0x10000`
6. Click Program

**Via PlatformIO:**
```bash
# Build and upload
pio run -e esp32dev --target upload

# For other variants
pio run -e esp32-c3 --target upload
pio run -e esp32-s2 --target upload
pio run -e esp32-s3 --target upload
```

### 2. Connect to WiFi

1. ESP32 boots and creates WiFi AP
2. On your phone/laptop, connect to:
   - **SSID**: `ESP32-Diagnostics`
   - **Password**: `diagnostic123`

### 3. Open Browser

The diagnostic page should open automatically (captive portal).

If not, navigate to:
- `http://192.168.4.1`
- Or `http://esp32-diag.local`

### 4. View Diagnostics!

You'll see a beautiful dashboard with all hardware information:
- Chip specs
- Memory usage
- I2C devices
- System info

---

## 📱 User Experience

```
1. Flash firmware
        ↓
2. ESP32 creates WiFi AP
        ↓
3. Connect phone to "ESP32-Diagnostics"
        ↓
4. Browser auto-opens diagnostic page
        ↓
5. View beautiful hardware dashboard
        ↓
6. Export JSON if needed
```

**Total time**: ~30 seconds from flash to viewing data!

---

## 🔧 Configuration

### Change WiFi Credentials

Edit `src/main.cpp`:

```cpp
const char* AP_SSID = "ESP32-Diagnostics";     // Change SSID
const char* AP_PASSWORD = "diagnostic123";     // Change password
```

### Change I2C Pins

Edit `src/main.cpp`:

```cpp
#define I2C_SDA 21  // Change SDA pin
#define I2C_SCL 22  // Change SCL pin
```

### Change mDNS Name

Edit `src/main.cpp`:

```cpp
const char* MDNS_NAME = "esp32-diag";  // Access via http://esp32-diag.local
```

---

## 📊 Firmware Sizes

| Chip | Firmware Size | RAM Usage | Flash Usage |
|------|---------------|-----------|-------------|
| ESP32 | 837 KB | 14.1% (46 KB) | 64.8% (850 KB) |
| ESP32-C3 | 892 KB | 12.5% (41 KB) | 66.7% (875 KB) |
| ESP32-S2 | 762 KB | 13.5% (44 KB) | 62.1% (738 KB) |
| ESP32-S3 | 782 KB | 13.9% (45 KB) | 23.9% (800 KB) |

---

## 🌐 API Endpoint

**GET** `/api/diagnostics`

Returns JSON with all diagnostic data:

```json
{
  "chip": {
    "model": "ESP32-S3",
    "revision": 0,
    "cores": 2,
    "frequency": 240,
    "features": "WiFi BLE",
    "mac": "AA:BB:CC:DD:EE:FF"
  },
  "memory": {
    "heap_size": 393216,
    "heap_free": 347168,
    "heap_used": 46048,
    "heap_usage_percent": 11,
    "psram_size": 8388608,
    "psram_free": 8388608,
    "flash_size": 16777216,
    "flash_speed": 80
  },
  "gpio": {
    "i2c_sda": 21,
    "i2c_scl": 22
  },
  "i2c_devices": [
    {
      "address": "0x3C",
      "name": "OLED Display (SSD1306)"
    }
  ],
  "system": {
    "sdk_version": "v5.1.2",
    "uptime_ms": 12345,
    "uptime_sec": 12
  }
}
```

---

## 🛠️ Development

### Project Structure

```
esp32-diagnostic-wifi/
├── platformio.ini          # PlatformIO configuration
├── src/
│   └── main.cpp           # Main firmware (embedded HTML)
└── README.md              # This file
```

### Dependencies

- **ESP Async WebServer** - Async web server
- **AsyncTCP** - Async TCP library
- **ArduinoJson** - JSON serialization

All dependencies auto-installed by PlatformIO.

### Build Commands

```bash
# Build all variants
pio run

# Build specific variant
pio run -e esp32dev
pio run -e esp32-c3
pio run -e esp32-s2
pio run -e esp32-s3

# Upload
pio run -e esp32dev --target upload

# Monitor serial output
pio device monitor
```

---

## 🐛 Troubleshooting

### Can't Connect to WiFi

**Problem**: WiFi AP not visible

**Solutions**:
- Wait 5-10 seconds after boot
- Check serial monitor for AP IP address
- Restart ESP32
- Check WiFi is enabled on your device

### Page Doesn't Load

**Problem**: Connected to WiFi but page won't load

**Solutions**:
- Try `http://192.168.4.1` directly
- Try `http://esp32-diag.local`
- Disable mobile data (use WiFi only)
- Clear browser cache

### No I2C Devices Found

**Problem**: I2C scan shows no devices

**Solutions**:
- Check I2C wiring (SDA, SCL, GND, VCC)
- Add 4.7kΩ pull-up resistors on SDA and SCL
- Verify device has power
- Check I2C address (some devices have configurable addresses)
- Try different I2C pins (edit firmware)

### Serial Monitor Shows Errors

**Problem**: Errors in serial output

**Solutions**:
- Baud rate: 115200
- Check USB cable (must support data)
- Try different USB port
- Update ESP32 drivers

---

## 📝 Serial Monitor Output

When firmware boots, you'll see:

```
=== ESP32 WiFi Diagnostic Tool ===
Author: Processware
Version: 1.0.0

Starting WiFi Access Point...
AP IP address: 192.168.4.1
SSID: ESP32-Diagnostics
Password: diagnostic123

mDNS started: http://esp32-diag.local

Web server started!
========================================
Connect to WiFi and open browser:
  1. Connect to WiFi: ESP32-Diagnostics
  2. Password: diagnostic123
  3. Open: http://192.168.4.1
  4. Or: http://esp32-diag.local
========================================

Scanning I2C bus...
  Found: 0x3C - OLED Display (SSD1306)
  Found: 0x68 - RTC/IMU (DS3231/MPU6050)
I2C scan complete. Found 2 devices.
```

---

## 🎨 Screenshots

### Main Dashboard
Beautiful gradient design with chip info, memory usage, GPIO config, and I2C devices.

### Mobile Responsive
Works perfectly on phones with responsive layout.

### Export JSON
Download complete diagnostic data as JSON file.

---

## 🔒 Security Notes

**Default Password**: `diagnostic123`

⚠️ **Change the password** in production environments!

The WiFi AP is open by default for easy testing. For security:
1. Change `AP_PASSWORD` to a strong password
2. Use WPA2 encryption (default)
3. Don't expose to untrusted networks

---

## 📦 Pre-compiled Binaries

Available in `/bin/` folder:
- `diagnostic-wifi-esp32.bin` (837 KB)
- `diagnostic-wifi-esp32-c3.bin` (892 KB)
- `diagnostic-wifi-esp32-s2.bin` (762 KB)
- `diagnostic-wifi-esp32-s3.bin` (782 KB)

Flash at offset `0x10000`.

---

## 🤝 Contributing

Improvements welcome! Areas for contribution:
- Additional chip support (C6, H2, P4)
- More I2C device identification
- WiFi station mode (connect to existing WiFi)
- OTA firmware updates
- Data logging to SD card
- Multi-language support

---

## 📄 License

MIT License - Free to use and modify.

---

## 👨‍💻 Author

**Processware**  
Email: dev@processware.io  
Version: 1.0.0  
Date: December 2025

---

## 🙏 Credits

- **ESP Async WebServer** by me-no-dev
- **AsyncTCP** by me-no-dev
- **ArduinoJson** by Benoit Blanchon
- **ESPConnect** integration by Processware

---

**Built with ❤️ for the ESP32 community**
