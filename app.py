#!/usr/bin/env python3
"""
ESPConnect Flask Server
Serves the ESPConnect Vue.js application with Flask backend
"""

from flask import Flask, send_from_directory, send_file
import os

app = Flask(__name__, static_folder='static', static_url_path='')

# Configuration
PORT = 8080  # Using port 8080 instead of default 5000
HOST = '0.0.0.0'  # Listen on all interfaces

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
        # For SPA routing, return index.html for non-existent routes
        return send_file(os.path.join(app.static_folder, 'index.html'))

@app.route('/favicon.ico')
def favicon():
    """Serve favicon"""
    return send_from_directory(app.static_folder, 'favicon.ico')

# CORS headers for Web Serial API (required for browser security)
@app.after_request
def add_security_headers(response):
    """Add security headers required for Web Serial API"""
    # Allow cross-origin isolation (required for some Web APIs)
    response.headers['Cross-Origin-Opener-Policy'] = 'same-origin'
    response.headers['Cross-Origin-Embedder-Policy'] = 'require-corp'
    return response

# Health check endpoint
@app.route('/health')
def health():
    """Health check endpoint for monitoring"""
    return {'status': 'healthy', 'service': 'ESPConnect Flask Server'}, 200

# API endpoint example (for future extensions)
@app.route('/api/info')
def api_info():
    """API endpoint providing server information"""
    return {
        'name': 'ESPConnect Flask Server',
        'version': '1.0.0',
        'description': 'Flask backend serving ESPConnect Vue.js frontend'
    }, 200

if __name__ == '__main__':
    print(f"🚀 ESPConnect Flask Server starting on http://{HOST}:{PORT}")
    print(f"📂 Serving static files from: {app.static_folder}")
    print(f"🌐 Open http://localhost:{PORT} in your Chromium browser")
    print(f"⚠️  Make sure to use Chrome, Edge, or another Chromium-based browser for Web Serial API support")
    
    app.run(
        host=HOST,
        port=PORT,
        debug=True,
        threaded=True
    )
