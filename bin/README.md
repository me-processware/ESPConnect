# ESP32 Diagnostic Firmware Binaries

Pre-compiled diagnostic firmware for ESP32 family chips.

**Author**: Processware  
**Version**: 2.0.0 (WiFi + Serial versions)

---

## 📦 Available Firmware (3.5 MB total)

### ⭐ WiFi Web Interface Version (RECOMMENDED)

Creates WiFi AP and serves beautiful web dashboard. **No serial monitor needed!**

| File | Chip | Size |
|------|------|------|
| `diagnostic-wifi-esp32.bin` | ESP32 | 837 KB |
| `diagnostic-wifi-esp32-s2.bin` | ESP32-S2 | 762 KB |
| `diagnostic-wifi-esp32-s3.bin` | ESP32-S3 | 782 KB |
| `diagnostic-wifi-esp32-c3.bin` | ESP32-C3 | 892 KB |

**WiFi Credentials:**
- SSID: `ESP32-Diagnostics`
- Password: `diagnostic123`
- URL: `http://192.168.4.1`

### Serial JSON Version

Outputs diagnostic data via serial monitor in JSON format.

| File | Chip | Size |
|------|------|------|
| `diagnostic-esp32.bin` | ESP32 | 291 KB |
| `diagnostic-esp32-s2.bin` | ESP32-S2 | 250 KB |
| `diagnostic-esp32-s3.bin` | ESP32-S3 | 278 KB |
| `diagnostic-esp32-c3.bin` | ESP32-C3 | 272 KB |

---

## 🚀 Quick Start - WiFi Version

### 1. Flash Firmware

**Via ESPConnect:**
1. Open `http://localhost:8080`
2. Connect ESP32
3. Flash Firmware tab
4. Choose `diagnostic-wifi-esp32.bin` (or your chip)
5. Offset: `0x10000`
6. Click Program

### 2. Connect to WiFi

1. ESP32 creates WiFi AP: `ESP32-Diagnostics`
2. Connect phone/laptop (password: `diagnostic123`)
3. Browser auto-opens diagnostic page
4. Or visit: `http://192.168.4.1`

### 3. View Dashboard!

Beautiful web interface with:
- 💻 Chip info
- 💾 Memory usage
- 📌 GPIO config
- 🔌 I2C device scan
- ⚙️ System info
- 📥 Export JSON button

**Total time: ~30 seconds!**

---

## 📊 Comparison

| Feature | Serial Version | WiFi Version ⭐ |
|---------|----------------|----------------|
| Output | Serial monitor | Web browser |
| Mobile | ❌ No | ✅ Yes |
| UI | JSON text | Beautiful dashboard |
| Export | Copy/paste | Download button |
| Ease | Medium | Easy |
| Size | ~270 KB | ~800 KB |

**Recommendation**: WiFi version for most users!

---

## 🔧 Flashing

**All firmware**: Flash at offset `0x10000`

**Baud rates:**
- 921600 (fast)
- 460800 (CH340 chips)
- 115200 (fallback)

---

## 🐛 Troubleshooting

### WiFi Version

**Can't see WiFi AP:**
- Wait 10 seconds after boot
- Restart ESP32
- Check WiFi enabled on device

**Page won't load:**
- Try `http://192.168.4.1`
- Disable mobile data
- Clear browser cache

### Serial Version

**No output:**
- Baud: 115200
- Press Reset button
- Wait 2-3 seconds

### Both

**No I2C devices:**
- Check wiring (SDA, SCL, GND, VCC)
- Add 4.7kΩ pull-ups
- Verify power

---

## 📝 Diagnostic Data

Both versions provide:

✅ Chip info (model, cores, frequency, MAC, features)  
✅ Memory (heap, PSRAM, flash)  
✅ GPIO (I2C pins)  
✅ I2C scan with device identification  
✅ System (SDK, uptime)  

---

## 🎯 Which Version?

**Use WiFi if:**
- You want easy interface
- You need mobile access
- You're showing to others
- You want beautiful UI

**Use Serial if:**
- You need automation
- You want smallest size
- You're scripting

---

## 📦 Source Code

**WiFi**: `/firmware/esp32-diagnostic-wifi/`  
**Serial**: `/firmware/esp32-diagnostic-tool/`

---

**Built with ❤️ by Processware**
