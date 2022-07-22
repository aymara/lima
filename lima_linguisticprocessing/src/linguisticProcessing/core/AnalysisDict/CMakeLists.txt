# Copyright 2002-2019 CEA LIST
# SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
#
# SPDX-License-Identifier: MIT

include_directories(BEFORE ${CMAKE_SOURCE_DIR}/src )

add_definitions(-DLIMA_ANALYSISDICT_EXPORTING)

########### next target ###############
SET(lima-lp-analysisdict_LIB_SRCS
    AbstractAnalysisDictionary.cpp
    AbstractRwAccessResource.cpp
    AnalysisDictionaryIterators.cpp
    FsaAccessResource.cpp
    FsaRwAccessResource.cpp
    AbstractDictionaryEntry.cpp
    AbstractDictionaryEntryHandler.cpp
    DictionaryData.cpp
    EnhancedAnalysisDictionary.cpp
    EnhancedAnalysisDictionaryEntry.cpp
    EnhancedAnalysisDictionaryIterator.cpp
    MultiLevelAnalysisDictionary.cpp
    MultiLevelAnalysisDictionaryEntry.cpp
    MultiLevelAnalysisDictionaryIterators.cpp
)

add_library(lima-lp-analysisdict SHARED ${lima-lp-analysisdict_LIB_SRCS})

target_link_libraries(lima-lp-analysisdict
  lima-lp-flattokenizer
  lima-lp-linguisticresources
)

ADD_DLL_TO_PATH(lima-lp-analysisdict)

set_target_properties(lima-lp-analysisdict PROPERTIES
                      VERSION ${LIMA_LP_LIB_VERSION}
                      SOVERSION ${LIMA_LP_LIB_SOVERSION})

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

