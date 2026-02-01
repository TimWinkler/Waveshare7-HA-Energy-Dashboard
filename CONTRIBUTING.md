# Contributing to Waveshare Energy Dashboard

Thank you for your interest in contributing! This document provides guidelines and instructions for contributing to the project.

## Table of Contents

- [Code of Conduct](#code-of-conduct)
- [Getting Started](#getting-started)
- [Git Flow Branching Model](#git-flow-branching-model)
- [Development Workflow](#development-workflow)
- [Coding Standards](#coding-standards)
- [Pull Request Process](#pull-request-process)
- [Issue Reporting](#issue-reporting)

## Code of Conduct

Be respectful, inclusive, and professional in all interactions.

## Getting Started

### Prerequisites

- [Docker](https://www.docker.com/get-started) installed and running
- [VS Code](https://code.visualstudio.com/) with the [Dev Containers](https://marketplace.visualstudio.com/items?itemName=ms-vscode-remote.remote-containers) extension
- Waveshare ESP32-S3-Touch-LCD-7 connected via USB

### Setting Up Development Environment

1. **Fork and Clone**:
   ```bash
   git clone https://github.com/yourusername/Waveshare7-HA-Energy-Dashboard.git
   cd Waveshare7-HA-Energy-Dashboard
   ```

2. **Open in DevContainer**:
   - Open in VS Code
   - Click "Reopen in Container" when prompted
   - Wait for the container to build (ESP-IDF v5.4 toolchain)

3. **Run Setup**:
   ```bash
   ./build.sh setup
   ```
   This will prompt for WiFi and MQTT credentials.

4. **Verify Setup**:
   ```bash
   ./build.sh build
   ```

## Git Flow Branching Model

This project follows the Git Flow branching model.

### Main Branches

- `main` - Production-ready code (protected)
- `develop` - Integration branch for development

### Supporting Branches

| Branch Type | Purpose | Branch From | Merge To |
|-------------|---------|-------------|----------|
| `feature/*` | New features | `develop` | `develop` |
| `release/*` | Release preparation | `develop` | `main` and `develop` |
| `hotfix/*` | Production bug fixes | `main` | `main` and `develop` |

### Quick Start

```bash
# Start a new feature
git checkout develop
git pull origin develop
git checkout -b feature/your-feature-name

# Start a hotfix
git checkout main
git pull origin main
git checkout -b hotfix/v1.0.1-bug-description
```

## Development Workflow

### 1. Create a Feature Branch

```bash
# Ensure develop is up to date
git checkout develop
git pull origin develop

# Create your feature branch
git checkout -b feature/your-feature-name
# Examples:
# feature/add-weather-icons
# feature/issue-123-fix-mqtt-reconnect
```

### 2. Make Changes

- Write clean, documented code
- Follow coding standards
- Test on actual hardware when possible
- Keep commits atomic and well-described

### 3. Test Locally

```bash
# Build the firmware
./build.sh build

# Flash and test on device
./build.sh run
```

### 4. Commit Changes

```bash
git add .
git commit -m "feat: add new feature"
# or
git commit -m "fix: resolve issue #123"
```

**Commit Message Format** (Conventional Commits):
- `feat:` New feature
- `fix:` Bug fix
- `docs:` Documentation changes
- `refactor:` Code refactoring
- `style:` Code style changes (formatting)
- `chore:` Maintenance tasks

### 5. Keep Branch Updated

```bash
# Regularly sync with develop
git fetch origin
git rebase origin/develop
```

### 6. Push and Create Pull Request

```bash
# Push your feature branch
git push origin feature/your-feature-name
```

Create a PR on GitHub targeting `develop` (not `main`).

## Coding Standards

### C Code (ESP-IDF)

- **Style**: Follow ESP-IDF coding style
- **Indentation**: 4 spaces (no tabs)
- **Braces**: K&R style
- **Naming**:
  - Functions: `snake_case`
  - Constants/Macros: `UPPER_SNAKE_CASE`
  - Types: `snake_case_t`
- **Comments**: Use `//` for single line, `/* */` for multi-line

**Example**:
```c
/**
 * @brief Update the display with new sensor data
 *
 * @param data Pointer to sensor data structure
 * @return ESP_OK on success, error code otherwise
 */
esp_err_t ui_update_sensor_display(const sensor_data_t *data)
{
    if (data == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    // Update the UI labels
    lv_label_set_text_fmt(label_power, "%d W", (int)data->current_power);

    return ESP_OK;
}
```

### Header Files

- Use include guards: `#ifndef FILENAME_H` / `#define FILENAME_H` / `#endif`
- Document public functions with Doxygen-style comments
- Keep declarations organized (types, then functions)

### LVGL UI Code

- Use meaningful widget names
- Group related UI elements
- Use styles from `ui_styles.c` for consistency
- Avoid magic numbers - use defines from `config.h`

## Pull Request Process

### Before Submitting

- [ ] Code follows style guidelines
- [ ] Firmware builds without errors
- [ ] Tested on actual hardware (if possible)
- [ ] Documentation updated (if needed)
- [ ] Commit messages are clear
- [ ] Branch is up to date with `develop`
- [ ] PR targets `develop` branch

### PR Target Branches

| Branch Type | Target Branch |
|-------------|---------------|
| `feature/*` | `develop` |
| `fix/*` | `develop` |
| `release/*` | `main` |
| `hotfix/*` | `main` |

### Review Process

1. CI builds the firmware automatically
2. Code review by maintainer
3. Address feedback
4. Approval and merge
5. Delete feature branch after merge

## Issue Reporting

### Bug Reports

Please include:
- Clear description of the issue
- Steps to reproduce
- Expected vs actual behavior
- Hardware setup details
- Serial monitor output (if relevant)
- Screenshots (if UI-related)

### Feature Requests

Please include:
- Clear description of the feature
- Use case / motivation
- Proposed implementation (if you have ideas)
- Willingness to contribute

## Areas for Contribution

### High Priority

- [ ] Additional screen layouts
- [ ] More sensor integrations
- [ ] Localization support
- [ ] Power consumption optimization
- [ ] OTA update support

### Medium Priority

- [ ] Custom themes/color schemes
- [ ] Touch gesture improvements
- [ ] Data logging to SD card
- [ ] Web configuration interface

### Documentation

- [ ] Setup video tutorials
- [ ] Home Assistant configuration examples
- [ ] Troubleshooting guides
- [ ] Hardware modification guides

## Development Tips

### Debugging

**Serial Monitor**:
```bash
./build.sh monitor
# Press Ctrl+] to exit
```

**Add Debug Output**:
```c
#include "esp_log.h"

static const char *TAG = "my_module";

ESP_LOGI(TAG, "Info message: %d", value);
ESP_LOGW(TAG, "Warning message");
ESP_LOGE(TAG, "Error message");
```

### Common Issues

**Build Fails**:
```bash
./build.sh clean
./build.sh build
```

**Flash Fails**:
- Check USB connection
- Verify correct port: `ls /dev/ttyACM* /dev/ttyUSB*`
- Hold BOOT button while flashing

**WiFi Won't Connect**:
- Verify credentials in `main/config.h`
- Check serial output for connection errors
- Ensure 2.4GHz network (ESP32 doesn't support 5GHz)

## Questions?

- Open an issue on GitHub
- Check existing issues and PRs
- Review the documentation

## License

By contributing, you agree that your contributions will be licensed under the MIT license.

---

Thank you for contributing to Waveshare Energy Dashboard!
