set(CONFIG_SOURCES
    appconfig.cpp
)

set(CONFIG_HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/../../include/config/appconfig.h
)

# Tell CMake to run Qt's MOC, UIC, and RCC when necessary
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# Create the config library
add_library(config STATIC ${CONFIG_SOURCES} ${CONFIG_HEADERS})

# Link Qt libraries
target_link_libraries(config PRIVATE
    Qt6::Core
)

# Include directories
target_include_directories(config PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../../include
)
