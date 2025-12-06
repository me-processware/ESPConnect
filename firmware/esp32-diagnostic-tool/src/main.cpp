/**
 * ESP32 Diagnostic Tool - JSON Output Version
 * 
 * Outputs diagnostic data in JSON format for web interface integration
 * 
 * Author: Processware
 * Date: December 2025
 * 
 * Usage:
 * 1. Build and flash via ESPConnect
 * 2. Open Serial Monitor or use Flask API to capture output
 * 3. Parse JSON and display in web interface
 */

#include <Arduino.h>
#include <Wire.h>
#include <esp_system.h>
#include <esp_chip_info.h>
#include <esp_flash.h>
#include <esp_mac.h>

// I2C pins (same as main.cpp)
#ifndef I2C_SDA
  #if CONFIG_IDF_TARGET_ESP32
    #define I2C_SDA 21
    #define I2C_SCL 22
  #elif CONFIG_IDF_TARGET_ESP32S2 || CONFIG_IDF_TARGET_ESP32S3
    #define I2C_SDA 8
    #define I2C_SCL 9
  #elif CONFIG_IDF_TARGET_ESP32C3
    #define I2C_SDA 8
    #define I2C_SCL 9
  #elif CONFIG_IDF_TARGET_ESP32C6
    #define I2C_SDA 6
    #define I2C_SCL 7
  #else
    #define I2C_SDA 21
    #define I2C_SCL 22
  #endif
#endif

// Helper function to escape JSON strings
String escapeJSON(const String& str) {
  String escaped = str;
  escaped.replace("\\", "\\\\");
  escaped.replace("\"", "\\\"");
  escaped.replace("\n", "\\n");
  escaped.replace("\r", "\\r");
  escaped.replace("\t", "\\t");
  return escaped;
}

// Scan I2C and return JSON array
String scanI2C() {
  Wire.begin(I2C_SDA, I2C_SCL);
  Wire.setClock(100000);
  
  String devices = "[";
  bool first = true;
  
  for (uint8_t address = 1; address < 127; address++) {
    Wire.beginTransmission(address);
    uint8_t error = Wire.endTransmission();
    
    if (error == 0) {
      if (!first) devices += ",";
      devices += "{";
      devices += "\"address\":\"0x" + String(address, HEX) + "\",";
      devices += "\"decimal\":" + String(address) + ",";
      
      // Device identification
      String deviceType = "Unknown";
      if (address == 0x20 || address == 0x27) deviceType = "PCF8574 I/O Expander or LCD";
      else if (address == 0x3C || address == 0x3D) deviceType = "OLED Display (SSD1306)";
      else if (address == 0x48) deviceType = "ADS1115 ADC or TMP102 Temperature";
      else if (address == 0x50 || address == 0x57) deviceType = "EEPROM (AT24Cxx)";
      else if (address == 0x68) deviceType = "DS1307 RTC or MPU6050 IMU";
      else if (address == 0x76 || address == 0x77) deviceType = "BMP280/BME280 Sensor";
      
      devices += "\"likely\":\"" + deviceType + "\"";
      devices += "}";
      first = false;
    }
    delay(5);
  }
  
  devices += "]";
  Wire.end();
  return devices;
}

void setup() {
  Serial.begin(115200);
  
  // Wait for serial port to be ready (especially important for ESP32-C3/S3 USB-Serial)
  delay(2000);
  
  // Send ready signal
  Serial.println("\n\n=== ESP32 Diagnostic Tool Starting ===");
  Serial.println("Initializing...");
  delay(500);
  
  // Start JSON output
  Serial.println("{");
  Serial.println("  \"diagnostic_tool\": {");
  Serial.println("    \"version\": \"1.0.0\",");
  Serial.println("    \"author\": \"Processware\",");
  Serial.println("    \"timestamp\": " + String(millis()) + ",");
  
  // Chip Information
  esp_chip_info_t chip_info;
  esp_chip_info(&chip_info);
  
  Serial.println("    \"chip\": {");
  
  // Chip model
  Serial.print("      \"model\": \"");
  #if CONFIG_IDF_TARGET_ESP32
    Serial.print("ESP32");
  #elif CONFIG_IDF_TARGET_ESP32S2
    Serial.print("ESP32-S2");
  #elif CONFIG_IDF_TARGET_ESP32S3
    Serial.print("ESP32-S3");
  #elif CONFIG_IDF_TARGET_ESP32C3
    Serial.print("ESP32-C3");
  #elif CONFIG_IDF_TARGET_ESP32C6
    Serial.print("ESP32-C6");
  #else
    Serial.print("Unknown");
  #endif
  Serial.println("\",");
  
  Serial.println("      \"revision\": " + String(chip_info.revision) + ",");
  Serial.println("      \"cores\": " + String(chip_info.cores) + ",");
  Serial.println("      \"frequency_mhz\": " + String(getCpuFrequencyMhz()) + ",");
  
  // Features
  Serial.println("      \"features\": {");
  Serial.println("        \"wifi\": " + String((chip_info.features & CHIP_FEATURE_WIFI_BGN) ? "true" : "false") + ",");
  Serial.println("        \"bluetooth_classic\": " + String((chip_info.features & CHIP_FEATURE_BT) ? "true" : "false") + ",");
  Serial.println("        \"bluetooth_le\": " + String((chip_info.features & CHIP_FEATURE_BLE) ? "true" : "false") + ",");
  Serial.println("        \"ieee802154\": " + String((chip_info.features & CHIP_FEATURE_IEEE802154) ? "true" : "false"));
  Serial.println("      },");
  
  // Flash
  uint32_t flash_size;
  esp_flash_get_size(NULL, &flash_size);
  Serial.println("      \"flash_size_mb\": " + String(flash_size / (1024 * 1024)) + ",");
  Serial.println("      \"flash_type\": \"" + String((chip_info.features & CHIP_FEATURE_EMB_FLASH) ? "Embedded" : "External") + "\",");
  
  // MAC Address
  uint8_t mac[6];
  esp_read_mac(mac, ESP_MAC_WIFI_STA);
  char macStr[18];
  sprintf(macStr, "%02X:%02X:%02X:%02X:%02X:%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
  Serial.println("      \"mac_address\": \"" + String(macStr) + "\"");
  
  Serial.println("    },");
  
  // Memory Information
  Serial.println("    \"memory\": {");
  Serial.println("      \"heap\": {");
  Serial.println("        \"total_kb\": " + String(ESP.getHeapSize() / 1024) + ",");
  Serial.println("        \"free_kb\": " + String(ESP.getFreeHeap() / 1024) + ",");
  Serial.println("        \"used_kb\": " + String((ESP.getHeapSize() - ESP.getFreeHeap()) / 1024) + ",");
  Serial.println("        \"largest_free_block_kb\": " + String(ESP.getMaxAllocHeap() / 1024));
  Serial.println("      },");
  
  // PSRAM
  #ifdef BOARD_HAS_PSRAM
  if (psramFound()) {
    Serial.println("      \"psram\": {");
    Serial.println("        \"available\": true,");
    Serial.println("        \"total_kb\": " + String(ESP.getPsramSize() / 1024) + ",");
    Serial.println("        \"free_kb\": " + String(ESP.getFreePsram() / 1024));
    Serial.println("      },");
  } else {
    Serial.println("      \"psram\": {");
    Serial.println("        \"available\": false");
    Serial.println("      },");
  }
  #else
  Serial.println("      \"psram\": {");
  Serial.println("        \"available\": false,");
  Serial.println("        \"supported\": false");
  Serial.println("      },");
  #endif
  
  // Flash
  Serial.println("      \"flash\": {");
  Serial.println("        \"size_mb\": " + String(ESP.getFlashChipSize() / (1024 * 1024)) + ",");
  Serial.println("        \"speed_mhz\": " + String(ESP.getFlashChipSpeed() / 1000000) + ",");
  uint32_t mode = ESP.getFlashChipMode();
  String modeStr = "Unknown";
  if (mode == 0) modeStr = "QIO";
  else if (mode == 1) modeStr = "QOUT";
  else if (mode == 2) modeStr = "DIO";
  else if (mode == 3) modeStr = "DOUT";
  Serial.println("        \"mode\": \"" + modeStr + "\"");
  Serial.println("      }");
  Serial.println("    },");
  
  // GPIO Information
  Serial.println("    \"gpio\": {");
  Serial.println("      \"i2c_default\": {");
  Serial.println("        \"sda\": " + String(I2C_SDA) + ",");
  Serial.println("        \"scl\": " + String(I2C_SCL));
  Serial.println("      },");
  
  #if CONFIG_IDF_TARGET_ESP32
    Serial.println("      \"total_pins\": 34,");
    Serial.println("      \"usable_pins\": [0,1,2,3,4,5,12,13,14,15,16,17,18,19,21,22,23,25,26,27,32,33,34,35,36,39],");
    Serial.println("      \"input_only\": [34,35,36,39],");
    Serial.println("      \"restricted\": [6,7,8,9,10,11],");
    Serial.println("      \"strapping\": [0,2,5,12,15],");
    Serial.println("      \"adc1\": [32,33,34,35,36,39],");
    Serial.println("      \"adc2\": [0,2,4,12,13,14,15,25,26,27]");
  #elif CONFIG_IDF_TARGET_ESP32S2
    Serial.println("      \"total_pins\": 43,");
    Serial.println("      \"restricted\": [22,23,24,25,26,27,28,29,30,31,32,43,44]");
  #elif CONFIG_IDF_TARGET_ESP32S3
    Serial.println("      \"total_pins\": 45,");
    Serial.println("      \"restricted\": [22,23,24,25,26,27,28,29,30,31,32,43,44]");
  #elif CONFIG_IDF_TARGET_ESP32C3
    Serial.println("      \"total_pins\": 22,");
    Serial.println("      \"restricted\": [11,12,13,14,15,16,17,18,19]");
  #elif CONFIG_IDF_TARGET_ESP32C6
    Serial.println("      \"total_pins\": 31,");
    Serial.println("      \"restricted\": [8,9,10,11,12,13,14]");
  #endif
  
  Serial.println("    },");
  
  // I2C Scan
  Serial.println("    \"i2c_scan\": {");
  Serial.println("      \"sda_pin\": " + String(I2C_SDA) + ",");
  Serial.println("      \"scl_pin\": " + String(I2C_SCL) + ",");
  Serial.print("      \"devices\": ");
  Serial.println(scanI2C());
  Serial.println("    },");
  
  // System Info
  Serial.println("    \"system\": {");
  Serial.println("      \"sdk_version\": \"" + String(ESP.getSdkVersion()) + "\",");
  Serial.println("      \"arduino_version\": " + String(ARDUINO) + ",");
  Serial.println("      \"uptime_ms\": " + String(millis()) + ",");
  Serial.println("      \"reset_reason\": " + String(esp_reset_reason()));
  Serial.println("    }");
  
  // End JSON
  Serial.println("  }");
  Serial.println("}");
  Serial.println("__DIAGNOSTIC_COMPLETE__");
  
  // Send completion message
  Serial.println("\n=== Diagnostic Complete ===");
  Serial.println("You can now close the serial monitor or reset the device.");
}

void loop() {
  // Nothing to do
  delay(10000);
}
