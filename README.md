# Waveshare 7" Home Assistant Energy Dashboard

An ESP-IDF powered energy monitoring dashboard for the **Waveshare ESP32-S3-Touch-LCD-7** display, integrating with Home Assistant via MQTT.

![ESP-IDF](https://img.shields.io/badge/ESP--IDF-v5.4-blue)
![License](https://img.shields.io/badge/License-MIT-green)
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

## Requirements

- **ESP-IDF v5.4+** (DevContainer provided)
- **Home Assistant** with:
  - MQTT broker (Mosquitto recommended)
  - [MQTT Statestream](https://www.home-assistant.io/integrations/mqtt_statestream/) integration
  - Utility meter sensors for YTD tracking

## Quick Start

### 1. Clone the Repository

```bash
git clone https://github.com/TimWinkler/Waveshare7-HA-Energy-Dashboard.git
cd Waveshare7-HA-Energy-Dashboard
```

### 2. Open in DevContainer (Recommended)

Open in VS Code and select **"Reopen in Container"** when prompted. This provides a complete ESP-IDF development environment.

### 3. Setup and Configure

Run the interactive setup:

```bash
./build.sh setup
```

You'll be prompted to enter:

```
=== WiFi Configuration ===
WiFi SSID: YourNetworkName
WiFi Password: ********

=== MQTT Configuration ===
Home Assistant IP (e.g., 192.168.1.100): 192.168.1.100
MQTT Username [mqtt]: mqtt
MQTT Password: ********
```

The setup will:
1. Create `main/config.h` from the template
2. Configure your WiFi and MQTT credentials
3. Set the ESP32-S3 as build target

> **Note:** To reconfigure later, run `./build.sh setup` again.

### 4. Build and Flash

```bash
./build.sh run
```

This builds, flashes, and opens the serial monitor. Press `Ctrl+]` to exit.

Or use individual commands:
```bash
./build.sh build    # Build only
./build.sh flash    # Flash only
./build.sh monitor  # Monitor only
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

The dashboard subscribes to these topics (configurable in `config.h`):

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
main/
├── main.c              # Entry point, WiFi, SNTP, task creation
├── config.h.example    # Configuration template (copy to config.h)
├── display_driver.c/h  # LCD initialization, LVGL setup
├── touch_driver.c/h    # GT911 touch controller
├── ui.c/h              # UI coordination, screen switching
├── ui_screens.c/h      # Screen layouts and widgets
├── ui_styles.c/h       # Visual styling
└── mqtt_handler.c/h    # MQTT client, sensor data parsing
```

## Development

### Build Script

The included `build.sh` script simplifies common operations:

```bash
./build.sh setup      # First-time setup
./build.sh build      # Build project
./build.sh flash      # Flash to device
./build.sh monitor    # Serial monitor (Ctrl+] to exit)
./build.sh run        # Build, flash, and monitor
./build.sh clean      # Clean build artifacts
./build.sh menuconfig # ESP-IDF configuration menu
./build.sh help       # Show all commands
```

Use a different serial port:
```bash
SERIAL_PORT=/dev/ttyUSB0 ./build.sh run
```

### Direct ESP-IDF Commands

```bash
idf.py build                          # Build project
idf.py -p /dev/ttyACM0 flash          # Flash to device
idf.py -p /dev/ttyACM0 monitor        # Serial monitor
idf.py -p /dev/ttyACM0 flash monitor  # Build, flash, and monitor
idf.py fullclean                      # Clean build
idf.py menuconfig                     # Configure project
```

### Security

- `main/config.h` is in `.gitignore` to prevent credential commits
- Pre-commit hook with [Gitleaks](https://github.com/gitleaks/gitleaks) for secret detection
- Always use `config.h.example` as template

## Cost Configuration

Default utility rates (configurable in `ui_screens.c`):

| Utility | Rate |
|---------|------|
| Grid Electricity | 0.30 EUR/kWh |
| Solar (savings) | 0.30 EUR/kWh |
| Gas | 2.20 EUR/m³ |
| Water | 5.00 EUR/m³ |

## License

MIT License - see [LICENSE](LICENSE) for details.

## Acknowledgments

- [LVGL](https://lvgl.io/) - Graphics library
- [ESP-IDF](https://github.com/espressif/esp-idf) - Development framework
- [Waveshare](https://www.waveshare.com/) - Hardware
