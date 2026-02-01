# Waveshare 7" Home Assistant Energy Dashboard

An ESP-IDF powered energy monitoring dashboard for the **Waveshare ESP32-S3-Touch-LCD-7** display, integrating with Home Assistant via MQTT.

[![Build](https://github.com/TimWinkler/Waveshare7-HA-Energy-Dashboard/actions/workflows/build.yml/badge.svg)](https://github.com/TimWinkler/Waveshare7-HA-Energy-Dashboard/actions/workflows/build.yml)
![ESP-IDF](https://img.shields.io/badge/ESP--IDF-v5.4-blue)
![License](https://img.shields.io/badge/License-GPL%20v3-blue)
![Hardware](https://img.shields.io/badge/Hardware-ESP32--S3-red)

## Overview

This project turns the Waveshare 7-inch touchscreen into a wall-mounted energy dashboard that displays real-time data from your Home Assistant instance. Perfect for monitoring your home's energy consumption, solar production, and utility costs at a glance.

### Features

- **3-Page Dashboard** with swipe navigation
  - **Today**: Real-time power, solar, gas, water usage, and temperatures
  - **Year-to-Date**: Annual consumption and costs for all utilities
  - **Forecast**: 7-day weather forecast with highs/lows and precipitation
- **Real-time MQTT Updates** from Home Assistant
- **Touch Navigation** with smooth screen transitions
- **Status Bar** showing time, date, and WiFi status
- **Cost Calculations** based on configurable utility rates

## Hardware

| Component | Specification |
|-----------|---------------|
| Board | [Waveshare ESP32-S3-Touch-LCD-7](https://www.waveshare.com/esp32-s3-touch-lcd-7.htm) rev 1.2 |
| Display | 7-inch 800x480 RGB LCD |
| Touch | GT911 capacitive controller |
| MCU | ESP32-S3 dual-core |
| Connectivity | WiFi 802.11 b/g/n |

## Development Setup

Development uses a **DevContainer** with ESP-IDF v5.4 pre-installed. This ensures a consistent build environment without manual toolchain setup.

### Prerequisites

- [Docker](https://www.docker.com/get-started) installed and running
- [VS Code](https://code.visualstudio.com/) with the [Dev Containers](https://marketplace.visualstudio.com/items?itemName=ms-vscode-remote.remote-containers) extension
- Waveshare ESP32-S3-Touch-LCD-7 connected via USB

### 1. Clone and Open in DevContainer

```bash
git clone https://github.com/TimWinkler/Waveshare7-HA-Energy-Dashboard.git
cd Waveshare7-HA-Energy-Dashboard
code .
```

When VS Code opens, click **"Reopen in Container"** in the notification (or use `Ctrl+Shift+P` → "Dev Containers: Reopen in Container").

The container will build with:
- ESP-IDF v5.4 toolchain
- All required build tools
- USB device passthrough (`/dev/ttyACM0`)

### 2. Run Interactive Setup

Once inside the DevContainer terminal:

```bash
./build.sh setup
```

The setup wizard prompts for your credentials:

```
=== WiFi Configuration ===
WiFi SSID: YourNetworkName
WiFi Password: ********

=== MQTT Configuration ===
Home Assistant IP (e.g., 192.168.1.100): 192.168.1.100
MQTT Username [mqtt]: mqtt
MQTT Password: ********
```

This creates `main/config.h` with your settings and configures the ESP32-S3 target.

### 3. Build and Flash

```bash
./build.sh run
```

This builds the firmware, flashes it to the device, and opens the serial monitor.

Press `Ctrl+]` to exit the monitor.

### Build Script Commands

| Command | Description |
|---------|-------------|
| `./build.sh setup` | Interactive setup wizard |
| `./build.sh build` | Build firmware |
| `./build.sh flash` | Flash to device |
| `./build.sh monitor` | Open serial monitor |
| `./build.sh run` | Build + flash + monitor |
| `./build.sh clean` | Clean build artifacts |
| `./build.sh menuconfig` | ESP-IDF configuration menu |

### USB Device Access

The DevContainer is configured to access `/dev/ttyACM0`. If your device appears on a different port:

```bash
SERIAL_PORT=/dev/ttyUSB0 ./build.sh run
```

To check available ports inside the container:
```bash
ls -la /dev/ttyACM* /dev/ttyUSB*
```

## Home Assistant Setup

### MQTT Statestream

Add to your `configuration.yaml`:

```yaml
mqtt_statestream:
  base_topic: ha
  include:
    entities:
      - sensor.daily_grid_energy
      - sensor.yearly_grid_energy
      - sensor.inverter_ac_power
      - sensor.yearly_solar_energy
      - sensor.daily_gas_consumption
      - sensor.yearly_gas
      - sensor.water_water_usage
      - sensor.yearly_water
      # Add your temperature and weather sensors
```

### Utility Meters (for YTD tracking)

```yaml
utility_meter:
  yearly_grid_energy:
    source: sensor.total_power_consumption
    cycle: yearly
  yearly_gas:
    source: sensor.gas_meter
    cycle: yearly
  yearly_solar_energy:
    source: sensor.solar_total_energy
    cycle: yearly
  yearly_water:
    source: sensor.water_meter
    cycle: yearly
```

## MQTT Topics

The dashboard subscribes to these topics (configurable in `main/config.h`):

| Data | Default Topic |
|------|---------------|
| Current Power | `vzlogger/data/chn0/raw` |
| Daily Grid | `ha/sensor/daily_grid_energy/state` |
| YTD Grid | `ha/sensor/yearly_grid_energy/state` |
| Solar Power | `ha/sensor/inverter_ac_power/state` |
| YTD Solar | `ha/sensor/yearly_solar_energy/state` |
| Daily Gas | `ha/sensor/daily_gas_consumption/state` |
| YTD Gas | `ha/sensor/yearly_gas/state` |
| Water Usage | `ha/sensor/water_water_usage/state` |
| YTD Water | `ha/sensor/yearly_water/state` |

## Project Structure

```
.
├── .devcontainer/          # DevContainer configuration
│   ├── Dockerfile          # ESP-IDF v5.4 base image
│   └── devcontainer.json   # VS Code container settings
├── main/
│   ├── main.c              # Entry point, WiFi, SNTP, tasks
│   ├── config.h.example    # Configuration template
│   ├── display_driver.c/h  # LCD initialization, LVGL
│   ├── touch_driver.c/h    # GT911 touch controller
│   ├── ui.c/h              # Screen coordination
│   ├── ui_screens.c/h      # Screen layouts and widgets
│   ├── ui_styles.c/h       # Visual styling
│   └── mqtt_handler.c/h    # MQTT client, data parsing
├── build.sh                # Build/flash helper script
├── CMakeLists.txt          # Project build config
├── partitions.csv          # Flash partition table
└── sdkconfig.defaults      # Default SDK configuration
```

## Configuration

### Credentials

Your `main/config.h` (created by setup) contains sensitive credentials and is excluded from git. To reconfigure:

```bash
./build.sh setup
```

### Utility Rates

Default rates in `main/ui_screens.c`:

| Utility | Rate |
|---------|------|
| Grid Electricity | 0.30 EUR/kWh |
| Solar (savings) | 0.30 EUR/kWh |
| Gas | 2.20 EUR/m³ |
| Water | 5.00 EUR/m³ |

## Security

- `main/config.h` is in `.gitignore` — credentials are never committed
- Pre-commit hook runs [Gitleaks](https://github.com/gitleaks/gitleaks) to detect accidental secret commits
- Only `config.h.example` (with placeholders) is tracked in git

## License

This project is licensed under the **GNU General Public License v3.0** - see [LICENSE](LICENSE) for details.

This means any derivative work must also be open source under GPL v3.

## Acknowledgments

- [LVGL](https://lvgl.io/) - Graphics library
- [ESP-IDF](https://github.com/espressif/esp-idf) - Development framework
- [Waveshare](https://www.waveshare.com/) - Hardware
