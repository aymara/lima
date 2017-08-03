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

add_definitions(-DLIMA_ANALYSISDICT_EXPORTING)

########### next target ###############
LIMALP_WRAP_HEADERS(analysisdict
  AbstractAnalysisDictionary.h
  EnhancedAnalysisDictionary.h
  FsaAccessResource.h
)

SET(lima-lp-analysisdict_LIB_SRCS
   AnalysisDictionaryIterators.cpp 
  FsaAccessResource.cpp FsaRwAccessResource.cpp AbstractDictionaryEntry.cpp
  AbstractDictionaryEntryHandler.cpp DictionaryData.cpp EnhancedAnalysisDictionary.cpp
  EnhancedAnalysisDictionaryEntry.cpp EnhancedAnalysisDictionaryIterator.cpp MultiLevelAnalysisDictionary.cpp
  MultiLevelAnalysisDictionaryEntry.cpp MultiLevelAnalysisDictionaryIterators.cpp
)

add_library(lima-lp-analysisdict SHARED ${lima-lp-analysisdict_LIB_SRCS} ${lima-lp-analysisdict_MOC_OUTFILES})

target_link_libraries(lima-lp-analysisdict
  lima-lp-flattokenizer
  lima-lp-linguisticresources
  lima-common
  ${Boost_LIBRARIES}
  ${QT_LIBRARIES}
)

set_target_properties(lima-lp-analysisdict PROPERTIES VERSION ${LIMA_LP_LIB_VERSION} SOVERSION ${LIMA_LP_LIB_SOVERSION})

install(TARGETS lima-lp-analysisdict DESTINATION ${LIB_INSTALL_DIR}
  COMPONENT runtime)


########### install files ###############
install(FILES
    FsaAccessResource.h
    AbstractDictionaryEntry.h 
    AbstractDictionaryEntryHandler.h 
    AbstractDictionaryEntry.tcc
    DictionaryData.h 
    EnhancedAnalysisDictionary.h 
    EnhancedAnalysisDictionaryEntry.h
    EnhancedAnalysisDictionaryIterator.h 
    MultiLevelAnalysisDictionary.h 
    MultiLevelAnalysisDictionaryEntry.h
    MultiLevelAnalysisDictionaryIterators.h
    AbstractAnalysisDictionary.h 
    AnalysisDictionaryIterators.h
    AnalysisDictExport.h
  DESTINATION include/linguisticProcessing/core/AnalysisDict
  COMPONENT devel
)

# FIND_PACKAGE(LIMALDATA PATHS ${CMAKE_INSTALL_PREFIX}/share/apps/lima/cmake/Modules)
# IF(LIMALDATA_FOUND)
#  add_subdirectory(Tests)
# ELSE (LIMALDATA_FOUND)
#   MESSAGE(STATUS "Could not find lingustic ressources to run test")
# ENDIF(LIMALDATA_FOUND)
