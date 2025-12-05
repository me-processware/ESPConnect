# Pre-compiled Diagnostic Firmware

**Author**: Processware  
**Version**: 1.0.0

Ready-to-flash diagnostic firmware for ESP32 boards. Flash via ESPConnect web interface!

---

## Available Firmware (1.1 MB total)

✅ **diagnostic-esp32.bin** - ESP32 (WROOM, DevKit, NodeMCU-32S) - 291 KB  
✅ **diagnostic-esp32-s2.bin** - ESP32-S2 (Saola, DevKit) - 250 KB  
✅ **diagnostic-esp32-s3.bin** - ESP32-S3 (DevKitC) - 278 KB  
✅ **diagnostic-esp32-c3.bin** - ESP32-C3 (DevKitM) - 271 KB  

⚠️ **ESP32-C6**: Not available yet (Arduino framework support pending)

---

## Quick Flash (3 Steps)

### 1. Choose Your Firmware

| Your Board | Flash This File |
|------------|-----------------|
| ESP32 DevKit, WROOM, NodeMCU-32S | `diagnostic-esp32.bin` |
| ESP32-S2 Saola, DevKit | `diagnostic-esp32-s2.bin` |
| ESP32-S3 DevKitC | `diagnostic-esp32-s3.bin` |
| ESP32-C3 DevKitM | `diagnostic-esp32-c3.bin` |

**Not sure?** Try `diagnostic-esp32.bin` - works with most boards.

### 2. Flash via ESPConnect

1. Open **http://localhost:8080**
2. **Connect** → Select ESP32
3. **Flash Firmware** tab
4. Choose `.bin` file
5. Offset: **0x10000**
6. Click **Program**

### 3. View Results

**Serial Monitor:**
- Baud: 115200
- Press Reset
- Copy JSON output

**Diagnostic Tab (Better!):**
- Paste JSON
- Click "Parse & Upload"
- View formatted results!

---

## What You Get

✅ Chip info (model, cores, frequency, MAC)  
✅ Memory analysis (Heap, PSRAM, Flash)  
✅ GPIO pinout with warnings  
✅ **I2C scanner** with device ID  
✅ System info (SDK, uptime)  
✅ JSON output for web integration  

---

## Troubleshooting

**Flash fails:** Hold BOOT button, check USB cable  
**No output:** Baud 115200, press Reset  
**I2C empty:** Check wiring, add 4.7kΩ pull-ups  

---

**Build your own:** See `firmware/esp32-diagnostic-tool/`
