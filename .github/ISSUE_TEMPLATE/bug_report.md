---
name: Bug Report
about: Create a report to help us improve
title: '[BUG] '
labels: bug
assignees: ''
---

## Bug Description
A clear and concise description of what the bug is.

## Steps to Reproduce
1. Flash firmware with '...'
2. Navigate to '...'
3. Observe '...'
4. See error

## Expected Behavior
A clear and concise description of what you expected to happen.

## Actual Behavior
What actually happened instead.

## Screenshots / Photos
If applicable, add photos of the display to help explain your problem.

## Environment
- **Hardware**: [e.g., Waveshare ESP32-S3-Touch-LCD-7 rev 1.2]
- **ESP-IDF Version**: [e.g., v5.4]
- **OS**: [e.g., Ubuntu 22.04, macOS 13.0, Windows 11]
- **Home Assistant Version**: [e.g., 2024.1.0]
- **MQTT Broker**: [e.g., Mosquitto 2.0.18]

## Serial Monitor Output
```
Paste relevant serial output here.
Use: ./build.sh monitor
```

## Configuration
<!-- Remove sensitive data like passwords! -->
```c
// Relevant parts of config.h (without secrets)
#define MQTT_BROKER_URI     "mqtt://192.168.x.x:1883"
// Topics being used...
```

## Additional Context
Add any other context about the problem here.

## Possible Solution
(Optional) Suggest a fix or reason for the bug.
