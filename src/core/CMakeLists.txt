set(CORE_SOURCES
    devicemanager.cpp
    color.cpp
    effect.cpp
    rgbcontroller.cpp
    profilemanager.cpp
)

set(CORE_HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/../../include/core/devicemanager.h
    ${CMAKE_CURRENT_SOURCE_DIR}/../../include/core/color.h
    ${CMAKE_CURRENT_SOURCE_DIR}/../../include/core/effect.h
    ${CMAKE_CURRENT_SOURCE_DIR}/../../include/core/rgbcontroller.h
    ${CMAKE_CURRENT_SOURCE_DIR}/../../include/core/profilemanager.h
)

# Tell CMake to run Qt's MOC, UIC, and RCC when necessary
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# Finde nlohmann_json
find_package(nlohmann_json QUIET)
if(NOT nlohmann_json_FOUND)
    # Wenn nicht gefunden, füge es als FetchContent hinzu
    include(FetchContent)
    FetchContent_Declare(
        nlohmann_json
        GIT_REPOSITORY https://github.com/nlohmann/json.git
        GIT_TAG v3.11.2
    )
    FetchContent_MakeAvailable(nlohmann_json)
endif()

# Create the core library
add_library(core STATIC ${CORE_SOURCES} ${CORE_HEADERS})

# Link Qt libraries
target_link_libraries(core PRIVATE
    Qt6::Core
    Qt6::Widgets
    nlohmann_json::nlohmann_json
)

# Include directories
target_include_directories(core PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../../include
)
