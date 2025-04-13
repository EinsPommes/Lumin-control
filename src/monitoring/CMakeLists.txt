set(MONITORING_SOURCES
    sensormonitor.cpp
)

set(MONITORING_HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/../../include/monitoring/sensormonitor.h
)

# Tell CMake to run Qt's MOC, UIC, and RCC when necessary
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# Create the monitoring library
add_library(monitoring STATIC ${MONITORING_SOURCES} ${MONITORING_HEADERS})

# Link Qt libraries
target_link_libraries(monitoring PRIVATE
    Qt6::Core
    Qt6::Widgets
)

# Windows-spezifische Bibliotheken
if(WIN32)
    if(MSVC)
        # Nur für MSVC die WMI-Bibliotheken verlinken
        target_link_libraries(monitoring PRIVATE
            wbemuuid
            pdh
        )
    else()
        # Für MinGW nur PDH verlinken, da WMI nicht unterstützt wird
        target_link_libraries(monitoring PRIVATE
            pdh
        )
        # Definiere __MINGW32__ für bedingte Kompilierung
        target_compile_definitions(monitoring PRIVATE __MINGW32__)
    endif()
endif()

# Include directories
target_include_directories(monitoring PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../../include
)
