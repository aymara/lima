# Copyright 2002-2019 CEA LIST
# SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
#
# SPDX-License-Identifier: MIT

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
   MetaDataDumper.cpp
)

DECLARE_LIMA_PLUGIN(lima-lp-linguisticprocessors)

target_link_libraries(lima-lp-linguisticprocessors
  lima-common-mediaprocessors
  lima-lp-misc
  lima-lp-analysishandlers
  lima-lp-annotationgraph
  lima-lp-linguisticresources
  lima-lp-linguisticanalysisstructure
)

set_target_properties(lima-lp-linguisticprocessors PROPERTIES
  VERSION ${LIMA_LP_LIB_VERSION}
  SOVERSION ${LIMA_LP_LIB_SOVERSION})

install(TARGETS lima-lp-linguisticprocessors DESTINATION ${LIB_INSTALL_DIR}
  COMPONENT runtime)


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
    MetaDataDumper.h
  DESTINATION include/linguisticProcessing/core/LinguisticProcessors
  COMPONENT devel
)
