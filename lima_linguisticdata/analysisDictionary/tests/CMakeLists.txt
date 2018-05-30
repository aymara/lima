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
########### next test ###############

set(EnhancedAnalysisDictionaryTest0_SRCS
   EnhancedAnalysisDictionaryTest0.cpp
)

set (EnhancedAnalysisDictionaryTest0_MOC_HEADERS 
  EnhancedAnalysisDictionaryTest0.h
)

qt5_wrap_cpp (EnhancedAnalysisDictionaryTest0_MOC_OUTFILES 
  ${EnhancedAnalysisDictionaryTest0_MOC_HEADERS}
)

add_executable(EnhancedAnalysisDictionaryTest0 
  ${EnhancedAnalysisDictionaryTest0_SRCS}
  ${EnhancedAnalysisDictionaryTest0_MOC_OUTFILES}
)

target_link_libraries(EnhancedAnalysisDictionaryTest0
  lima-common-factory
  lima-common-fsaaccess
  lima-common-xmlconfigurationfiles
  lima-common-tools
  lima-common-datahandler 
  lima-common-mediaprocessors 
  lima-common-time 
  lima-common-misc
  lima-common-data 
  lima-common-mediaticdata
  lima-lp-annotationgraph
  lima-lp-lineartextrepresentation
  lima-lp-misc
  lima-lp-tgv
  lima-lp-linguisticdata
  lima-lp-bagofwords
  lima-lp-propertycode
  lima-lp-regexmatcher
  lima-lp-wordsensetagger
  lima-lp-linguisticprocessing-core
  lima-lp-analysisdict
  lima-lp-eventanalyzer
  lima-lp-corefsolver
  lima-lp-specificentities
  lima-lp-syntacticanalysis
  lima-lp-morphologicanalysis
  lima-lp-dictionary
  lima-lp-easyxmldumper
  lima-lp-analysisdumpers
  lima-lp-textsegmentation
  lima-lp-linguisticanalysisstructure
  lima-lp-semanticanalysis
  lima-lp-linguisticresources
  lima-lp-entitytracker
  lima-lp-flattokenizer
  lima-lp-applyrecognizer
  lima-lp-linguisticprocessors
  lima-lp-postagger
  lima-lp-automaton
  lima-lp-analysishandlers
  lima-lp-client
  ${Boost_LIBRARIES} 
  ${QT_LIBRARIES}
)

add_test(NAME EnhancedAnalysisDictionaryTest0 
  COMMAND EnhancedAnalysisDictionaryTest0
)

########### next test ###############

set(EnhancedAnalysisDictionaryTest1_SRCS
   EnhancedAnalysisDictionaryTest1.cpp
)

set (EnhancedAnalysisDictionaryTest1_MOC_HEADERS 
  EnhancedAnalysisDictionaryTest1.h
)

qt5_wrap_cpp (EnhancedAnalysisDictionaryTest1_MOC_OUTFILES 
  ${EnhancedAnalysisDictionaryTest1_MOC_HEADERS}
)

add_executable(EnhancedAnalysisDictionaryTest1 
  ${EnhancedAnalysisDictionaryTest1_SRCS}
  ${EnhancedAnalysisDictionaryTest1_MOC_OUTFILES}
)

target_link_libraries(EnhancedAnalysisDictionaryTest1 
  lima-common-factory
  lima-common-fsaaccess
  lima-common-xmlconfigurationfiles
  lima-common-tools
  lima-common-datahandler 
  lima-common-mediaprocessors 
  lima-common-time 
  lima-common-misc
  lima-common-data 
  lima-common-mediaticdata
  lima-lp-annotationgraph
  lima-lp-lineartextrepresentation
  lima-lp-misc
  lima-lp-tgv
  lima-lp-linguisticdata
  lima-lp-bagofwords
  lima-lp-propertycode
  lima-lp-regexmatcher
  lima-lp-wordsensetagger
  lima-lp-linguisticprocessing-core
  lima-lp-analysisdict
  lima-lp-eventanalyzer
  lima-lp-corefsolver
  lima-lp-specificentities
  lima-lp-syntacticanalysis
  lima-lp-morphologicanalysis
  lima-lp-dictionary
  lima-lp-easyxmldumper
  lima-lp-analysisdumpers
  lima-lp-textsegmentation
  lima-lp-linguisticanalysisstructure
  lima-lp-semanticanalysis
  lima-lp-linguisticresources
  lima-lp-entitytracker
  lima-lp-flattokenizer
  lima-lp-applyrecognizer
  lima-lp-linguisticprocessors
  lima-lp-postagger
  lima-lp-automaton
  lima-lp-analysishandlers
  lima-lp-client
  ${Boost_LIBRARIES} 
  ${QT_LIBRARIES}
)

add_test(NAME EnhancedAnalysisDictionaryTest1 
  COMMAND EnhancedAnalysisDictionaryTest1
)


########### next test ###############

set (DictionaryDataTest1_MOC_HEADERS 
  DictionaryDataTest1.h
)

qt5_wrap_cpp (DictionaryDataTest1_MOC_OUTFILES
  ${DictionaryDataTest1_MOC_HEADERS}
)


SET(DictionaryDataTest1_SRCS
   DictionaryDataTest1.cpp
)

add_executable(DictionaryDataTest1 
  ${DictionaryDataTest1_SRCS}
  ${DictionaryDataTest1_MOC_OUTFILES}
)

target_link_libraries(DictionaryDataTest1 
  lima-common-factory
  lima-common-xmlconfigurationfiles
  lima-common-tools
  lima-common-datahandler 
  lima-common-mediaprocessors 
  lima-common-time 
  lima-common-misc
  lima-common-data 
  lima-common-mediaticdata
  lima-lp-annotationgraph
  lima-lp-lineartextrepresentation
  lima-lp-misc
  lima-lp-tgv
  lima-lp-linguisticdata
  lima-lp-bagofwords
  lima-lp-propertycode
  lima-lp-regexmatcher
  lima-lp-wordsensetagger
  lima-lp-linguisticprocessing-core
  lima-lp-analysisdict
  lima-lp-eventanalyzer
  lima-lp-corefsolver
  lima-lp-specificentities
  lima-lp-syntacticanalysis
  lima-lp-morphologicanalysis
  lima-lp-dictionary
  lima-lp-easyxmldumper
  lima-lp-analysisdumpers
  lima-lp-textsegmentation
  lima-lp-linguisticanalysisstructure
  lima-lp-semanticanalysis
  lima-lp-linguisticresources
  lima-lp-entitytracker
  lima-lp-flattokenizer
  lima-lp-applyrecognizer
  lima-lp-linguisticprocessors
  lima-lp-postagger
  lima-lp-automaton
  lima-lp-analysishandlers
  lima-lp-client
  ${Boost_LIBRARIES} 
  ${QT_LIBRARIES}
)

add_test(NAME DictionaryDataTest1 COMMAND DictionaryDataTest1)

