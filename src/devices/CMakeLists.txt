set(DEVICES_SOURCES
    asusrgbdevice.cpp
    asusdevicemanager.cpp
)

set(DEVICES_HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/../../include/devices/irgbdevice.h
    ${CMAKE_CURRENT_SOURCE_DIR}/../../include/devices/irgbdeviceplugin.h
    ${CMAKE_CURRENT_SOURCE_DIR}/../../include/devices/asusrgbdevice.h
    ${CMAKE_CURRENT_SOURCE_DIR}/../../include/devices/asusdevicemanager.h
)

# Tell CMake to run Qt's MOC, UIC, and RCC when necessary
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# Hauptbibliothek für Geräteunterstützung
add_library(devices STATIC
    ${DEVICES_SOURCES}
    ${DEVICES_HEADERS}
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
