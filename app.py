#!/usr/bin/env python3
"""
ESPConnect Flask Server - Enhanced with Diagnostic Integration
Serves the ESPConnect Vue.js application with diagnostic data API

Author: Processware
Date: December 2025
"""

from flask import Flask, send_from_directory, send_file, request, jsonify
from flask_cors import CORS
import os
import json
import time
from datetime import datetime

app = Flask(__name__, static_folder='static', static_url_path='')
CORS(app)  # Enable CORS for API endpoints

# Configuration
PORT = 8080
HOST = '0.0.0.0'

# In-memory storage for diagnostic data (use database in production)
diagnostic_data_store = {}

@app.route('/')
def index():
    """Serve the main index.html file"""
    return send_file(os.path.join(app.static_folder, 'index.html'))

@app.route('/<path:path>')
def serve_static(path):
    """Serve static files (JS, CSS, fonts, etc.)"""
    file_path = os.path.join(app.static_folder, path)
    if os.path.exists(file_path):
        return send_from_directory(app.static_folder, path)
    else:
        return send_file(os.path.join(app.static_folder, 'index.html'))

@app.route('/favicon.ico')
def favicon():
    """Serve favicon"""
    return send_from_directory(app.static_folder, 'favicon.ico')

@app.route('/bin/<path:filename>')
def serve_bin(filename):
    """Serve diagnostic firmware files from bin folder"""
    bin_folder = os.path.join(os.path.dirname(__file__), 'bin')
    if os.path.exists(os.path.join(bin_folder, filename)):
        return send_from_directory(bin_folder, filename, mimetype='application/octet-stream')
    else:
        return jsonify({'error': 'Firmware file not found'}), 404

# CORS headers for Web Serial API
@app.after_request
def add_security_headers(response):
    """Add security headers required for Web Serial API"""
    response.headers['Cross-Origin-Opener-Policy'] = 'same-origin'
    response.headers['Cross-Origin-Embedder-Policy'] = 'require-corp'
    return response

# Health check endpoint
@app.route('/health')
def health():
    """Health check endpoint for monitoring"""
    return {'status': 'healthy', 'service': 'ESPConnect Flask Server'}, 200

# API endpoint example
@app.route('/api/info')
def api_info():
    """API endpoint providing server information"""
    return {
        'name': 'ESPConnect Flask Server Enhanced',
        'version': '2.0.0',
        'description': 'Flask backend with diagnostic integration',
        'author': 'Processware'
    }, 200

# ============================================
# Diagnostic API Endpoints
# ============================================

@app.route('/api/diagnostic/upload', methods=['POST'])
def upload_diagnostic():
    """
    Upload diagnostic data from ESP32
    
    Expected JSON format:
    {
        "device_id": "MAC address or unique ID",
        "diagnostic_tool": { ... diagnostic data ... }
    }
    """
    try:
        data = request.get_json()
        
        if not data or 'diagnostic_tool' not in data:
            return jsonify({'error': 'Invalid diagnostic data format'}), 400
        
        # Generate unique ID if not provided
        device_id = data.get('device_id', f'device_{int(time.time())}')
        
        # Store diagnostic data with timestamp
        diagnostic_data_store[device_id] = {
            'data': data['diagnostic_tool'],
            'uploaded_at': datetime.now().isoformat(),
            'timestamp': time.time()
        }
        
        return jsonify({
            'success': True,
            'device_id': device_id,
            'message': 'Diagnostic data uploaded successfully'
        }), 200
        
    except Exception as e:
        return jsonify({'error': str(e)}), 500

@app.route('/api/diagnostic/<device_id>', methods=['GET'])
def get_diagnostic(device_id):
    """
    Retrieve diagnostic data for a specific device
    """
    if device_id not in diagnostic_data_store:
        return jsonify({'error': 'Device not found'}), 404
    
    return jsonify(diagnostic_data_store[device_id]), 200

@app.route('/api/diagnostic/list', methods=['GET'])
def list_diagnostics():
    """
    List all stored diagnostic data
    """
    devices = []
    for device_id, data in diagnostic_data_store.items():
        devices.append({
            'device_id': device_id,
            'chip_model': data['data'].get('chip', {}).get('model', 'Unknown'),
            'mac_address': data['data'].get('chip', {}).get('mac_address', 'Unknown'),
            'uploaded_at': data['uploaded_at']
        })
    
    return jsonify({'devices': devices}), 200

@app.route('/api/diagnostic/<device_id>', methods=['DELETE'])
def delete_diagnostic(device_id):
    """
    Delete diagnostic data for a specific device
    """
    if device_id not in diagnostic_data_store:
        return jsonify({'error': 'Device not found'}), 404
    
    del diagnostic_data_store[device_id]
    return jsonify({'success': True, 'message': 'Diagnostic data deleted'}), 200

@app.route('/api/diagnostic/parse', methods=['POST'])
def parse_diagnostic():
    """
    Parse raw serial output and extract JSON diagnostic data
    
    Expected format:
    {
        "serial_output": "raw serial output containing JSON"
    }
    """
    try:
        data = request.get_json()
        serial_output = data.get('serial_output', '')
        
        # Find JSON in serial output
        start_idx = serial_output.find('{')
        end_idx = serial_output.rfind('}')
        
        if start_idx == -1 or end_idx == -1:
            return jsonify({'error': 'No JSON data found in serial output'}), 400
        
        json_str = serial_output[start_idx:end_idx+1]
        diagnostic_data = json.loads(json_str)
        
        return jsonify({
            'success': True,
            'diagnostic_data': diagnostic_data
        }), 200
        
    except json.JSONDecodeError as e:
        return jsonify({'error': f'Invalid JSON: {str(e)}'}), 400
    except Exception as e:
        return jsonify({'error': str(e)}), 500

@app.route('/api/diagnostic/i2c-devices', methods=['GET'])
def get_i2c_device_database():
    """
    Return database of known I2C devices for identification
    """
    i2c_devices = {
        '0x20': {'name': 'PCF8574', 'type': 'I/O Expander', 'description': '8-bit I/O expander'},
        '0x27': {'name': 'PCF8574A or LCD', 'type': 'I/O Expander / Display', 'description': 'I2C LCD adapter or I/O expander'},
        '0x3C': {'name': 'SSD1306', 'type': 'OLED Display', 'description': '128x64 or 128x32 OLED'},
        '0x3D': {'name': 'SSD1306', 'type': 'OLED Display', 'description': '128x64 or 128x32 OLED (alt address)'},
        '0x48': {'name': 'ADS1115 or TMP102', 'type': 'ADC / Temperature', 'description': '16-bit ADC or temperature sensor'},
        '0x50': {'name': 'AT24C32/64', 'type': 'EEPROM', 'description': 'I2C EEPROM memory'},
        '0x57': {'name': 'AT24C32/64', 'type': 'EEPROM', 'description': 'I2C EEPROM memory (alt address)'},
        '0x68': {'name': 'DS1307 or MPU6050', 'type': 'RTC / IMU', 'description': 'Real-time clock or 6-axis IMU'},
        '0x69': {'name': 'MPU6050', 'type': 'IMU', 'description': '6-axis IMU (alt address)'},
        '0x76': {'name': 'BMP280 or BME280', 'type': 'Environmental Sensor', 'description': 'Pressure/temperature/humidity sensor'},
        '0x77': {'name': 'BMP280 or BME280', 'type': 'Environmental Sensor', 'description': 'Pressure/temperature/humidity sensor (alt address)'}
    }
    
    return jsonify({'i2c_devices': i2c_devices}), 200

if __name__ == '__main__':
    print(f"🚀 ESPConnect Flask Server Enhanced starting on http://{HOST}:{PORT}")
    print(f"📂 Serving static files from: {app.static_folder}")
    print(f"🌐 Open http://localhost:{PORT} in your Chromium browser")
    print(f"⚠️  Make sure to use Chrome, Edge, or another Chromium-based browser for Web Serial API support")
    print(f"\n📊 Diagnostic API Endpoints:")
    print(f"   POST   /api/diagnostic/upload - Upload diagnostic data")
    print(f"   GET    /api/diagnostic/<id> - Get diagnostic data")
    print(f"   GET    /api/diagnostic/list - List all diagnostics")
    print(f"   DELETE /api/diagnostic/<id> - Delete diagnostic data")
    print(f"   POST   /api/diagnostic/parse - Parse serial output")
    print(f"   GET    /api/diagnostic/i2c-devices - Get I2C device database")
    
    app.run(
        host=HOST,
        port=PORT,
        debug=True,
        threaded=True
    )
