#   Copyright 2002-2013 CEA LIST
#    
#   This file is part of LIMA.
#
#   LIMA is free software: you can redistribute it and/or modify
#   it under the terms of the GNU Affero General Public License as published by
#   the Free Software Foundation, either version 3 of the License, or
#   (at your option) any later version.
#
#   LIMA is distributed in the hope that it will be useful,
#   but WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#   GNU Affero General Public License for more details.
#
#   You should have received a copy of the GNU Affero General Public License
#   along with LIMA.  If not, see <http://www.gnu.org/licenses/>

# WARNING: should include src BEFORE to prevent to use ${DIST_LIMA} header files
include_directories(BEFORE ${CMAKE_SOURCE_DIR}/src)

# NOTE: lima-common-qslog code has been included in lima-common-tools
#       to solve a circular dependency problem


# add_definitions( -DLIMA_COMMONQSLOG_EXPORTING )
# 
# ########### next target ###############
# 
# set (lima-common-qslog_MOC_HEADERS
#     QsLogCategories.h
# )
# 
# 
# qt5_wrap_cpp (lima-common-qslog_MOC_OUTFILES ${lima-common-qslog_MOC_HEADERS})
# 
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
#   ${QT_LIBRARIES}
# )
# set_target_properties(lima-common-qslog PROPERTIES VERSION ${LIMA_COMMON_LIB_VERSION} SOVERSION ${LIMA_COMMON_LIB_SOVERSION})
# install(TARGETS lima-common-qslog DESTINATION ${LIB_INSTALL_DIR})

########### next target ###############

#SET(qslogger_SRCS
#   main.cpp
#)

#add_executable(qslogger ${qslogger_SRCS})

#target_link_libraries(qslogger lima-common-qslog ${QT_LIBRARIES} )

#install(TARGETS qslogger DESTINATION bin)


########### install files ###############
install(FILES
    QsLog_export.h
    QsLog.h
    QsDebugOutput.h
    QsLogDest.h
    QsLogCategories.h
  DESTINATION include/common/QsLog)
