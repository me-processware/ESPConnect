# ESPConnect Flask Server

A Flask-based web server that serves the ESPConnect Vue.js application for managing ESP32 and ESP8266 microcontrollers directly from your browser.

## Overview

This Flask application serves the built ESPConnect frontend, which allows you to:
- Flash firmware to ESP32/ESP8266 boards
- Manage SPIFFS/LittleFS file systems
- Monitor serial output
- View device information and partitions
- Backup and restore flash memory

The application uses the Web Serial API, which requires a Chromium-based browser (Chrome, Edge, Brave, etc.).

## Features

- ✅ Flask backend serving static Vue.js frontend
- ✅ Web Serial API support for direct USB communication
- ✅ Health check endpoint for monitoring
- ✅ Docker support for easy deployment
- ✅ CORS headers configured for Web Serial API
- ✅ Production-ready configuration
- ✅ Port 8080 (avoiding default Flask 5000)

## Requirements

- Python 3.11+
- Flask 3.0+
- Chromium-based browser (Chrome, Edge, Brave, Arc)
- ESP32/ESP8266 board with USB connection

## Installation

### Option 1: Direct Python Installation

```bash
# Install dependencies
pip install -r requirements.txt

# Run the server
python app.py
```

### Option 2: Docker

```bash
# Build and run with Docker
docker build -t espconnect-flask .
docker run -p 8080:8080 espconnect-flask
```

### Option 3: Docker Compose

```bash
# Run with docker-compose
docker-compose up -d
```

## Usage

1. Start the Flask server using one of the methods above
2. Open your Chromium browser and navigate to `http://localhost:8080`
3. Connect your ESP32/ESP8266 board via USB
4. Click "Connect" in the web interface
5. Select your device from the browser's serial port dialog
6. Start managing your ESP device!

## API Endpoints

| Endpoint | Method | Description |
|----------|--------|-------------|
| `/` | GET | Serves the main application |
| `/health` | GET | Health check endpoint |
| `/api/info` | GET | Server information |
| `/<path>` | GET | Serves static assets |

## Configuration

You can modify the following settings in `app.py`:

```python
PORT = 8080  # Change server port
HOST = '0.0.0.0'  # Change host (0.0.0.0 = all interfaces)
```

## Production Deployment

For production deployment, consider using:

1. **Gunicorn** (WSGI server):
```bash
pip install gunicorn
gunicorn -w 4 -b 0.0.0.0:8080 app:app
```

2. **Nginx** (reverse proxy):
```nginx
server {
    listen 80;
    server_name your-domain.com;
    
    location / {
        proxy_pass http://127.0.0.1:8080;
        proxy_set_header Host $host;
        proxy_set_header X-Real-IP $remote_addr;
    }
}
```

3. **systemd** (service management):
```ini
[Unit]
Description=ESPConnect Flask Server
After=network.target

[Service]
User=www-data
WorkingDirectory=/path/to/espconnect-flask
ExecStart=/usr/bin/python3 /path/to/espconnect-flask/app.py
Restart=always

[Install]
WantedBy=multi-user.target
```

## Troubleshooting

### Web Serial API not available
- Make sure you're using a Chromium-based browser (Chrome, Edge, Brave)
- Ensure you're accessing via `http://localhost` or `https://` (not `http://0.0.0.0`)
- Check that your browser supports Web Serial API

### Device not detected
- Verify the USB cable supports data transfer (not just charging)
- Check that no other application is using the serial port
- Try a different USB port
- Install appropriate USB drivers for your ESP board

### Connection fails
- Hold BOOT button, press RESET, then try connecting
- Lower the baud rate in the application settings
- Check USB cable quality

## Security Considerations

- The Web Serial API only works over HTTPS or localhost for security
- This server includes CORS headers required for Web Serial API
- For production, always use HTTPS with proper SSL certificates
- Consider adding authentication if exposing to the internet

## Extending the Application

The Flask backend can be extended with additional features:

1. **Database integration** - Store device configurations, logs
2. **User authentication** - Multi-user support with login
3. **REST API** - Additional endpoints for device management
4. **WebSocket** - Real-time updates and notifications
5. **File storage** - Cloud backup of firmware and configurations

Example API extension in `app.py`:
```python
@app.route('/api/devices', methods=['GET'])
def list_devices():
    # Your logic here
    return {'devices': []}, 200
```

## License

This Flask wrapper is provided as-is. The original ESPConnect project is licensed under MIT License.

## Credits

- Original ESPConnect: [thelastoutpostworkshop/ESPConnect](https://github.com/thelastoutpostworkshop/ESPConnect)
- Flask: [https://flask.palletsprojects.com/](https://flask.palletsprojects.com/)

## Support

For issues with:
- **Flask server**: Check this README and Flask documentation
- **ESPConnect functionality**: Visit the [original repository](https://github.com/thelastoutpostworkshop/ESPConnect)
- **ESP devices**: Consult ESP32/ESP8266 documentation
