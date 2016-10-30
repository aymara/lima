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
include_directories(BEFORE ${CMAKE_SOURCE_DIR}/src )

add_definitions(-DLIMA_LINGUISTICPROCESSORS_EXPORTING)

########### next target ###############

SET(lima-lp-linguisticprocessors_LIB_SRCS
   FullTokenXmlLogger.cpp 
   StatusLogger.cpp
   LinguisticMetaData.cpp
#    HandlerStreamBuf.cpp
#    DumperStream.cpp
#    AbstractAnalysisDumper.cpp
   AbstractTextualAnalysisDumper.cpp
   ExternalProcessUnit.cpp
   AnalysisLoader.cpp
)

DECLARE_LIMA_PLUGIN(lima-lp-linguisticprocessors)

target_link_libraries(lima-lp-linguisticprocessors
  lima-lp-misc
  lima-lp-analysishandlers
  lima-lp-annotationgraph
  lima-lp-linguisticresources
  lima-lp-linguisticanalysisstructure
  lima-common
  ${Boost_LIBRARIES}
  ${QT_LIBRARIES})

set_target_properties(lima-lp-linguisticprocessors PROPERTIES 
  VERSION ${LIMA_LP_LIB_VERSION} 
  SOVERSION ${LIMA_LP_LIB_SOVERSION})

install(TARGETS lima-lp-linguisticprocessors DESTINATION ${LIB_INSTALL_DIR})


########### install files ###############
install(FILES 
  AbstractTextualAnalysisDumper.h
  AnalysisLoader.h
  ExternalProcessUnit.h 
  FullTokenXmlLogger.h 
  LinguisticAnalysisDumper.h 
  LinguisticMetaData.h 
  LinguisticProcessorsExport.h
  StatusLogger.h 
  DESTINATION include/linguisticProcessing/core/LinguisticProcessors)
