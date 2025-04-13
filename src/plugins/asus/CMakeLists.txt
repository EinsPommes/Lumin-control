set(ASUS_PLUGIN_SOURCES
    asusrgbdeviceplugin.cpp
)

set(ASUS_PLUGIN_HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../include/plugins/asus/asusrgbdeviceplugin.h
)

set(ASUS_PLUGIN_RESOURCES
    asusrgbdeviceplugin.json
)

# Tell CMake to run Qt's MOC, UIC, and RCC when necessary
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# Erstelle das Plugin als Shared Library
add_library(asus_plugin SHARED
    ${ASUS_PLUGIN_SOURCES}
    ${ASUS_PLUGIN_HEADERS}
    ${ASUS_PLUGIN_RESOURCES}
)

# Link Qt libraries
target_link_libraries(asus_plugin PRIVATE
    Qt6::Core
    Qt6::Widgets
    devices
)

# Include directories
target_include_directories(asus_plugin PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../include
)

# Setze den Ausgabepfad für das fertige Plugin
set_target_properties(asus_plugin PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/plugins"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/plugins"
)

# Kopiere die JSON-Metadatendatei in das Ausgabeverzeichnis
add_custom_command(TARGET asus_plugin POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
    ${CMAKE_CURRENT_SOURCE_DIR}/asusrgbdeviceplugin.json
    ${CMAKE_BINARY_DIR}/plugins/asusrgbdeviceplugin.json
)
