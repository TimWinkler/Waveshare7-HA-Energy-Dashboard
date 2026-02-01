#!/bin/bash
# =============================================================================
# Waveshare Energy Dashboard - Build & Flash Script
# =============================================================================
# Usage: ./build.sh [command]
#
# Commands:
#   setup     - First-time setup with WiFi/MQTT configuration
#   build     - Build the project
#   flash     - Flash to device
#   monitor   - Open serial monitor
#   run       - Build, flash, and monitor
#   clean     - Clean build artifacts
#   menuconfig - Open ESP-IDF configuration menu
#   help      - Show this help message
# =============================================================================

set -e

# Configuration
SERIAL_PORT="${SERIAL_PORT:-/dev/ttyACM0}"
IDF_TARGET="esp32s3"

# Colors
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
CYAN='\033[0;36m'
NC='\033[0m'

# Get script directory
SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
cd "$SCRIPT_DIR"

# -----------------------------------------------------------------------------
# Helper Functions
# -----------------------------------------------------------------------------

print_header() {
    echo -e "\n${CYAN}=== $1 ===${NC}\n"
}

print_success() {
    echo -e "${GREEN}$1${NC}"
}

print_warning() {
    echo -e "${YELLOW}$1${NC}"
}

print_error() {
    echo -e "${RED}$1${NC}"
}

check_idf() {
    if ! command -v idf.py &> /dev/null; then
        print_error "ESP-IDF not found!"
        echo "Please run this script inside the DevContainer or source ESP-IDF:"
        echo "  . \$IDF_PATH/export.sh"
        exit 1
    fi
}

check_config() {
    if [ ! -f "main/config.h" ]; then
        print_error "main/config.h not found!"
        echo ""
        echo "Please create your configuration file:"
        echo "  cp main/config.h.example main/config.h"
        echo ""
        echo "Then edit main/config.h with your WiFi and MQTT credentials."
        exit 1
    fi
}

check_device() {
    if [ ! -e "$SERIAL_PORT" ]; then
        print_warning "Device not found at $SERIAL_PORT"
        echo "Available serial ports:"
        ls -la /dev/ttyACM* /dev/ttyUSB* 2>/dev/null || echo "  No serial devices found"
        echo ""
        echo "Set a different port with: SERIAL_PORT=/dev/ttyXXX ./build.sh $1"
        exit 1
    fi
}

# -----------------------------------------------------------------------------
# Commands
# -----------------------------------------------------------------------------

cmd_setup() {
    print_header "First-Time Setup"

    check_idf

    # Copy config if needed
    if [ ! -f "main/config.h" ]; then
        echo "Creating main/config.h from template..."
        cp main/config.h.example main/config.h
    else
        print_warning "main/config.h already exists"
        read -p "Overwrite with new configuration? [y/N] " -n 1 -r
        echo
        if [[ $REPLY =~ ^[Yy]$ ]]; then
            cp main/config.h.example main/config.h
        else
            echo "Keeping existing config.h"
            echo ""
            # Set target
            echo "Setting target to $IDF_TARGET..."
            idf.py set-target $IDF_TARGET
            print_success "Setup complete!"
            return
        fi
    fi

    # Interactive configuration
    print_header "WiFi Configuration"
    read -p "WiFi SSID: " wifi_ssid
    read -s -p "WiFi Password: " wifi_password
    echo ""

    print_header "MQTT Configuration"
    read -p "Home Assistant IP (e.g., 192.168.1.100): " ha_ip
    read -p "MQTT Username [mqtt]: " mqtt_user
    mqtt_user="${mqtt_user:-mqtt}"
    read -s -p "MQTT Password: " mqtt_password
    echo ""

    # Update config.h with provided values
    echo ""
    echo "Updating main/config.h..."

    # Escape special characters for sed
    wifi_ssid_escaped=$(printf '%s\n' "$wifi_ssid" | sed 's/[&/\]/\\&/g')
    wifi_password_escaped=$(printf '%s\n' "$wifi_password" | sed 's/[&/\]/\\&/g')
    ha_ip_escaped=$(printf '%s\n' "$ha_ip" | sed 's/[&/\]/\\&/g')
    mqtt_user_escaped=$(printf '%s\n' "$mqtt_user" | sed 's/[&/\]/\\&/g')
    mqtt_password_escaped=$(printf '%s\n' "$mqtt_password" | sed 's/[&/\]/\\&/g')

    sed -i "s/YOUR_WIFI_SSID/$wifi_ssid_escaped/g" main/config.h
    sed -i "s/YOUR_WIFI_PASSWORD/$wifi_password_escaped/g" main/config.h
    sed -i "s/YOUR_HA_IP/$ha_ip_escaped/g" main/config.h
    sed -i "s/YOUR_MQTT_USER/$mqtt_user_escaped/g" main/config.h
    sed -i "s/YOUR_MQTT_PASSWORD/$mqtt_password_escaped/g" main/config.h

    print_success "Configuration saved to main/config.h"

    # Set target
    echo ""
    echo "Setting target to $IDF_TARGET..."
    idf.py set-target $IDF_TARGET

    print_success "Setup complete!"
    echo ""
    echo "Next step: ./build.sh run"
}

cmd_build() {
    print_header "Building Project"

    check_idf
    check_config

    idf.py build

    print_success "Build complete!"
}

cmd_flash() {
    print_header "Flashing to Device"

    check_idf
    check_config
    check_device "flash"

    idf.py -p "$SERIAL_PORT" flash

    print_success "Flash complete!"
}

cmd_monitor() {
    print_header "Opening Serial Monitor"

    check_idf
    check_device "monitor"

    echo "Press Ctrl+] to exit monitor"
    echo ""

    idf.py -p "$SERIAL_PORT" monitor
}

cmd_run() {
    print_header "Build, Flash & Monitor"

    check_idf
    check_config
    check_device "run"

    echo "Press Ctrl+] to exit monitor"
    echo ""

    idf.py -p "$SERIAL_PORT" flash monitor
}

cmd_clean() {
    print_header "Cleaning Build Artifacts"

    check_idf

    idf.py fullclean

    print_success "Clean complete!"
}

cmd_menuconfig() {
    print_header "ESP-IDF Configuration"

    check_idf

    idf.py menuconfig
}

cmd_help() {
    echo "Waveshare Energy Dashboard - Build & Flash Script"
    echo ""
    echo "Usage: ./build.sh [command]"
    echo ""
    echo "Commands:"
    echo "  setup      - First-time setup with WiFi/MQTT configuration"
    echo "  build      - Build the project"
    echo "  flash      - Flash to device"
    echo "  monitor    - Open serial monitor (Ctrl+] to exit)"
    echo "  run        - Build, flash, and monitor (default)"
    echo "  clean      - Clean build artifacts"
    echo "  menuconfig - Open ESP-IDF configuration menu"
    echo "  help       - Show this help message"
    echo ""
    echo "Environment variables:"
    echo "  SERIAL_PORT - Serial port (default: /dev/ttyACM0)"
    echo ""
    echo "Examples:"
    echo "  ./build.sh setup                    # Interactive setup with credentials"
    echo "  ./build.sh run                      # Build, flash, monitor"
    echo "  SERIAL_PORT=/dev/ttyUSB0 ./build.sh flash"
}

# -----------------------------------------------------------------------------
# Main
# -----------------------------------------------------------------------------

case "${1:-run}" in
    setup)
        cmd_setup
        ;;
    build)
        cmd_build
        ;;
    flash)
        cmd_flash
        ;;
    monitor)
        cmd_monitor
        ;;
    run)
        cmd_run
        ;;
    clean)
        cmd_clean
        ;;
    menuconfig)
        cmd_menuconfig
        ;;
    help|--help|-h)
        cmd_help
        ;;
    *)
        print_error "Unknown command: $1"
        echo ""
        cmd_help
        exit 1
        ;;
esac
