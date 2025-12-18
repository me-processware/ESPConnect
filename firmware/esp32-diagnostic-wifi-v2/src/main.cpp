/**
 * ESP32 WiFi Diagnostic Tool - Enhanced v2.1
 * 
 * Comprehensive hardware diagnostics with complete GPIO mapping
 * Universal support for all ESP32 variants
 * 
 * Author: Processware
 * Version: 2.1.0
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
#include <driver/adc.h>

// WiFi AP Configuration
const char* AP_SSID = "ESP32-Diagnostics";
const char* AP_PASSWORD = "diagnostic123";
const char* MDNS_NAME = "esp32-diag";

// Web Server
AsyncWebServer server(80);

// I2C Configuration
#define I2C_SDA 21
#define I2C_SCL 22

// GPIO Pin Information Structure
struct GPIOInfo {
  int pin;
  const char* name;
  bool isUsable;
  bool isInputOnly;
  bool isStrapping;
  bool hasADC;
  bool hasPWM;
  bool hasTouch;
  const char* specialFunction;
  const char* warning;
};

// Function prototypes
String getChipModel();
String getChipFeatures();
void scanI2C(JsonArray& devices);
void getGPIOInfo(JsonArray& gpios);
String getHTMLPage();
String getDiagnosticJSON();
bool isPinUsable(int pin);
const char* getPinSpecialFunction(int pin);
const char* getPinWarning(int pin);

void setup() {
  Serial.begin(115200);
  delay(2000);
  
  Serial.println("\n\n=== ESP32 WiFi Diagnostic Tool v2.1 ===");
  Serial.println("Enhanced GPIO Detection");
  Serial.println("Author: Processware\n");
  
  // Start WiFi AP
  Serial.println("Starting WiFi Access Point...");
  
  // Disconnect any previous connections
  WiFi.disconnect(true);
  delay(100);
  
  // Set WiFi mode to AP
  if (!WiFi.mode(WIFI_AP)) {
    Serial.println("ERROR: Failed to set WiFi mode to AP!");
    Serial.println("Retrying...");
    delay(1000);
    WiFi.mode(WIFI_AP);
  }
  
  delay(100);
  
  // Start Access Point
  if (!WiFi.softAP(AP_SSID, AP_PASSWORD)) {
    Serial.println("ERROR: Failed to start Access Point!");
    Serial.println("Trying with default settings...");
    WiFi.softAP("ESP32-Diag", "12345678");
  }
  
  delay(500);
  
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
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(200, "text/html", getHTMLPage());
  });
  
  server.on("/api/diagnostics", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(200, "application/json", getDiagnosticJSON());
  });
  
  server.onNotFound([](AsyncWebServerRequest *request){
    request->redirect("/");
  });
  
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
  delay(1000);
}

String getChipModel() {
  #if CONFIG_IDF_TARGET_ESP32
    return "ESP32";
  #elif CONFIG_IDF_TARGET_ESP32S2
    return "ESP32-S2";
  #elif CONFIG_IDF_TARGET_ESP32S3
    return "ESP32-S3";
  #elif CONFIG_IDF_TARGET_ESP32C3
    return "ESP32-C3";
  #elif CONFIG_IDF_TARGET_ESP32C6
    return "ESP32-C6";
  #elif CONFIG_IDF_TARGET_ESP32H2
    return "ESP32-H2";
  #else
    return "Unknown";
  #endif
}

String getChipFeatures() {
  esp_chip_info_t chip_info;
  esp_chip_info(&chip_info);
  
  String features = "";
  if (chip_info.features & CHIP_FEATURE_WIFI_BGN) features += "WiFi ";
  if (chip_info.features & CHIP_FEATURE_BT) features += "Bluetooth ";
  if (chip_info.features & CHIP_FEATURE_BLE) features += "BLE ";
  
  return features.length() > 0 ? features : "None";
}

bool isPinUsable(int pin) {
  #if CONFIG_IDF_TARGET_ESP32
    // ESP32: GPIO 34-39 are input only, 6-11 are flash
    if (pin >= 6 && pin <= 11) return false; // Flash pins
    return (pin >= 0 && pin <= 39);
  #elif CONFIG_IDF_TARGET_ESP32S2
    // ESP32-S2: GPIO 26-32 are flash, 43-46 are special
    if (pin >= 26 && pin <= 32) return false; // Flash pins
    return (pin >= 0 && pin <= 46);
  #elif CONFIG_IDF_TARGET_ESP32S3
    // ESP32-S3: GPIO 26-32 are flash, 19-20 are USB
    if (pin >= 26 && pin <= 32) return false; // Flash pins
    return (pin >= 0 && pin <= 48);
  #elif CONFIG_IDF_TARGET_ESP32C3
    // ESP32-C3: GPIO 11-17 are flash, 18-19 are USB
    if (pin >= 11 && pin <= 17) return false; // Flash pins
    return (pin >= 0 && pin <= 21);
  #else
    return false;
  #endif
}

const char* getPinSpecialFunction(int pin) {
  #if CONFIG_IDF_TARGET_ESP32
    switch(pin) {
      case 1: return "UART0 TX";
      case 3: return "UART0 RX";
      case 21: return "I2C SDA";
      case 22: return "I2C SCL";
      case 18: return "SPI SCK";
      case 19: return "SPI MISO";
      case 23: return "SPI MOSI";
      case 5: return "SPI CS";
      default: return "";
    }
  #elif CONFIG_IDF_TARGET_ESP32S2
    switch(pin) {
      case 43: return "UART0 TX";
      case 44: return "UART0 RX";
      case 8: return "I2C SDA";
      case 9: return "I2C SCL";
      case 36: return "SPI SCK";
      case 37: return "SPI MISO";
      case 35: return "SPI MOSI";
      case 34: return "SPI CS";
      case 19: return "USB D-";
      case 20: return "USB D+";
      default: return "";
    }
  #elif CONFIG_IDF_TARGET_ESP32S3
    switch(pin) {
      case 43: return "UART0 TX";
      case 44: return "UART0 RX";
      case 8: return "I2C SDA";
      case 9: return "I2C SCL";
      case 12: return "SPI SCK";
      case 13: return "SPI MISO";
      case 11: return "SPI MOSI";
      case 10: return "SPI CS";
      case 19: return "USB D-";
      case 20: return "USB D+";
      case 48: return "RGB LED";
      default: return "";
    }
  #elif CONFIG_IDF_TARGET_ESP32C3
    switch(pin) {
      case 21: return "UART0 TX";
      case 20: return "UART0 RX";
      case 8: return "I2C SDA";
      case 9: return "I2C SCL";
      case 6: return "SPI SCK";
      case 5: return "SPI MISO";
      case 7: return "SPI MOSI / RGB LED";
      case 10: return "SPI CS";
      case 18: return "USB D-";
      case 19: return "USB D+";
      default: return "";
    }
  #endif
  return "";
}

const char* getPinWarning(int pin) {
  #if CONFIG_IDF_TARGET_ESP32
    if (pin == 0) return "Strapping pin - BOOT button";
    if (pin == 2) return "Strapping pin - affects boot mode";
    if (pin == 5) return "Strapping pin - affects boot mode";
    if (pin == 12) return "Strapping pin - affects flash voltage";
    if (pin == 15) return "Strapping pin - affects boot mode";
    if (pin >= 6 && pin <= 11) return "Connected to flash - DO NOT USE";
    if (pin >= 34 && pin <= 39) return "Input only - no output/pullup";
  #elif CONFIG_IDF_TARGET_ESP32S2
    if (pin == 0) return "Strapping pin - BOOT button";
    if (pin == 45) return "Strapping pin - affects boot mode";
    if (pin == 46) return "Strapping pin - affects boot mode";
    if (pin >= 26 && pin <= 32) return "Connected to flash - DO NOT USE";
    if (pin == 19 || pin == 20) return "USB pins - use with caution";
  #elif CONFIG_IDF_TARGET_ESP32S3
    if (pin == 0) return "Strapping pin - BOOT button";
    if (pin == 3) return "Strapping pin - affects boot mode";
    if (pin == 45) return "Strapping pin - affects boot mode";
    if (pin == 46) return "Strapping pin - affects boot mode";
    if (pin >= 26 && pin <= 32) return "Connected to flash - DO NOT USE";
    if (pin == 19 || pin == 20) return "USB pins - use with caution";
  #elif CONFIG_IDF_TARGET_ESP32C3
    if (pin == 2) return "Strapping pin - affects boot mode";
    if (pin == 8) return "Strapping pin - affects boot mode";
    if (pin == 9) return "Strapping pin - BOOT button";
    if (pin >= 11 && pin <= 17) return "Connected to flash - DO NOT USE";
    if (pin == 18 || pin == 19) return "USB pins - use with caution";
  #endif
  return "";
}

void getGPIOInfo(JsonArray& gpios) {
  int maxPin = 0;
  
  #if CONFIG_IDF_TARGET_ESP32
    maxPin = 39;
  #elif CONFIG_IDF_TARGET_ESP32S2
    maxPin = 46;
  #elif CONFIG_IDF_TARGET_ESP32S3
    maxPin = 48;
  #elif CONFIG_IDF_TARGET_ESP32C3
    maxPin = 21;
  #endif
  
  for (int pin = 0; pin <= maxPin; pin++) {
    if (!isPinUsable(pin)) continue;
    
    JsonObject gpio = gpios.createNestedObject();
    gpio["pin"] = pin;
    gpio["name"] = String("GPIO") + String(pin);
    
    // Check if input only
    bool inputOnly = false;
    #if CONFIG_IDF_TARGET_ESP32
      inputOnly = (pin >= 34 && pin <= 39);
    #endif
    gpio["input_only"] = inputOnly;
    
    // Check strapping pins
    bool isStrapping = false;
    #if CONFIG_IDF_TARGET_ESP32
      isStrapping = (pin == 0 || pin == 2 || pin == 5 || pin == 12 || pin == 15);
    #elif CONFIG_IDF_TARGET_ESP32S2
      isStrapping = (pin == 0 || pin == 45 || pin == 46);
    #elif CONFIG_IDF_TARGET_ESP32S3
      isStrapping = (pin == 0 || pin == 3 || pin == 45 || pin == 46);
    #elif CONFIG_IDF_TARGET_ESP32C3
      isStrapping = (pin == 2 || pin == 8 || pin == 9);
    #endif
    gpio["strapping"] = isStrapping;
    
    // ADC capability
    bool hasADC = false;
    #if CONFIG_IDF_TARGET_ESP32
      hasADC = ((pin >= 32 && pin <= 39) || (pin >= 0 && pin <= 19 && pin != 1 && pin != 3));
    #elif CONFIG_IDF_TARGET_ESP32S2
      hasADC = (pin >= 1 && pin <= 10);
    #elif CONFIG_IDF_TARGET_ESP32S3
      hasADC = (pin >= 1 && pin <= 10);
    #elif CONFIG_IDF_TARGET_ESP32C3
      hasADC = (pin >= 0 && pin <= 4);
    #endif
    gpio["adc"] = hasADC;
    
    // PWM capability (almost all pins support PWM)
    gpio["pwm"] = !inputOnly;
    
    // Touch capability
    bool hasTouch = false;
    #if CONFIG_IDF_TARGET_ESP32
      hasTouch = ((pin >= 0 && pin <= 3) || (pin >= 4 && pin <= 15) || pin == 27 || (pin >= 32 && pin <= 33));
    #elif CONFIG_IDF_TARGET_ESP32S2 || CONFIG_IDF_TARGET_ESP32S3
      hasTouch = (pin >= 1 && pin <= 14);
    #endif
    gpio["touch"] = hasTouch;
    
    // Special function
    const char* specialFunc = getPinSpecialFunction(pin);
    if (strlen(specialFunc) > 0) {
      gpio["special"] = specialFunc;
    }
    
    // Warning
    const char* warning = getPinWarning(pin);
    if (strlen(warning) > 0) {
      gpio["warning"] = warning;
    }
  }
}

void scanI2C(JsonArray& devices) {
  Serial.println("Scanning I2C bus...");
  
  for (byte address = 1; address < 127; address++) {
    Wire.beginTransmission(address);
    byte error = Wire.endTransmission();
    
    if (error == 0) {
      JsonObject device = devices.createNestedObject();
      device["address"] = String("0x") + String(address, HEX);
      
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
  JsonArray gpios = doc.createNestedArray("gpio_pins");
  getGPIOInfo(gpios);
  
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
    <title>ESP32 Diagnostics v2.1</title>
    <style>
        * { margin: 0; padding: 0; box-sizing: border-box; }
        
        body {
            font-family: -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, sans-serif;
            background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
            min-height: 100vh;
            padding: 20px;
        }
        
        .container { max-width: 1400px; margin: 0 auto; }
        
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
        
        .header .version {
            font-size: 0.9em;
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
            from { opacity: 0; transform: translateY(20px); }
            to { opacity: 1; transform: translateY(0); }
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
        }
        
        .info-item value {
            display: block;
            font-size: 1.3em;
            color: #333;
            font-weight: 500;
        }
        
        .gpio-table {
            width: 100%;
            border-collapse: collapse;
            margin-top: 15px;
            font-size: 0.9em;
        }
        
        .gpio-table th, .gpio-table td {
            padding: 10px;
            text-align: left;
            border-bottom: 1px solid #e0e0e0;
        }
        
        .gpio-table th {
            background: #667eea;
            color: white;
            font-weight: 600;
            position: sticky;
            top: 0;
        }
        
        .gpio-table tr:hover {
            background: #f7f9fc;
        }
        
        .badge {
            display: inline-block;
            padding: 3px 8px;
            border-radius: 12px;
            font-size: 0.75em;
            font-weight: 600;
            margin: 2px;
        }
        
        .badge-primary { background: #667eea; color: white; }
        .badge-success { background: #51cf66; color: white; }
        .badge-warning { background: #ffa94d; color: white; }
        .badge-danger { background: #ff6b6b; color: white; }
        .badge-info { background: #4dabf7; color: white; }
        
        .btn {
            background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
            color: white;
            border: none;
            padding: 12px 30px;
            border-radius: 25px;
            font-size: 1em;
            font-weight: 600;
            cursor: pointer;
            transition: transform 0.2s;
            margin: 5px;
        }
        
        .btn:hover {
            transform: translateY(-2px);
            box-shadow: 0 5px 15px rgba(102, 126, 234, 0.4);
        }
        
        .loading {
            text-align: center;
            padding: 40px;
            color: #667eea;
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
        
        .footer {
            text-align: center;
            color: white;
            margin-top: 30px;
            opacity: 0.8;
        }
        
        @media (max-width: 768px) {
            .header h1 { font-size: 1.8em; }
            .info-grid { grid-template-columns: 1fr; }
            .gpio-table { font-size: 0.8em; }
        }
    </style>
</head>
<body>
    <div class="container">
        <div class="header">
            <h1>🔧 ESP32 Diagnostics</h1>
            <p>Complete Hardware Analysis & GPIO Mapping</p>
            <p class="version">v2.1 Enhanced - Universal GPIO Detection</p>
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
            
            <!-- GPIO Pins -->
            <div class="card">
                <h2>📌 GPIO Pin Mapping (<span id="gpio-count">0</span> pins)</h2>
                <p style="margin-bottom: 15px; color: #666;">Complete pin capabilities for your <span id="gpio-chip">ESP32</span></p>
                <div style="overflow-x: auto;">
                    <table class="gpio-table">
                        <thead>
                            <tr>
                                <th>Pin</th>
                                <th>Capabilities</th>
                                <th>Special Function</th>
                                <th>Notes</th>
                            </tr>
                        </thead>
                        <tbody id="gpio-table-body">
                        </tbody>
                    </table>
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
            <p>Built with ❤️ by Processware | ESP32 WiFi Diagnostic Tool v2.1</p>
            <p style="font-size: 0.9em; margin-top: 5px;">Enhanced GPIO Detection - Universal Support</p>
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
            
            const heapUsage = data.memory.heap_usage_percent;
            const heapBar = document.getElementById('mem-heap-bar');
            heapBar.style.width = heapUsage + '%';
            heapBar.textContent = heapUsage + '%';
            if (heapUsage > 80) heapBar.classList.add('warning');
            
            if (data.memory.psram_size) {
                document.getElementById('psram-container').style.display = 'block';
                document.getElementById('psram-free-container').style.display = 'block';
                document.getElementById('mem-psram-size').textContent = formatBytes(data.memory.psram_size);
                document.getElementById('mem-psram-free').textContent = formatBytes(data.memory.psram_free);
            }
            
            // GPIO Pins
            document.getElementById('gpio-count').textContent = data.gpio_pins.length;
            document.getElementById('gpio-chip').textContent = data.chip.model;
            
            const gpioTableBody = document.getElementById('gpio-table-body');
            gpioTableBody.innerHTML = '';
            
            data.gpio_pins.forEach(gpio => {
                const row = document.createElement('tr');
                
                // Pin number
                const pinCell = document.createElement('td');
                pinCell.innerHTML = `<strong>GPIO ${gpio.pin}</strong>`;
                row.appendChild(pinCell);
                
                // Capabilities
                const capCell = document.createElement('td');
                let badges = '';
                if (gpio.adc) badges += '<span class="badge badge-success">ADC</span>';
                if (gpio.pwm) badges += '<span class="badge badge-info">PWM</span>';
                if (gpio.touch) badges += '<span class="badge badge-primary">Touch</span>';
                if (gpio.input_only) badges += '<span class="badge badge-warning">Input Only</span>';
                if (gpio.strapping) badges += '<span class="badge badge-danger">Strapping</span>';
                capCell.innerHTML = badges || '-';
                row.appendChild(capCell);
                
                // Special function
                const specialCell = document.createElement('td');
                specialCell.textContent = gpio.special || '-';
                row.appendChild(specialCell);
                
                // Warning/Notes
                const noteCell = document.createElement('td');
                if (gpio.warning) {
                    noteCell.innerHTML = `<span style="color: #ff6b6b;">⚠️ ${gpio.warning}</span>`;
                } else {
                    noteCell.textContent = 'OK';
                }
                row.appendChild(noteCell);
                
                gpioTableBody.appendChild(row);
            });
            
            // I2C Devices
            const i2cContainer = document.getElementById('i2c-devices');
            if (data.i2c_devices.length > 0) {
                let tableHTML = '<table class="gpio-table"><thead><tr><th>Address</th><th>Device</th></tr></thead><tbody>';
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
            
            // Show content
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
            link.download = 'esp32-diagnostics-v2.json';
            link.click();
            URL.revokeObjectURL(url);
        }
        
        function loadData() {
            fetch('/api/diagnostics')
                .then(response => response.json())
                .then(data => updateUI(data))
                .catch(error => {
                    console.error('Error:', error);
                    document.getElementById('loading').innerHTML = '<p style="color: red;">Error loading diagnostics</p>';
                });
        }
        
        window.addEventListener('load', loadData);
    </script>
</body>
</html>
)rawliteral";
}
