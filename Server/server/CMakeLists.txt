cmake_minimum_required(VERSION 3.5)

project(server LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


find_package(QT NAMES Qt6 Qt5 COMPONENTS Widgets Gui Network REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets Gui Network REQUIRED)

set(PROJECT_SOURCES
        main.cpp
        server.cpp
        server.h
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(server
        ${PROJECT_SOURCES}
    )
else()
    if(ANDROID)
        add_library(server SHARED
            ${PROJECT_SOURCES}
        )
    else()
        add_executable(server
            ${PROJECT_SOURCES}
        )
    endif()
endif()

message(${PROJECT_SOURCE_DIR})
include_directories(${PROJECT_SOURCE_DIR}/../shared_lib/shared_lib)
target_compile_definitions(server PUBLIC SHARED_APP_DATA_PATH="${PROJECT_SOURCE_DIR}")
target_link_libraries(server PRIVATE Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Gui Qt${QT_VERSION_MAJOR}::Network
    ${PROJECT_SOURCE_DIR}/../shared_lib/build/debug/libshared_lib.so)

