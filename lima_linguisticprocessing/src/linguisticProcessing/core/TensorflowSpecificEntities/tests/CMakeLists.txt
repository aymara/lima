# SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
#
# SPDX-License-Identifier: MIT

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Find the QtWidgets library

SET(Test_SRCS
  Test.cpp
)

add_executable(TestModuleNER ${Test_SRCS})

# Use the Widgets module from Qt 5.
target_link_libraries(TestModuleNER
  lima-lp-tensorflowspecificentities
  Qt${QT_VERSION_MAJOR}::Core
  Qt${QT_VERSION_MAJOR}::Test
  ${PROJECT_LIBRARIES}
)

message("TestModuleNER WORKING_DIRECTORY is ${CMAKE_CURRENT_SOURCE_DIR}/..")
add_test(
  NAME TestModuleNER
  COMMAND TestModuleNER
  WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/..")

install(TARGETS TestModuleNER RUNTIME DESTINATION bin)
