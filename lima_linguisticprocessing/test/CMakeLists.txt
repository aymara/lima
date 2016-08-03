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

include_directories($ENV{LIMA_EXTERNALS}}/include)

# QHttpServer is necessary for limaserver HTTP server
find_package(QHttpServer QUIET)
if (QHTTPSERVER_FOUND)
  message("QHttpServer found : Lima HTTP server will be built")
else()
  message("QHttpServer NOT found : Lima HTTP server will NOT be built")
endif()

add_subdirectory(testAnnotationGraphs)
add_subdirectory(perlTCPClientServer)
if (QHTTPSERVER_FOUND)
  add_subdirectory(limaServer)
endif()

include_directories(BEFORE ${CMAKE_SOURCE_DIR}/src )

FIND_LIBRARY(SVMTOOL_LIBRARIES NAMES svmtool PATHS $ENV{MM_EXTERNALS}/lib)

# SET(optionalLibs)
# 
# IF(SVMTOOL++_FOUND)
#   SET(optionalLibs ${optionalLibs} svmtool)
# ENDIF(SVMTOOL++_FOUND)
# 
########### next target ###############

SET(showinttypessizes_SRCS
   showinttypessizes.cpp
)

add_executable(showinttypessizes ${showinttypessizes_SRCS})
target_link_libraries(showinttypessizes)

install(TARGETS showinttypessizes DESTINATION bin)

########### next target ###############

SET(analyzeText_SRCS
   analyzeText.cpp
)

add_executable(analyzeText ${analyzeText_SRCS})
MESSAGE(STATUS "Optional libs: ${optionalLibs}")
MESSAGE(STATUS "Boost_LIBRARIES: ${Boost_LIBRARIES}")
target_link_libraries(analyzeText
    lima-common
    lima-linguisticprocessing
    ${optionalLibs}
    ${Boost_LIBRARIES}
    ${QT_LIBRARIES}
)

install(TARGETS analyzeText DESTINATION bin)

########### next target ###############
if (${PYTHONLIBS_FOUND})
  SET(srl_SRCS
    srl.cpp
  )

  add_executable(srl ${srl_SRCS})
  target_link_libraries(srl
      ${PYTHON_LIBRARY}
      ${QT_LIBRARIES}
  )

  install(TARGETS srl DESTINATION bin)
endif()
########### next target ###############

# SET(threadedAnalyzeText_SRCS
#    threadedAnalyzeText.cpp
# )
# 
# add_executable(threadedAnalyzeText ${threadedAnalyzeText_SRCS})
# 
# target_link_libraries(threadedAnalyzeText
#     lima-common
#     lima-linguisticprocessing
#   ${optionalLibs}
#   ${QT_LIBRARIES}
#   ${Boost_LIBRARIES}
# )
# 
# install(TARGETS threadedAnalyzeText DESTINATION bin)


########### next target ###############

SET(analyzetextservercore_SRCS
   analyzetextservercore.cpp
)

add_executable(analyzetextservercore ${analyzetextservercore_SRCS})

target_link_libraries(analyzetextservercore
  lima-common
  lima-linguisticprocessing

  ${Boost_LIBRARIES}
  ${QT_LIBRARIES}
  ${optionalLibs}
)

install(TARGETS analyzetextservercore DESTINATION bin)


########### install files ###############
