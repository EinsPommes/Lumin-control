# LuminControl

A central application for controlling RGB devices, effects, fans, and system monitoring.

## Project Overview

LuminControl is an open-source alternative to proprietary RGB and system control programs like Corsair iCUE, ASUS Aura Sync, and Razer Synapse. Built with C++20 and Qt 6, it provides a unified interface for managing all your RGB devices and monitoring system performance.

## Features (Planned)

- **Device Management**: Control all RGB devices through a plugin system
- **RGB Control**: Customize colors and effects for your devices
- **System Monitoring**: Track CPU/GPU temperatures and usage
- **Profile System**: Save and load different configurations

## Project Structure

```
LuminControl/
├── assets/         # Icons, styles, themes
├── include/        # Header files
├── plugins/        # Device plugins (DLLs/SOs)
├── src/
│   ├── config/     # Settings & profile management
│   ├── core/       # RGB logic, device control
│   ├── devices/    # Plugin-based device implementations
│   ├── monitoring/ # Sensor monitoring
│   └── ui/         # Qt GUI components
└── CMakeLists.txt  # Main build configuration
```

## Building the Project

### Prerequisites

- C++20 compatible compiler (GCC 10+, MSVC 2019+, Clang 10+)
- CMake 3.16 or higher
- Qt 6.0 or higher

### Build Instructions

1. Clone the repository
2. Create a build directory:
   ```
   mkdir build
   cd build
   ```
3. Configure with CMake:
   ```
   cmake ..
   ```
4. Build the project:
   ```
   cmake --build .
   ```
5. Run the application:
   ```
   ./bin/LuminControl
   ```

## Plugin System

LuminControl uses a plugin system to support various RGB devices. Plugins are implemented as dynamic libraries (DLLs/SOs) that implement the `IRGBDevicePlugin` interface.

## License

This project is open source and available under the MIT License.
