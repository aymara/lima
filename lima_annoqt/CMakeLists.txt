project(annoqt)
cmake_minimum_required(VERSION 3.4.0)

enable_testing()

# include(${CMAKE_SOURCE_DIR}/../SystemSpecificInformations.cmake)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/../cmake/modules)

# include(${CMAKE_SOURCE_DIR}/../manageQt5.cmake)
addQt5Modules(Core Widgets Xml)
# include_directories(SYSTEM ${Qt5_INCLUDES})
# set(Qt5_LIBRARIES "${Qt5_LIBRARIES}")

set(LIMA_LP_LIB_VERSION ${LIMA_GENERIC_LIB_VERSION})
set(LIMA_LP_LIB_SOVERSION ${LIMA_GENERIC_LIB_SOVERSION})

add_subdirectory(src)

########### install files ###############

