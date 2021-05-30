cmake_minimum_required(VERSION 3.5)

project(client LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(${CMAKE_BINARY_DIR})
find_package(QT NAMES Qt6 Qt5 COMPONENTS Widgets Gui Network REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets Gui Network REQUIRED)

set(PROJECT_SOURCES
        main.cpp
        client.cpp
        client.h
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(client
        ${PROJECT_SOURCES}
    )
else()
    if(ANDROID)
        add_library(client SHARED
            ${PROJECT_SOURCES}
        )
    else()
        add_executable(client
            ${PROJECT_SOURCES}
        )
    endif()
endif()

message("pwd = " ${PROJECT_BINARY_DIR})
message(${PROJECT_SOURCE_DIR})
target_compile_definitions(client PUBLIC SHARED_APP_DATA_PATH="${PROJECT_SOURCE_DIR}")
target_link_libraries(client PRIVATE Qt${QT_VERSION_MAJOR}::Widgets
     Qt${QT_VERSION_MAJOR}::Gui Qt${QT_VERSION_MAJOR}::Network)
