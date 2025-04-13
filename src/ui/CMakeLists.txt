set(UI_SOURCES
    mainwindow.cpp
)

set(UI_HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/../../include/ui/mainwindow.h
)

# Tell CMake to run Qt's MOC, UIC, and RCC when necessary
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# Explizite MOC-Einstellungen für Visual Studio
if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /bigobj")
endif()

# Manuelle MOC-Ausführung für mainwindow.h
# qt_wrap_cpp(MOC_FILES ${UI_HEADERS})

# UI-Bibliothek
add_library(ui STATIC ${UI_SOURCES} ${UI_HEADERS})

# Link Qt libraries
target_link_libraries(ui PRIVATE
    Qt6::Core
    Qt6::Widgets
    Qt6::Charts
    config
    core
    devices
    monitoring
)

# Include directories
target_include_directories(ui PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../../include
)
