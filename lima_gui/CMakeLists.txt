cmake_minimum_required(VERSION 3.0)

enable_testing()

add_definitions(-D QT_NO_KEYWORDS)
include(${CMAKE_SOURCE_DIR}/../manageQt5.cmake)
addQt5Modules(Core Test Qml Gui)
include_directories(${Qt5_INCLUDES})

# set(CMAKE_AUTOMOC ON)

add_subdirectory(src)

add_subdirectory(test)
