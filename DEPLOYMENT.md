# ESPConnect Flask - Deployment Guide

This guide covers various deployment options for the ESPConnect Flask server.

## Quick Start (Development)

```bash
# Start the development server
python3 app.py

# Or use the startup script
./start.sh
```

Access at: `http://localhost:8080`

## Production Deployment Options

### 1. Gunicorn (Recommended for Production)

Gunicorn is a production-grade WSGI server that's more robust than Flask's built-in server.

```bash
# Install Gunicorn
pip install gunicorn

# Run with 4 worker processes
gunicorn -w 4 -b 0.0.0.0:8080 wsgi:app

# With more options
gunicorn -w 4 -b 0.0.0.0:8080 --access-logfile - --error-logfile - wsgi:app
```

### 2. Docker Deployment

#### Build and Run

```bash
# Build the Docker image
docker build -t espconnect-flask .

# Run the container
docker run -d -p 8080:8080 --name espconnect espconnect-flask

# View logs
docker logs -f espconnect

# Stop the container
docker stop espconnect
```

#### Docker Compose

```bash
# Start services
docker-compose up -d

# View logs
docker-compose logs -f

# Stop services
docker-compose down
```

### 3. Systemd Service (Linux)

Create a systemd service for automatic startup and management.

**Step 1:** Create service file `/etc/systemd/system/espconnect.service`

```ini
[Unit]
Description=ESPConnect Flask Server
After=network.target

[Service]
Type=simple
User=ubuntu
WorkingDirectory=/home/ubuntu/espconnect-flask
ExecStart=/usr/bin/python3 /home/ubuntu/espconnect-flask/app.py
Restart=always
RestartSec=10

[Install]
WantedBy=multi-user.target
```

**Step 2:** Enable and start the service

```bash
# Reload systemd
sudo systemctl daemon-reload

# Enable service (start on boot)
sudo systemctl enable espconnect

# Start service
sudo systemctl start espconnect

# Check status
sudo systemctl status espconnect

# View logs
sudo journalctl -u espconnect -f
```

### 4. Nginx Reverse Proxy

Use Nginx as a reverse proxy for better performance and SSL termination.

**Step 1:** Install Nginx

```bash
sudo apt update
sudo apt install nginx
```

**Step 2:** Create Nginx configuration `/etc/nginx/sites-available/espconnect`

```nginx
server {
    listen 80;
    server_name your-domain.com;

    location / {
        proxy_pass http://127.0.0.1:8080;
        proxy_set_header Host $host;
        proxy_set_header X-Real-IP $remote_addr;
        proxy_set_header X-Forwarded-For $proxy_add_x_forwarded_for;
        proxy_set_header X-Forwarded-Proto $scheme;
        
        # Required for Web Serial API
        add_header Cross-Origin-Opener-Policy same-origin;
        add_header Cross-Origin-Embedder-Policy require-corp;
    }

    # Static files (optional optimization)
    location /assets/ {
        alias /home/ubuntu/espconnect-flask/static/assets/;
        expires 1y;
        add_header Cache-Control "public, immutable";
    }
}
```

**Step 3:** Enable the site

```bash
# Create symbolic link
sudo ln -s /etc/nginx/sites-available/espconnect /etc/nginx/sites-enabled/

# Test configuration
sudo nginx -t

# Reload Nginx
sudo systemctl reload nginx
```

### 5. SSL/HTTPS with Let's Encrypt

For production, you should use HTTPS. Here's how to set it up with Let's Encrypt.

```bash
# Install Certbot
sudo apt install certbot python3-certbot-nginx

# Obtain SSL certificate
sudo certbot --nginx -d your-domain.com

# Auto-renewal is configured automatically
# Test renewal
sudo certbot renew --dry-run
```

### 6. Cloud Deployment

#### AWS EC2

```bash
# 1. Launch EC2 instance (Ubuntu 22.04)
# 2. SSH into instance
# 3. Clone your repository
# 4. Install dependencies
sudo apt update
sudo apt install python3-pip
pip3 install -r requirements.txt

# 5. Run with Gunicorn
gunicorn -w 4 -b 0.0.0.0:8080 wsgi:app
```

#### DigitalOcean Droplet

Same as AWS EC2 - follow the steps above.

#### Heroku

Create `Procfile`:
```
web: gunicorn wsgi:app
```

Deploy:
```bash
heroku create your-app-name
git push heroku main
```

#### Railway.app

1. Connect your GitHub repository
2. Railway auto-detects Python and installs dependencies
3. Set start command: `python app.py`

## Environment Variables

For production, consider using environment variables:

```python
# In app.py
import os

PORT = int(os.environ.get('PORT', 8080))
HOST = os.environ.get('HOST', '0.0.0.0')
DEBUG = os.environ.get('DEBUG', 'False').lower() == 'true'
```

Set them:
```bash
export PORT=8080
export HOST=0.0.0.0
export DEBUG=False
```

## Performance Optimization

### 1. Enable Gzip Compression

In Nginx:
```nginx
gzip on;
gzip_types text/css application/javascript application/json;
gzip_min_length 1000;
```

### 2. Cache Static Assets

```nginx
location ~* \.(js|css|png|jpg|jpeg|gif|ico|svg|woff|woff2|ttf|eot)$ {
    expires 1y;
    add_header Cache-Control "public, immutable";
}
```

### 3. Use CDN

Consider using a CDN like CloudFlare for static assets.

## Monitoring

### Health Checks

The application includes a health check endpoint:

```bash
curl http://localhost:8080/health
```

### Uptime Monitoring

Use services like:
- UptimeRobot
- Pingdom
- StatusCake

Configure them to check `/health` endpoint every 5 minutes.

## Security Checklist

- [ ] Use HTTPS in production
- [ ] Set proper CORS headers
- [ ] Keep dependencies updated
- [ ] Use environment variables for sensitive data
- [ ] Implement rate limiting (if needed)
- [ ] Set up firewall rules
- [ ] Regular security updates
- [ ] Monitor logs for suspicious activity

## Troubleshooting

### Port Already in Use

```bash
# Find process using port 8080
sudo lsof -i :8080

# Kill the process
sudo kill -9 <PID>
```

### Permission Denied

```bash
# Give execute permission
chmod +x app.py start.sh

# Or run with sudo (not recommended)
sudo python3 app.py
```

### Static Files Not Loading

Check that the `static` directory exists and contains the built files:
```bash
ls -la static/
```

## Backup and Recovery

### Backup

```bash
# Backup the entire application
tar -czf espconnect-backup-$(date +%Y%m%d).tar.gz espconnect-flask/

# Backup to remote server
rsync -avz espconnect-flask/ user@remote:/backup/espconnect/
```

### Recovery

```bash
# Extract backup
tar -xzf espconnect-backup-20231204.tar.gz

# Restore from remote
rsync -avz user@remote:/backup/espconnect/ espconnect-flask/
```

## Scaling

For high traffic, consider:

1. **Horizontal Scaling**: Multiple instances behind a load balancer
2. **Vertical Scaling**: Increase server resources
3. **CDN**: Serve static assets from CDN
4. **Caching**: Implement Redis/Memcached if adding dynamic features

## Support

For deployment issues:
- Check application logs
- Verify firewall settings
- Ensure all dependencies are installed
- Test with `curl` or `wget` from command line
