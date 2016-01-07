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

add_definitions(-DLIMA_SPECIFICENTITIES_EXPORTING)

########### main target ###############

SET(lima-lp-specificentities_LIB_SRCS
   SpecificEntitiesXmlLogger.cpp
   SpecificEntitiesConstraints.cpp
   NormalizationUtils.cpp
   NormalizeDateTimeResources.cpp
   MicrosForNormalization.cpp
   SpecificEntitiesMicros.cpp
   SpecificEntitiesLoader.cpp
)

DECLARE_LIMA_PLUGIN(lima-lp-specificentities)

target_link_libraries(lima-lp-specificentities lima-common-factory lima-common-misc lima-common-data lima-common-fsaaccess lima-common-mediaticdata lima-common-time lima-common-factory lima-common-xmlconfigurationfiles lima-common-processunitframework lima-common-mediaprocessors lima-lp-linguisticprocessors lima-lp-linguisticresources lima-lp-annotationgraph lima-lp-automaton lima-lp-linguisticanalysisstructure lima-lp-syntacticanalysis
)

set_target_properties(lima-lp-specificentities PROPERTIES VERSION ${LIMA_LP_LIB_VERSION} SOVERSION ${LIMA_LP_LIB_SOVERSION})

install(TARGETS lima-lp-specificentities DESTINATION lib)

########### specific targets for normalization actions ###############

SET(lima-lp-se-datetime_LIB_SRCS
  NormalizeDateTime.cpp
)
DECLARE_LIMA_PLUGIN(lima-lp-se-datetime)
target_link_libraries(lima-lp-se-datetime  lima-lp-specificentities lima-lp-automaton  ${QT_LIBRARIES} lima-common-qslog ${Boost_LIBRARIES} lima-lp-misc lima-lp-linguisticprocessing-core lima-lp-linguisticresources lima-lp-flattokenizer lima-lp-propertycode lima-lp-applyrecognizer lima-lp-specificentities lima-lp-syntacticanalysis lima-lp-linguisticdata  lima-lp-bagofwords lima-lp-postagger )
set_target_properties(lima-lp-se-datetime PROPERTIES VERSION ${LIMA_LP_LIB_VERSION} SOVERSION ${LIMA_LP_LIB_SOVERSION})

install(TARGETS lima-lp-se-datetime DESTINATION lib)

SET(lima-lp-se-number_LIB_SRCS
  NormalizeNumber.cpp
)
DECLARE_LIMA_PLUGIN(lima-lp-se-number)
target_link_libraries(lima-lp-se-number  ${QT_LIBRARIES} lima-common-qslog ${Boost_LIBRARIES} lima-lp-misc lima-lp-linguisticprocessing-core lima-lp-linguisticresources lima-lp-flattokenizer lima-lp-propertycode lima-lp-applyrecognizer lima-lp-specificentities lima-lp-syntacticanalysis lima-lp-linguisticdata lima-lp-bagofwords lima-lp-postagger ${optionalLibs})
set_target_properties(lima-lp-se-number PROPERTIES VERSION ${LIMA_LP_LIB_VERSION} SOVERSION ${LIMA_LP_LIB_SOVERSION})

install(TARGETS lima-lp-se-number DESTINATION lib)

SET(lima-lp-se-person_LIB_SRCS
  NormalizePersonName.cpp
  NormalizePerson.cpp
)
DECLARE_LIMA_PLUGIN(lima-lp-se-person)
target_link_libraries(lima-lp-se-person    ${QT_LIBRARIES} lima-common-qslog ${Boost_LIBRARIES} lima-lp-misc lima-lp-linguisticprocessing-core lima-lp-linguisticresources lima-lp-flattokenizer lima-lp-propertycode lima-lp-applyrecognizer lima-lp-specificentities lima-lp-syntacticanalysis lima-lp-linguisticdata lima-lp-bagofwords lima-lp-postagger )
set_target_properties(lima-lp-se-person PROPERTIES VERSION ${LIMA_LP_LIB_VERSION} SOVERSION ${LIMA_LP_LIB_SOVERSION})

install(TARGETS lima-lp-se-person DESTINATION lib)

########### install files ###############
install(FILES  
  NormalizeDateTimeResources.h    
  NormalizationUtils.h 
  NormalizeNumber.h    
  SpecificEntitiesXmlLogger.h 
  NormalizeDateTime.h   
  SpecificEntitiesConstraints.h 
  SpecificEntitiesRecognizer.h 
  NormalizePersonName.h 
  NormalizePerson.h 
  SpecificEntitiesLoader.h
  SpecificEntitiesExport.h
  DESTINATION include/linguisticProcessing/core/SpecificEntities)
