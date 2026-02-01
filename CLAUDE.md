# CLAUDE.md

This file provides guidance to Claude Code when working with this repository.

## Project Overview

ESP-IDF project for the **Waveshare ESP32-S3-Touch-LCD-7** (rev 1.2) - a 7-inch capacitive touch display with ESP32-S3.

**Hardware:**
- Device: Waveshare ESP32-S3-Touch-LCD-7 rev 1.2
- USB Device ID: `303a:1001` (Espressif USB JTAG/serial debug unit)
- Serial Port: `/dev/ttyACM0`
- Display: 7-inch 800x480 RGB LCD
- Touch: GT911 capacitive controller

## Build Commands

```bash
idf.py build                          # Build project
idf.py -p /dev/ttyACM0 flash          # Flash to device
idf.py -p /dev/ttyACM0 monitor        # Serial monitor
idf.py -p /dev/ttyACM0 flash monitor  # Build, flash, and monitor
idf.py fullclean                      # Clean build
idf.py menuconfig                     # Configure project
```

## Architecture

```
main/
├── main.c           # Entry point, WiFi, SNTP, task creation
├── config.h         # WiFi, MQTT, pin configurations
├── display_driver.c/h  # LCD initialization, LVGL setup
├── touch_driver.c/h    # GT911 touch controller
├── ui.c/h           # UI coordination, screen switching
├── ui_screens.c/h   # Screen layouts and widgets
├── ui_styles.c/h    # Visual styling
└── mqtt_handler.c/h # MQTT client, sensor data parsing
```

## Key Configuration

- **WiFi**: `config.h` - WIFI_SSID, WIFI_PASSWORD
- **MQTT**: `config.h` - MQTT_BROKER_URI, MQTT_USERNAME, MQTT_PASSWORD
- **Topics**: `config.h` - TOPIC_* defines for Home Assistant sensors

## Home Assistant Integration

The dashboard connects to Home Assistant via MQTT Statestream. Required HA configuration:

1. MQTT Statestream for exposing sensor states
2. Utility meters for YTD tracking (yearly_grid_energy, yearly_gas, etc.)
3. Template sensors for weather data
