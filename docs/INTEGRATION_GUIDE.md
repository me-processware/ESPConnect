# ESP32 Diagnostic Tool - Web Integration Guide

**Author**: Processware  
**Date**: December 2025

---

## Overview

This guide explains how to integrate the ESP32 Diagnostic Tool output directly into the ESPConnect Flask application for a clean web interface display.

## Architecture

```
┌─────────────────┐
│   ESP32 Device  │
│  (Diagnostic    │
│   Firmware)     │
└────────┬────────┘
         │ Serial Output (JSON)
         ↓
┌─────────────────┐
│  Serial Monitor │
│  (ESPConnect)   │
└────────┬────────┘
         │ Copy/Paste or API
         ↓
┌─────────────────┐
│  Flask Backend  │
│  (Parse & Store)│
└────────┬────────┘
         │ REST API
         ↓
┌─────────────────┐
│  Vue.js Frontend│
│  (Display)      │
└─────────────────┘
```

## Integration Methods

### Method 1: Manual Upload (Easiest)

**User Workflow:**
1. Flash diagnostic firmware via ESPConnect
2. Open Serial Monitor in ESPConnect
3. Copy entire serial output
4. Go to "Diagnostic" tab
5. Click "Upload Diagnostic Data"
6. Paste output and click "Parse & Upload"
7. View beautiful formatted results

**Pros:**
- No firmware modification needed
- Works with existing ESPConnect
- Simple implementation

**Cons:**
- Manual copy/paste step
- Not real-time

---

### Method 2: Automatic Capture (Advanced)

**Requires:**
- Modified Serial Monitor component
- Automatic JSON detection and parsing
- Background API upload

**Implementation:**
1. Modify `SerialMonitorTab.vue` to detect JSON markers
2. When `__DIAGNOSTIC_COMPLETE__` marker is found
3. Automatically extract JSON
4. POST to `/api/diagnostic/parse`
5. Show notification with link to Diagnostic tab

**Pros:**
- Seamless user experience
- Automatic processing
- No manual steps

**Cons:**
- Requires Vue.js modification
- More complex implementation

---

## Setup Instructions

### Step 1: Prepare Diagnostic Firmware

**Option A: Use JSON Output Version**

1. Copy `src/main_json.cpp` to `src/main.cpp`
2. Build in PlatformIO
3. Flash via ESPConnect

**Option B: Use Original Version**

- Original firmware works too
- Just need to manually parse output

### Step 2: Setup Flask Backend

1. **Copy enhanced Flask app:**
   ```bash
   cp espconnect-flask-enhanced/app.py espconnect-flask/app.py
   ```

2. **Install dependencies:**
   ```bash
   pip install flask-cors
   ```

3. **Restart Flask server:**
   ```bash
   python app.py
   ```

4. **Verify API endpoints:**
   ```bash
   curl http://localhost:8080/api/diagnostic/list
   ```

### Step 3: Add Vue.js Component

1. **Copy DiagnosticTab.vue to Vue project:**
   ```
   espconnect-flask/src/components/DiagnosticTab.vue
   ```

2. **Register component in App.vue:**
   ```vue
   <script>
   import DiagnosticTab from './components/DiagnosticTab.vue'
   
   export default {
     components: {
       DiagnosticTab
     }
   }
   </script>
   ```

3. **Add tab to navigation:**
   ```vue
   <v-tab value="diagnostic">
     <v-icon start>mdi-chip</v-icon>
     Diagnostic
   </v-tab>
   
   <v-window-item value="diagnostic">
     <DiagnosticTab />
   </v-window-item>
   ```

4. **Rebuild Vue.js frontend:**
   ```bash
   cd ESPConnect
   npm run build
   cp -r dist/* ../espconnect-flask/static/
   ```

### Step 4: Test Integration

1. **Flash diagnostic firmware**
2. **Open Serial Monitor**
3. **Copy output**
4. **Go to Diagnostic tab**
5. **Upload and view results**

---

## API Reference

### POST /api/diagnostic/upload

Upload diagnostic data directly (if you have JSON).

**Request:**
```json
{
  "device_id": "24:6F:28:XX:XX:XX",
  "diagnostic_tool": {
    "version": "1.0.0",
    "chip": { ... },
    "memory": { ... },
    "gpio": { ... },
    "i2c_scan": { ... },
    "system": { ... }
  }
}
```

**Response:**
```json
{
  "success": true,
  "device_id": "24:6F:28:XX:XX:XX",
  "message": "Diagnostic data uploaded successfully"
}
```

---

### POST /api/diagnostic/parse

Parse raw serial output and extract JSON.

**Request:**
```json
{
  "serial_output": "... raw serial output containing JSON ..."
}
```

**Response:**
```json
{
  "success": true,
  "diagnostic_data": {
    "diagnostic_tool": { ... }
  }
}
```

---

### GET /api/diagnostic/list

List all stored diagnostic data.

**Response:**
```json
{
  "devices": [
    {
      "device_id": "device_123",
      "chip_model": "ESP32",
      "mac_address": "24:6F:28:XX:XX:XX",
      "uploaded_at": "2025-12-04T10:30:00"
    }
  ]
}
```

---

### GET /api/diagnostic/<device_id>

Get diagnostic data for specific device.

**Response:**
```json
{
  "data": {
    "chip": { ... },
    "memory": { ... },
    "gpio": { ... },
    "i2c_scan": { ... },
    "system": { ... }
  },
  "uploaded_at": "2025-12-04T10:30:00",
  "timestamp": 1733308200
}
```

---

### DELETE /api/diagnostic/<device_id>

Delete diagnostic data.

**Response:**
```json
{
  "success": true,
  "message": "Diagnostic data deleted"
}
```

---

### GET /api/diagnostic/i2c-devices

Get I2C device identification database.

**Response:**
```json
{
  "i2c_devices": {
    "0x3C": {
      "name": "SSD1306",
      "type": "OLED Display",
      "description": "128x64 or 128x32 OLED"
    },
    ...
  }
}
```

---

## Diagnostic Tab Features

### Visual Components

**1. Chip Information Card**
- Model, revision, cores, frequency
- MAC address
- Flash size and type
- Feature chips (WiFi, Bluetooth, etc.)

**2. Memory Information Card**
- Heap usage with progress bar
- PSRAM status
- Flash memory details
- Color-coded warnings

**3. GPIO Configuration Card**
- Default I2C pins highlighted
- Pin count summary
- Restricted pins warning

**4. I2C Scan Results Table**
- Address (hex and decimal)
- Device identification
- Empty state with troubleshooting tips

**5. System Information Card**
- SDK version
- Arduino version
- Uptime (formatted)
- Reset reason

### Actions

- **Refresh** - Reload latest diagnostic data
- **Upload** - Parse and upload serial output
- **Export as JSON** - Download diagnostic data
- **Copy to Clipboard** - Copy JSON to clipboard

---

## Customization

### Change Color Scheme

Edit `DiagnosticTab.vue`:

```vue
<style scoped>
.diagnostic-header {
  background: linear-gradient(135deg, #your-color-1 0%, #your-color-2 100%);
}
</style>
```

### Add Custom Sections

Add new expansion panel:

```vue
<v-expansion-panel>
  <v-expansion-panel-title>
    <v-icon class="mr-2">mdi-your-icon</v-icon>
    <strong>Your Custom Section</strong>
  </v-expansion-panel-title>
  <v-expansion-panel-text>
    <!-- Your content here -->
  </v-expansion-panel-text>
</v-expansion-panel>
```

### Modify Firmware Output

Edit `src/main_json.cpp` to add custom fields:

```cpp
Serial.println("    \"custom_data\": {");
Serial.println("      \"your_field\": \"value\"");
Serial.println("    },");
```

---

## Troubleshooting

### JSON Parse Error

**Problem**: "No JSON data found in serial output"

**Solutions:**
1. Make sure you copied the ENTIRE output
2. Check that firmware is the JSON version
3. Look for `{` and `}` markers in output
4. Verify no corruption during copy/paste

---

### API Connection Failed

**Problem**: Cannot reach `/api/diagnostic/*` endpoints

**Solutions:**
1. Check Flask server is running
2. Verify port 8080 is not blocked
3. Check CORS is enabled
4. Test with `curl` first

---

### No Devices in List

**Problem**: `/api/diagnostic/list` returns empty

**Solutions:**
1. Upload diagnostic data first
2. Check Flask server logs
3. Verify data is being stored
4. Try manual upload via API

---

### Vue Component Not Showing

**Problem**: Diagnostic tab doesn't appear

**Solutions:**
1. Check component is registered in App.vue
2. Verify Vue.js build was successful
3. Check browser console for errors
4. Rebuild and copy static files

---

## Production Considerations

### Data Persistence

Current implementation uses **in-memory storage** (lost on restart).

**For production, use a database:**

```python
from flask_sqlalchemy import SQLAlchemy

app.config['SQLALCHEMY_DATABASE_URI'] = 'sqlite:///diagnostics.db'
db = SQLAlchemy(app)

class Diagnostic(db.Model):
    id = db.Column(db.Integer, primary_key=True)
    device_id = db.Column(db.String(100), unique=True)
    data = db.Column(db.JSON)
    uploaded_at = db.Column(db.DateTime)
```

### Authentication

Add authentication to protect diagnostic data:

```python
from flask_httpauth import HTTPBasicAuth

auth = HTTPBasicAuth()

@auth.verify_password
def verify_password(username, password):
    # Your auth logic
    return True

@app.route('/api/diagnostic/upload', methods=['POST'])
@auth.login_required
def upload_diagnostic():
    # Protected endpoint
    pass
```

### Rate Limiting

Prevent abuse with rate limiting:

```python
from flask_limiter import Limiter

limiter = Limiter(app, key_func=lambda: request.remote_addr)

@app.route('/api/diagnostic/upload', methods=['POST'])
@limiter.limit("10 per minute")
def upload_diagnostic():
    pass
```

---

## Future Enhancements

### Planned Features

- [ ] Real-time serial monitor integration
- [ ] Automatic JSON detection
- [ ] Device comparison tool
- [ ] Historical data tracking
- [ ] Export to PDF report
- [ ] Share diagnostic link
- [ ] Multi-device dashboard
- [ ] Firmware version tracking
- [ ] I2C device library expansion
- [ ] GPIO pin tester integration

---

## Support

**Author**: Processware

For issues or questions:
1. Check this guide thoroughly
2. Review API documentation
3. Check Flask server logs
4. Verify Vue.js console for errors

---

## Version History

### v2.0.0 (December 2025)
- Added JSON output firmware
- Created Flask API endpoints
- Built Vue.js Diagnostic component
- Integrated with ESPConnect
- Added I2C device database
- Implemented data parsing

---

**Happy Integrating! 🚀**
