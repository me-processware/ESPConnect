/**
 * ESP32 WiFi Diagnostic Tool
 * 
 * Creates WiFi AP and serves diagnostic data via web interface
 * No serial monitor needed - just connect to WiFi and browse!
 * 
 * Author: Processware
 * Version: 1.0.0
 * Date: December 2025
 */

#include <Arduino.h>
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <ESPmDNS.h>
#include <esp_chip_info.h>
#include <esp_system.h>
#include <Wire.h>
#include <ArduinoJson.h>

// WiFi AP Configuration
const char* AP_SSID = "ESP32-Diagnostics";
const char* AP_PASSWORD = "diagnostic123";  // Change if needed
const char* MDNS_NAME = "esp32-diag";

// Web Server
AsyncWebServer server(80);

// I2C Configuration
#define I2C_SDA 21
#define I2C_SCL 22

// Function prototypes
String getChipModel();
String getChipFeatures();
void scanI2C(JsonArray& devices);
String getHTMLPage();
String getDiagnosticJSON();

void setup() {
  Serial.begin(115200);
  delay(2000);
  
  Serial.println("\n\n=== ESP32 WiFi Diagnostic Tool ===");
  Serial.println("Author: Processware");
  Serial.println("Version: 1.0.0\n");
  
  // Start WiFi AP
  Serial.println("Starting WiFi Access Point...");
  WiFi.mode(WIFI_AP);
  WiFi.softAP(AP_SSID, AP_PASSWORD);
  
  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);
  Serial.printf("SSID: %s\n", AP_SSID);
  Serial.printf("Password: %s\n\n", AP_PASSWORD);
  
  // Start mDNS
  if (MDNS.begin(MDNS_NAME)) {
    Serial.printf("mDNS started: http://%s.local\n\n", MDNS_NAME);
    MDNS.addService("http", "tcp", 80);
  }
  
  // Initialize I2C
  Wire.begin(I2C_SDA, I2C_SCL);
  
  // Web Server Routes
  
  // Main page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(200, "text/html", getHTMLPage());
  });
  
  // JSON API endpoint
  server.on("/api/diagnostics", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(200, "application/json", getDiagnosticJSON());
  });
  
  // Captive portal redirect
  server.onNotFound([](AsyncWebServerRequest *request){
    request->redirect("/");
  });
  
  // Start server
  server.begin();
  Serial.println("Web server started!");
  Serial.println("========================================");
  Serial.println("Connect to WiFi and open browser:");
  Serial.printf("  1. Connect to WiFi: %s\n", AP_SSID);
  Serial.printf("  2. Password: %s\n", AP_PASSWORD);
  Serial.printf("  3. Open: http://%s\n", IP.toString().c_str());
  Serial.printf("  4. Or: http://%s.local\n", MDNS_NAME);
  Serial.println("========================================\n");
}

void loop() {
  // Nothing to do - web server handles everything
  delay(1000);
}

String getChipModel() {
  esp_chip_info_t chip_info;
  esp_chip_info(&chip_info);
  
  String model = "Unknown";
  
  #if CONFIG_IDF_TARGET_ESP32
    model = "ESP32";
  #elif CONFIG_IDF_TARGET_ESP32S2
    model = "ESP32-S2";
  #elif CONFIG_IDF_TARGET_ESP32S3
    model = "ESP32-S3";
  #elif CONFIG_IDF_TARGET_ESP32C3
    model = "ESP32-C3";
  #elif CONFIG_IDF_TARGET_ESP32C6
    model = "ESP32-C6";
  #elif CONFIG_IDF_TARGET_ESP32H2
    model = "ESP32-H2";
  #endif
  
  return model;
}

String getChipFeatures() {
  esp_chip_info_t chip_info;
  esp_chip_info(&chip_info);
  
  String features = "";
  if (chip_info.features & CHIP_FEATURE_WIFI_BGN) {
    features += "WiFi ";
  }
  if (chip_info.features & CHIP_FEATURE_BT) {
    features += "Bluetooth ";
  }
  if (chip_info.features & CHIP_FEATURE_BLE) {
    features += "BLE ";
  }
  
  return features.length() > 0 ? features : "None";
}

void scanI2C(JsonArray& devices) {
  Serial.println("Scanning I2C bus...");
  
  for (byte address = 1; address < 127; address++) {
    Wire.beginTransmission(address);
    byte error = Wire.endTransmission();
    
    if (error == 0) {
      JsonObject device = devices.createNestedObject();
      device["address"] = String("0x") + String(address, HEX);
      
      // Identify common devices
      String name = "Unknown Device";
      switch (address) {
        case 0x3C: case 0x3D: name = "OLED Display (SSD1306)"; break;
        case 0x68: name = "RTC/IMU (DS3231/MPU6050)"; break;
        case 0x76: case 0x77: name = "Pressure Sensor (BMP280/BME280)"; break;
        case 0x48: case 0x49: name = "ADC (ADS1115)"; break;
        case 0x50: case 0x51: name = "EEPROM (AT24C)"; break;
        case 0x20: case 0x21: name = "I/O Expander (PCF8574)"; break;
        case 0x40: name = "PWM Driver (PCA9685)"; break;
      }
      device["name"] = name;
      
      Serial.printf("  Found: 0x%02X - %s\n", address, name.c_str());
    }
  }
  
  Serial.printf("I2C scan complete. Found %d devices.\n\n", devices.size());
}

String getDiagnosticJSON() {
  JsonDocument doc;
  
  // Chip Info
  esp_chip_info_t chip_info;
  esp_chip_info(&chip_info);
  
  JsonObject chip = doc.createNestedObject("chip");
  chip["model"] = getChipModel();
  chip["revision"] = chip_info.revision;
  chip["cores"] = chip_info.cores;
  chip["frequency"] = ESP.getCpuFreqMHz();
  chip["features"] = getChipFeatures();
  
  uint64_t mac = ESP.getEfuseMac();
  char macStr[18];
  sprintf(macStr, "%02X:%02X:%02X:%02X:%02X:%02X",
          (uint8_t)(mac >> 40), (uint8_t)(mac >> 32), (uint8_t)(mac >> 24),
          (uint8_t)(mac >> 16), (uint8_t)(mac >> 8), (uint8_t)mac);
  chip["mac"] = String(macStr);
  
  // Memory Info
  JsonObject memory = doc.createNestedObject("memory");
  memory["heap_size"] = ESP.getHeapSize();
  memory["heap_free"] = ESP.getFreeHeap();
  memory["heap_used"] = ESP.getHeapSize() - ESP.getFreeHeap();
  memory["heap_usage_percent"] = (int)((float)(ESP.getHeapSize() - ESP.getFreeHeap()) / ESP.getHeapSize() * 100);
  
  #if CONFIG_IDF_TARGET_ESP32S3 || CONFIG_IDF_TARGET_ESP32S2
  if (ESP.getPsramSize() > 0) {
    memory["psram_size"] = ESP.getPsramSize();
    memory["psram_free"] = ESP.getFreePsram();
  }
  #endif
  
  memory["flash_size"] = ESP.getFlashChipSize();
  memory["flash_speed"] = ESP.getFlashChipSpeed() / 1000000;
  
  // GPIO Info
  JsonObject gpio = doc.createNestedObject("gpio");
  gpio["i2c_sda"] = I2C_SDA;
  gpio["i2c_scl"] = I2C_SCL;
  
  // I2C Scan
  JsonArray i2c_devices = doc.createNestedArray("i2c_devices");
  scanI2C(i2c_devices);
  
  // System Info
  JsonObject system = doc.createNestedObject("system");
  system["sdk_version"] = ESP.getSdkVersion();
  system["uptime_ms"] = millis();
  system["uptime_sec"] = millis() / 1000;
  
  String output;
  serializeJsonPretty(doc, output);
  return output;
}

String getHTMLPage() {
  return R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>ESP32 Diagnostics</title>
    <style>
        * {
            margin: 0;
            padding: 0;
            box-sizing: border-box;
        }
        
        body {
            font-family: -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, Oxygen, Ubuntu, Cantarell, sans-serif;
            background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
            min-height: 100vh;
            padding: 20px;
        }
        
        .container {
            max-width: 1200px;
            margin: 0 auto;
        }
        
        .header {
            text-align: center;
            color: white;
            margin-bottom: 30px;
        }
        
        .header h1 {
            font-size: 2.5em;
            margin-bottom: 10px;
            text-shadow: 2px 2px 4px rgba(0,0,0,0.3);
        }
        
        .header p {
            font-size: 1.2em;
            opacity: 0.9;
        }
        
        .card {
            background: white;
            border-radius: 15px;
            padding: 25px;
            margin-bottom: 20px;
            box-shadow: 0 10px 30px rgba(0,0,0,0.2);
            animation: slideUp 0.5s ease-out;
        }
        
        @keyframes slideUp {
            from {
                opacity: 0;
                transform: translateY(20px);
            }
            to {
                opacity: 1;
                transform: translateY(0);
            }
        }
        
        .card h2 {
            color: #667eea;
            margin-bottom: 20px;
            font-size: 1.8em;
            border-bottom: 3px solid #667eea;
            padding-bottom: 10px;
        }
        
        .info-grid {
            display: grid;
            grid-template-columns: repeat(auto-fit, minmax(250px, 1fr));
            gap: 15px;
        }
        
        .info-item {
            background: #f7f9fc;
            padding: 15px;
            border-radius: 10px;
            border-left: 4px solid #667eea;
        }
        
        .info-item label {
            display: block;
            font-weight: 600;
            color: #555;
            margin-bottom: 5px;
            font-size: 0.9em;
            text-transform: uppercase;
            letter-spacing: 0.5px;
        }
        
        .info-item value {
            display: block;
            font-size: 1.3em;
            color: #333;
            font-weight: 500;
        }
        
        .progress-bar {
            background: #e0e0e0;
            border-radius: 10px;
            height: 20px;
            overflow: hidden;
            margin-top: 10px;
        }
        
        .progress-fill {
            height: 100%;
            background: linear-gradient(90deg, #667eea 0%, #764ba2 100%);
            transition: width 0.5s ease;
            display: flex;
            align-items: center;
            justify-content: center;
            color: white;
            font-size: 0.8em;
            font-weight: bold;
        }
        
        .progress-fill.warning {
            background: linear-gradient(90deg, #f093fb 0%, #f5576c 100%);
        }
        
        table {
            width: 100%;
            border-collapse: collapse;
            margin-top: 15px;
        }
        
        th, td {
            padding: 12px;
            text-align: left;
            border-bottom: 1px solid #e0e0e0;
        }
        
        th {
            background: #667eea;
            color: white;
            font-weight: 600;
        }
        
        tr:hover {
            background: #f7f9fc;
        }
        
        .badge {
            display: inline-block;
            padding: 5px 12px;
            border-radius: 20px;
            font-size: 0.85em;
            font-weight: 600;
            margin-right: 5px;
        }
        
        .badge-primary {
            background: #667eea;
            color: white;
        }
        
        .badge-success {
            background: #51cf66;
            color: white;
        }
        
        .btn {
            background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
            color: white;
            border: none;
            padding: 12px 30px;
            border-radius: 25px;
            font-size: 1em;
            font-weight: 600;
            cursor: pointer;
            transition: transform 0.2s, box-shadow 0.2s;
            margin-top: 15px;
        }
        
        .btn:hover {
            transform: translateY(-2px);
            box-shadow: 0 5px 15px rgba(102, 126, 234, 0.4);
        }
        
        .btn:active {
            transform: translateY(0);
        }
        
        .loading {
            text-align: center;
            padding: 40px;
            color: #667eea;
            font-size: 1.2em;
        }
        
        .spinner {
            border: 4px solid #f3f3f3;
            border-top: 4px solid #667eea;
            border-radius: 50%;
            width: 40px;
            height: 40px;
            animation: spin 1s linear infinite;
            margin: 20px auto;
        }
        
        @keyframes spin {
            0% { transform: rotate(0deg); }
            100% { transform: rotate(360deg); }
        }
        
        .footer {
            text-align: center;
            color: white;
            margin-top: 30px;
            opacity: 0.8;
        }
        
        @media (max-width: 768px) {
            .header h1 {
                font-size: 1.8em;
            }
            
            .info-grid {
                grid-template-columns: 1fr;
            }
        }
    </style>
</head>
<body>
    <div class="container">
        <div class="header">
            <h1>🔧 ESP32 Diagnostics</h1>
            <p>Hardware Analysis & System Information</p>
        </div>
        
        <div id="loading" class="card loading">
            <div class="spinner"></div>
            <p>Loading diagnostic data...</p>
        </div>
        
        <div id="content" style="display: none;">
            <!-- Chip Information -->
            <div class="card">
                <h2>💻 Chip Information</h2>
                <div class="info-grid">
                    <div class="info-item">
                        <label>Model</label>
                        <value id="chip-model">-</value>
                    </div>
                    <div class="info-item">
                        <label>Revision</label>
                        <value id="chip-revision">-</value>
                    </div>
                    <div class="info-item">
                        <label>CPU Cores</label>
                        <value id="chip-cores">-</value>
                    </div>
                    <div class="info-item">
                        <label>Frequency</label>
                        <value id="chip-frequency">-</value>
                    </div>
                    <div class="info-item">
                        <label>MAC Address</label>
                        <value id="chip-mac">-</value>
                    </div>
                    <div class="info-item">
                        <label>Features</label>
                        <value id="chip-features">-</value>
                    </div>
                </div>
            </div>
            
            <!-- Memory Information -->
            <div class="card">
                <h2>💾 Memory Information</h2>
                <div class="info-grid">
                    <div class="info-item">
                        <label>Heap Size</label>
                        <value id="mem-heap-size">-</value>
                        <div class="progress-bar">
                            <div class="progress-fill" id="mem-heap-bar">0%</div>
                        </div>
                    </div>
                    <div class="info-item">
                        <label>Heap Free</label>
                        <value id="mem-heap-free">-</value>
                    </div>
                    <div class="info-item">
                        <label>Flash Size</label>
                        <value id="mem-flash-size">-</value>
                    </div>
                    <div class="info-item">
                        <label>Flash Speed</label>
                        <value id="mem-flash-speed">-</value>
                    </div>
                    <div class="info-item" id="psram-container" style="display: none;">
                        <label>PSRAM Size</label>
                        <value id="mem-psram-size">-</value>
                    </div>
                    <div class="info-item" id="psram-free-container" style="display: none;">
                        <label>PSRAM Free</label>
                        <value id="mem-psram-free">-</value>
                    </div>
                </div>
            </div>
            
            <!-- GPIO Configuration -->
            <div class="card">
                <h2>📌 GPIO Configuration</h2>
                <div class="info-grid">
                    <div class="info-item">
                        <label>I2C SDA Pin</label>
                        <value id="gpio-sda">-</value>
                    </div>
                    <div class="info-item">
                        <label>I2C SCL Pin</label>
                        <value id="gpio-scl">-</value>
                    </div>
                </div>
            </div>
            
            <!-- I2C Devices -->
            <div class="card">
                <h2>🔌 I2C Bus Scan</h2>
                <div id="i2c-devices">
                    <p>No I2C devices found</p>
                </div>
            </div>
            
            <!-- System Information -->
            <div class="card">
                <h2>⚙️ System Information</h2>
                <div class="info-grid">
                    <div class="info-item">
                        <label>SDK Version</label>
                        <value id="sys-sdk">-</value>
                    </div>
                    <div class="info-item">
                        <label>Uptime</label>
                        <value id="sys-uptime">-</value>
                    </div>
                </div>
                <button class="btn" onclick="exportJSON()">📥 Export JSON</button>
                <button class="btn" onclick="refreshData()">🔄 Refresh Data</button>
            </div>
        </div>
        
        <div class="footer">
            <p>Built with ❤️ by Processware | ESP32 WiFi Diagnostic Tool v1.0.0</p>
        </div>
    </div>
    
    <script>
        let diagnosticData = null;
        
        function formatBytes(bytes) {
            if (bytes < 1024) return bytes + ' B';
            else if (bytes < 1048576) return (bytes / 1024).toFixed(2) + ' KB';
            else return (bytes / 1048576).toFixed(2) + ' MB';
        }
        
        function formatUptime(ms) {
            const seconds = Math.floor(ms / 1000);
            const minutes = Math.floor(seconds / 60);
            const hours = Math.floor(minutes / 60);
            const days = Math.floor(hours / 24);
            
            if (days > 0) return days + 'd ' + (hours % 24) + 'h';
            if (hours > 0) return hours + 'h ' + (minutes % 60) + 'm';
            if (minutes > 0) return minutes + 'm ' + (seconds % 60) + 's';
            return seconds + 's';
        }
        
        function updateUI(data) {
            diagnosticData = data;
            
            // Chip Info
            document.getElementById('chip-model').textContent = data.chip.model;
            document.getElementById('chip-revision').textContent = 'v' + data.chip.revision;
            document.getElementById('chip-cores').textContent = data.chip.cores;
            document.getElementById('chip-frequency').textContent = data.chip.frequency + ' MHz';
            document.getElementById('chip-mac').textContent = data.chip.mac;
            
            // Features badges
            const features = data.chip.features.split(' ').filter(f => f.length > 0);
            let featuresHTML = '';
            features.forEach(feature => {
                featuresHTML += `<span class="badge badge-primary">${feature}</span>`;
            });
            document.getElementById('chip-features').innerHTML = featuresHTML;
            
            // Memory Info
            document.getElementById('mem-heap-size').textContent = formatBytes(data.memory.heap_size);
            document.getElementById('mem-heap-free').textContent = formatBytes(data.memory.heap_free);
            document.getElementById('mem-flash-size').textContent = formatBytes(data.memory.flash_size);
            document.getElementById('mem-flash-speed').textContent = data.memory.flash_speed + ' MHz';
            
            // Heap usage bar
            const heapUsage = data.memory.heap_usage_percent;
            const heapBar = document.getElementById('mem-heap-bar');
            heapBar.style.width = heapUsage + '%';
            heapBar.textContent = heapUsage + '%';
            if (heapUsage > 80) {
                heapBar.classList.add('warning');
            }
            
            // PSRAM (if available)
            if (data.memory.psram_size) {
                document.getElementById('psram-container').style.display = 'block';
                document.getElementById('psram-free-container').style.display = 'block';
                document.getElementById('mem-psram-size').textContent = formatBytes(data.memory.psram_size);
                document.getElementById('mem-psram-free').textContent = formatBytes(data.memory.psram_free);
            }
            
            // GPIO Info
            document.getElementById('gpio-sda').textContent = 'GPIO ' + data.gpio.i2c_sda;
            document.getElementById('gpio-scl').textContent = 'GPIO ' + data.gpio.i2c_scl;
            
            // I2C Devices
            const i2cContainer = document.getElementById('i2c-devices');
            if (data.i2c_devices.length > 0) {
                let tableHTML = '<table><thead><tr><th>Address</th><th>Device</th></tr></thead><tbody>';
                data.i2c_devices.forEach(device => {
                    tableHTML += `<tr><td><span class="badge badge-success">${device.address}</span></td><td>${device.name}</td></tr>`;
                });
                tableHTML += '</tbody></table>';
                i2cContainer.innerHTML = tableHTML;
            } else {
                i2cContainer.innerHTML = '<p>No I2C devices found. Check wiring and pull-up resistors.</p>';
            }
            
            // System Info
            document.getElementById('sys-sdk').textContent = data.system.sdk_version;
            document.getElementById('sys-uptime').textContent = formatUptime(data.system.uptime_ms);
            
            // Show content, hide loading
            document.getElementById('loading').style.display = 'none';
            document.getElementById('content').style.display = 'block';
        }
        
        function refreshData() {
            document.getElementById('loading').style.display = 'block';
            document.getElementById('content').style.display = 'none';
            loadData();
        }
        
        function exportJSON() {
            const dataStr = JSON.stringify(diagnosticData, null, 2);
            const dataBlob = new Blob([dataStr], {type: 'application/json'});
            const url = URL.createObjectURL(dataBlob);
            const link = document.createElement('a');
            link.href = url;
            link.download = 'esp32-diagnostics.json';
            link.click();
            URL.revokeObjectURL(url);
        }
        
        function loadData() {
            fetch('/api/diagnostics')
                .then(response => response.json())
                .then(data => {
                    updateUI(data);
                })
                .catch(error => {
                    console.error('Error loading diagnostics:', error);
                    document.getElementById('loading').innerHTML = '<p style="color: red;">Error loading diagnostics. Please refresh the page.</p>';
                });
        }
        
        // Load data on page load
        window.addEventListener('load', loadData);
    </script>
</body>
</html>
)rawliteral";
}
