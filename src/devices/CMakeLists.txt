set(DEVICES_SOURCES
)

set(DEVICES_HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/../../include/devices/irgbdevice.h
    ${CMAKE_CURRENT_SOURCE_DIR}/../../include/devices/irgbdeviceplugin.h
)

# Tell CMake to run Qt's MOC, UIC, and RCC when necessary
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# Hauptbibliothek für Geräteunterstützung (nur Interfaces)
add_library(devices STATIC
    # Keine Quellen hier, nur Interfaces
)

# Link Qt libraries
target_link_libraries(devices PRIVATE
    Qt6::Core
    Qt6::Widgets
)

# Include directories
target_include_directories(devices PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../../include
)
