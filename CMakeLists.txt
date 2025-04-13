cmake_minimum_required(VERSION 3.16)
project(LuminControl VERSION 0.1.0 LANGUAGES CXX)

# Set C++17 standard (für Qt-Kompatibilität)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Für MSVC: Erforderliche Compiler-Optionen hinzufügen
if(MSVC)
    add_compile_options("/Zc:__cplusplus" "/permissive-" "/bigobj")
    # Zusätzliche Optionen für MSVC, um Qt-MOC-Probleme zu beheben
    add_compile_definitions(_SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING)
endif()

# Qt-Konfiguration
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# Include directories
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

# Find Qt packages
find_package(Qt6 REQUIRED COMPONENTS Core Widgets Charts)

# Explizite Definition der Qt-Include-Verzeichnisse
include_directories(${Qt6Core_INCLUDE_DIRS}
                    ${Qt6Widgets_INCLUDE_DIRS}
                    ${Qt6Charts_INCLUDE_DIRS})

# nlohmann/json als Abhängigkeit hinzufügen
include(FetchContent)
FetchContent_Declare(
    nlohmann_json
    URL https://github.com/nlohmann/json/releases/download/v3.11.2/json.tar.xz
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
FetchContent_MakeAvailable(nlohmann_json)

# Add subdirectories
add_subdirectory(src)

# Set output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(PLUGIN_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/plugins)

# Copy assets to build directory
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/assets DESTINATION ${CMAKE_BINARY_DIR})

# Erstelle Verzeichnisse für Profile und Plugins
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/bin/plugins)
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/bin/profiles)

# Set installation paths
install(DIRECTORY ${CMAKE_BINARY_DIR}/bin/ 
        DESTINATION bin 
        PATTERN "*.exe" 
        PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ
                    GROUP_EXECUTE GROUP_READ
                    WORLD_EXECUTE WORLD_READ)

install(DIRECTORY ${CMAKE_BINARY_DIR}/plugins/ 
        DESTINATION plugins)

install(DIRECTORY ${CMAKE_BINARY_DIR}/assets/ 
        DESTINATION assets)
