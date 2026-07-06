# Raspberry Pi Setup — Wireless Sensor Monitoring System
# Group A, Team A6 — Advanced Embedded Systems, HSHL

## Hardware Required
- Raspberry Pi 3 Model B (or B+)
- MicroSD card (16 GB minimum, Class 10)
- Power supply (5V / 2.5A)
- Network connection (WiFi or Ethernet)

---

## Task 1 — Flash Raspberry Pi OS

1. Download **Raspberry Pi Imager** from https://www.raspberrypi.com/software/
2. Insert microSD card into your PC
3. In Imager: choose **Raspberry Pi OS Lite (64-bit)** (no desktop needed)
4. Click the gear icon ⚙ and configure:
   - Hostname: `raspberrypi`
   - Enable SSH → use password authentication
   - Set username: `pi` / password: `your_password`
   - Configure your WiFi SSID and password
   - Set locale / timezone
5. Write the image, insert card into Pi, power on
6. Wait ~60 seconds then SSH in:
   ```
   ssh pi@raspberrypi.local
   ```

---

## Task 2 — System Update

```bash
sudo apt update && sudo apt upgrade -y
sudo apt install -y curl git
```

---

## Task 3 — Install Mosquitto MQTT Broker

```bash
sudo apt install -y mosquitto mosquitto-clients

# Enable and start the service
sudo systemctl enable mosquitto
sudo systemctl start mosquitto

# Allow anonymous connections for lab testing
sudo nano /etc/mosquitto/mosquitto.conf
```

Add these lines to the config:
```
listener 1883
allow_anonymous true
```

Restart and test:
```bash
sudo systemctl restart mosquitto
mosquitto_sub -t "test" &
mosquitto_pub -t "test" -m "hello"
```

You should see `hello` printed. MQTT broker is working.

---

## Task 4 — Find Pi's IP Address

```bash
hostname -I
```

Note the IP (e.g., `192.168.1.50`). Update the ESP32 sketch's `mqttServer` with this IP.

---

## Task 5 — Install Node.js and Node-RED

```bash
# Install Node.js 18 LTS
curl -fsSL https://deb.nodesource.com/setup_18.x | sudo -E bash -
sudo apt install -y nodejs

# Install Node-RED globally
sudo npm install -g --unsafe-perm node-red

# Enable Node-RED as a service
sudo systemctl enable nodered.service
sudo systemctl start nodered.service
```

Verify: Open browser and navigate to `http://<Pi-IP>:1880`
You should see the Node-RED editor.

---

## Task 6 — Install Node-RED Dashboard Plugin

In the Node-RED editor:
1. Click the hamburger menu (top-right) → **Manage palette**
2. Go to the **Install** tab
3. Search for `node-red-dashboard`
4. Click **Install** → wait for completion
5. Restart Node-RED when prompted

Alternatively via SSH:
```bash
cd ~/.node-red
npm install node-red-dashboard
sudo systemctl restart nodered.service
```

Dashboard will be available at: `http://<Pi-IP>:1880/ui`

---

## Task 7 — Import Node-RED Flows

Four progressive flows are provided in this folder:

| File | Description |
|------|-------------|
| `flow_01_mqtt_debug.json` | MQTT receive + raw packet debug |
| `flow_02_basic_dashboard.json` | Gauge, status, knock count display |
| `flow_03_history_charts.json` | Temperature + knock history charts |
| `flow_04_remote_control.json` | Full dashboard with remote control |

**Import steps:**
1. Open Node-RED editor at `http://<Pi-IP>:1880`
2. Hamburger menu → **Import**
3. Click **select a file to import** and choose the JSON file
4. Click **Import** → then **Deploy** (top-right red button)

Start with `flow_01` and verify data arrives before moving to the next.

---

## Task 8 — Verify End-to-End

1. Power on ESP32 (ensure `mqttServer` IP matches Pi's IP)
2. Open Serial Monitor → confirm "MQTT connected"
3. On Pi terminal, run:
   ```bash
   mosquitto_sub -t "sensors/node01" -v
   ```
   You should see packets like:
   ```
   TEMP:23.4;KNOCK:0;STATUS:NORMAL;MUTE:0;WARN:25.0;CRIT:30.0
   ```
4. Open `http://<Pi-IP>:1880/ui` — dashboard shows live data

---

## Troubleshooting

| Problem | Fix |
|---------|-----|
| ESP32 won't connect to MQTT | Check IP in sketch, verify mosquitto is running |
| No data in Node-RED | Check MQTT broker node topic matches `sensors/node01` |
| Dashboard blank | Ensure node-red-dashboard is installed and deployed |
| Pi not reachable | Use `hostname -I` to confirm IP, ping from PC |
