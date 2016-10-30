#   Copyright 2004-2014 CEA LIST
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
########### next target ###############

SET(easyprocessor_SRCS
  easyprocessor.cpp 
  EasySourceHandler.cpp
)

add_executable(easyprocessor ${easyprocessor_SRCS})
target_link_libraries(easyprocessor
    lima-common-processunitframework 
    lima-common-fsaaccess 
    lima-common-tools 
    lima-common-xmlconfigurationfiles 
    lima-common-factory 
    lima-common-data 
    lima-common-misc 
    lima-common-datahandler 
    lima-common-time 
    lima-common-mediaprocessors 
    lima-common-mediaticdata

    lima-lp-analysisdict
    lima-lp-analysisdumpers
    lima-lp-analysishandlers
    lima-lp-annotationgraph
    lima-lp-applyrecognizer
    lima-lp-automaton
    lima-lp-bagofwords
    lima-lp-client
    lima-lp-corefsolver
    lima-lp-dictionary
    lima-lp-easyxmldumper
    lima-lp-entitytracker
    lima-lp-eventanalyzer
    lima-lp-flattokenizer
    lima-lp-lineartextrepresentation
    lima-lp-linguisticanalysisstructure
    lima-lp-linguisticdata
    lima-lp-linguisticprocessing-core
    lima-lp-linguisticprocessors
    lima-lp-linguisticresources
    lima-lp-misc
    lima-lp-morphologicanalysis
    lima-lp-postagger
    lima-lp-propertycode
    lima-lp-regexmatcher
    lima-lp-se-datetime
    lima-lp-se-number
    lima-lp-se-person
    lima-lp-semanticanalysis
    lima-lp-specificentities
    lima-lp-syntacticanalysis
    lima-lp-textsegmentation
    lima-lp-tgv
    lima-lp-wordsensetagger

    ${optionalLibs}
    ${Boost_LIBRARIES}
    ${QT_LIBRARIES}
)

install(TARGETS easyprocessor DESTINATION bin)

install(FILES
    easyprocessor.conf.xml
  DESTINATION share/config/lima)

install(FILES 
    easy.dtd
    passage.dtd
  DESTINATION share/apps/pelf/resources)
