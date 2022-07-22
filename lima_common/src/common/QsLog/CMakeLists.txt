# Copyright 2002-2019 CEA LIST
# SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
#
# SPDX-License-Identifier: MIT

# WARNING: should include src BEFORE to prevent to use ${DIST_LIMA} header files
include_directories(BEFORE ${CMAKE_SOURCE_DIR}/src)

# NOTE: lima-common-qslog code has been included in lima-common-tools
#       to solve a circular dependency problem


# add_definitions( -DLIMA_COMMONQSLOG_EXPORTING )
#
# ########### next target ###############
#
# SET(lima-common-qslog_LIB_SRCS
#     QsLog.cpp
#     QsDebugOutput.cpp
#     QsLogDest.cpp
#     QsLogCategories.cpp
#    )
#
# add_library(lima-common-qslog SHARED ${lima-common-qslog_LIB_SRCS} ${lima-common-qslog_MOC_OUTFILES})
# target_link_libraries(lima-common-qslog
#   lima-common-qslog
#   ${Qt_LIBRARIES}
# )
# ADD_DLL_TO_PATH(lima-common-qslog)
#
# set_target_properties(lima-common-qslog PROPERTIES VERSION ${LIMA_COMMON_LIB_VERSION} SOVERSION ${LIMA_COMMON_LIB_SOVERSION})
# install(TARGETS lima-common-qslog DESTINATION ${LIB_INSTALL_DIR})

########### next target ###############

SET(qslogger_SRCS
  main.cpp
)

add_executable(qslogger ${qslogger_SRCS})

target_link_libraries(qslogger
  lima-common-tools
)

install(TARGETS qslogger DESTINATION bin
  COMPONENT runtime)


########### install files ###############
install(FILES
    QsLog_export.h
    QsLog.h
    QsDebugOutput.h
    QsLogDest.h
    QsLogCategories.h
  DESTINATION include/common/QsLog
  COMPONENT devel
)
