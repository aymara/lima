# Copyright 2002-2019 CEA LIST
# SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
#
# SPDX-License-Identifier: MIT

include_directories(BEFORE ${CMAKE_SOURCE_DIR}/src )


add_definitions(-DLIMA_EVENTANALISYS_EXPORTING)

########### next target ###############

SET(lima-lp-eventanalyzer_LIB_SRCS EventDataDumper.cpp
  Entity.cpp
  Event.cpp
  Events.cpp
  EventAnalyzer.cpp
  EventDumper.cpp
  EventHandler.cpp
  EventParagraph.cpp
  Paragraph.cpp
  EventXmlLogger.cpp
  EventSegmentation.cpp
  SimpleEventBuilder.cpp
  EventData.cpp
  EventTemplateData.cpp
  EventTemplateDataXmlLogger.cpp
  EventTemplateDataDumper.cpp
  EventTemplate.cpp
  EventTemplateDefinitionResource.cpp
  EventTemplateStructure.cpp
  EventTemplateFilling.cpp
  EventTemplateFillingActions.cpp
  EventTemplateMerging.cpp
  EventTemplateElement.cpp
)

DECLARE_LIMA_PLUGIN(lima-lp-eventanalyzer)

target_link_libraries(lima-lp-eventanalyzer
  lima-common-time
  lima-common-xmlconfigurationfiles
  lima-lp-linguisticprocessors
  lima-lp-textsegmentation
  lima-lp-automaton
  lima-lp-applyrecognizer
)

set_target_properties(lima-lp-eventanalyzer PROPERTIES VERSION ${LIMA_LP_LIB_VERSION} SOVERSION ${LIMA_LP_LIB_SOVERSION})

install(TARGETS lima-lp-eventanalyzer DESTINATION ${LIB_INSTALL_DIR}
  COMPONENT runtime)

########### install files ###############
install(FILES
    EventAnalysisExport.h
    EventAnalyzer.h
    Events.h
    EventDumper.h
    EventParagraph.h
    Paragraph.h
    EventHandler.h
    Event.h
    Entity.h
    EventXmlLogger.h
    EventSegmentation.h
    SimpleEventBuilder.h
    EventTemplateData.h
    EventTemplate.h
    EventTemplateElement.h
    EventTemplateDefinitionResource.h
    EventTemplateStructure.h
    EventData.h
  DESTINATION include/linguisticProcessing/core/EventAnalysis
  COMPONENT devel
)
