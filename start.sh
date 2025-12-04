#!/bin/bash
# ESPConnect Flask Server Startup Script

echo "Starting ESPConnect Flask Server..."
cd "$(dirname "$0")"
python3 app.py
