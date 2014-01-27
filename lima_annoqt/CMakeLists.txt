project(annoqt)
cmake_minimum_required(VERSION 2.8)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/../cmake/modules ${CMAKE_INSTALL_PREFIX})
find_package(Qt4 REQUIRED)

add_subdirectory(src)


########### install files ###############


