# Define project
project(pelf)
cmake_minimum_required(VERSION 2.8)

# Find required packages
find_package(Qt4 REQUIRED)
set(QT_USE_QTXML 1)

# Go to subdirectories
add_subdirectory(src)
