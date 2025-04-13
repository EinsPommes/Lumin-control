# Unterverzeichnisse hinzufügen
add_subdirectory(config)
add_subdirectory(core)
add_subdirectory(devices)
add_subdirectory(monitoring)

# UI-Quellen direkt in die Hauptanwendung einbinden
set(UI_SOURCES
    ui/mainwindow.cpp
)

set(UI_HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/../include/ui/mainwindow.h
)

set(UI_UI_FILES
    ui/mainwindow.ui
)

# Qt-Konfiguration für UI
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# Hauptanwendung
add_executable(LuminControl
    main.cpp
    ${UI_SOURCES}
    ${UI_HEADERS}
)

# Qt-Module für Hauptanwendung verlinken
target_link_libraries(LuminControl PRIVATE
    Qt6::Core
    Qt6::Widgets
    Qt6::Charts
    config
    core
    devices
    monitoring
)

# Include-Verzeichnisse für Hauptanwendung
target_include_directories(LuminControl PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../include
)

# Ausgabeverzeichnis festlegen
set_target_properties(LuminControl PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

# Installation
install(TARGETS LuminControl
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)
