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


add_definitions(-DLIMA_EVENTANALISYS_EXPORTING)

########### next target ###############

LIMALP_WRAP_HEADERS(eventanalyzer
  EventTemplateDefinitionResource.h
  EventHandler.h
)

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
  lima-common-factory
  lima-common-misc
  lima-common-data
  lima-common-fsaaccess
  lima-common-mediaticdata
  lima-common-time
  lima-common-xmlconfigurationfiles
  lima-common-processunitframework
  lima-common-mediaprocessors
  lima-lp-linguisticprocessors
  lima-lp-linguisticresources
  lima-lp-annotationgraph
  lima-lp-linguisticanalysisstructure
  lima-lp-textsegmentation
  lima-lp-syntacticanalysis
  lima-lp-automaton
  lima-lp-analysishandlers
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
