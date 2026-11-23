# SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
#
# SPDX-License-Identifier: MIT

################### tests ########################

########### next test ###############

# On Windows, the test's expectations (a change notification while
# QTemporaryFile still holds the file) do not hold: it was already disabled
# there with LIMA_DISABLE_FSW_TESTING by the former Windows build script.
if (NOT WIN32 AND "x$ENV{LIMA_DISABLE_FSW_TESTING}" STREQUAL "x")
message(STATUS "LIMA_DISABLE_FSW_TESTING is NOT set: build tests for LimaFileSystemWatcher")

SET(LimaFileSystemWatcherTest0_SRCS
  LimaFileSystemWatcherTest.cpp
)

# The parent directory's add_definitions(-D..._EXPORTING) is inherited here, but
# the test must import these classes from the DLLs, not define them.
remove_definitions(-DLIMA_COMMONTOOLS_EXPORTING -DLIMA_COMMONQSLOG_EXPORTING)

add_executable(LimaFileSystemWatcherTest0
  ${LimaFileSystemWatcherTest0_SRCS}
  ${LimaFileSystemWatcherTest0_MOC_OUTFILES})

target_link_libraries(LimaFileSystemWatcherTest0
  lima-common-factory
  lima-common-tools
  Qt${QT_VERSION_MAJOR}::Test
)

add_test(NAME LimaFileSystemWatcherTest0 COMMAND LimaFileSystemWatcherTest0)
set_tests_properties(LimaFileSystemWatcherTest0 PROPERTIES ENVIRONMENT LIMA_CONF=${CMAKE_SOURCE_DIR}/conf)

else()
message(STATUS "LIMA_DISABLE_FSW_TESTING is set: do NOT build tests for LimaFileSystemWatcher")
endif()

