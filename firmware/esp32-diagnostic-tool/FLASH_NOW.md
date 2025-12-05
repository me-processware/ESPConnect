# ESP32 Diagnostic Firmware - JSON Version
## Flash This Immediately! ⚡

**Author**: Processware  
**Output Format**: JSON (for web integration)

---

## Quick Flash (3 Steps)

### Step 1: Copy Code

The complete firmware code is in `main.cpp` (this directory).

### Step 2: Setup PlatformIO Project

**Option A: Use Existing Project**
```bash
# Copy main.cpp to your esp32-diagnostic-tool project
cp main.cpp /path/to/esp32-diagnostic-tool/src/main.cpp
```

**Option B: Create New Project**
1. Create folder: `esp32-diagnostic-json`
2. Copy `platformio.ini` and `main.cpp` to it
3. Create `src/` folder
4. Move `main.cpp` to `src/main.cpp`

### Step 3: Build & Flash

**In PlatformIO:**
1. Open project folder
2. Edit `platformio.ini` - set your board:
   ```ini
   default_envs = esp32dev  ; or esp32-s2, esp32-s3, esp32-c3, esp32-c6
   ```
3. Press `Ctrl+Alt+B` to build
4. Find `.bin` file in `.pio/build/[board]/firmware.bin`
5. Flash via ESPConnect:
   - Connect to ESP32
   - Flash Firmware tab
   - Choose `.bin` file
   - Offset: `0x10000`
   - Click Program

---

## What This Firmware Does

✅ Outputs **structured JSON** instead of formatted text  
✅ Easy to parse by web applications  
✅ Includes marker `__DIAGNOSTIC_COMPLETE__` for auto-detection  
✅ Same diagnostic features as original:
- Chip information
- Memory analysis
- GPIO configuration
- **I2C bus scanner**
- System information

---

## Output Example

```json
{
  "diagnostic_tool": {
    "version": "1.0.0",
    "author": "Processware",
    "timestamp": 12345,
    "chip": {
      "model": "ESP32",
      "revision": 3,
      "cores": 2,
      "frequency_mhz": 240,
      "features": {
        "wifi": true,
        "bluetooth_classic": true,
        "bluetooth_le": true,
        "ieee802154": false
      },
      "flash_size_mb": 4,
      "flash_type": "External",
      "mac_address": "24:6F:28:XX:XX:XX"
    },
    "memory": {
      "heap": {
        "total_kb": 327,
        "free_kb": 298,
        "used_kb": 29,
        "largest_free_block_kb": 110
      },
      "psram": {
        "available": false
      },
      "flash": {
        "size_mb": 4,
        "speed_mhz": 40,
        "mode": "DIO"
      }
    },
    "gpio": {
      "i2c_default": {
        "sda": 21,
        "scl": 22
      },
      "total_pins": 34,
      "usable_pins": [0,1,2,3,4,5,12,13,14,15,16,17,18,19,21,22,23,25,26,27,32,33,34,35,36,39],
      "input_only": [34,35,36,39],
      "restricted": [6,7,8,9,10,11],
      "strapping": [0,2,5,12,15],
      "adc1": [32,33,34,35,36,39],
      "adc2": [0,2,4,12,13,14,15,25,26,27]
    },
    "i2c_scan": {
      "sda_pin": 21,
      "scl_pin": 22,
      "devices": [
        {
          "address": "0x3c",
          "decimal": 60,
          "likely": "OLED Display (SSD1306)"
        },
        {
          "address": "0x68",
          "decimal": 104,
          "likely": "DS1307 RTC or MPU6050 IMU"
        }
      ]
    },
    "system": {
      "sdk_version": "v4.4.6",
      "arduino_version": 20300,
      "uptime_ms": 2150,
      "reset_reason": 1
    }
  }
}
__DIAGNOSTIC_COMPLETE__
```

---

## After Flashing

### View Output

**Method 1: Serial Monitor in ESPConnect**
1. Go to Serial Monitor tab
2. Click Start
3. Baud: 115200
4. Press Reset on ESP32
5. Copy entire JSON output

**Method 2: PlatformIO Serial Monitor**
```bash
pio device monitor -b 115200
```

### Use with Web Interface

1. Copy the JSON output
2. Go to Flask app: `http://localhost:8080`
3. Navigate to Diagnostic tab
4. Click "Upload Diagnostic Data"
5. Paste JSON and click "Parse & Upload"
6. View beautiful formatted results!

### Use with API

```bash
# Parse and store
curl -X POST http://localhost:8080/api/diagnostic/parse \
  -H "Content-Type: application/json" \
  -d '{"serial_output": "... paste JSON output ..."}'

# View stored data
curl http://localhost:8080/api/diagnostic/list
```

---

## Supported Boards

- ✅ ESP32 (WROOM, DevKit)
- ✅ ESP32-S2
- ✅ ESP32-S3
- ✅ ESP32-C3
- ✅ ESP32-C6

Firmware automatically detects your chip and adjusts output accordingly.

---

## Customization

### Change I2C Pins

Edit `main.cpp` around line 35:

```cpp
#define I2C_SDA 21  // Your SDA pin
#define I2C_SCL 22  // Your SCL pin
```

### Add Custom Data

Add your own fields to JSON output:

```cpp
// In setup() function, before closing JSON
Serial.println("    \"custom\": {");
Serial.println("      \"my_field\": \"my_value\"");
Serial.println("    },");
```

---

## Troubleshooting

**No output in Serial Monitor:**
- Check baud rate is 115200
- Press Reset button on ESP32
- Try different USB cable

**JSON is incomplete:**
- Wait for `__DIAGNOSTIC_COMPLETE__` marker
- Don't interrupt during output
- Increase Serial buffer if needed

**I2C shows no devices:**
- Check wiring (SDA, SCL, GND, VCC)
- Add 4.7kΩ pull-up resistors
- Verify device power supply
- Try different I2C pins

---

## File Structure

```
esp32-diagnostic-json/
├── platformio.ini    # PlatformIO configuration
├── main.cpp          # Complete firmware code (JSON output)
└── FLASH_NOW.md      # This file
```

---

## Next Steps

1. ✅ Flash this firmware
2. ✅ Open Serial Monitor (115200 baud)
3. ✅ Copy JSON output
4. ✅ Use with Flask API or web interface
5. ✅ Enjoy structured diagnostic data!

---

**Ready to flash? The complete code is in `main.cpp`! 🚀**

**Author**: Processware  
**Version**: 1.0.0 (JSON Output)
