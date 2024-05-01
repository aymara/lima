# Copyright 2002-2020 CEA LIST
# SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
#
# SPDX-License-Identifier: MIT

set(test_lima_sources
    test_lima.cpp
    lima.cpp
)
add_executable(test_lima ${test_lima_sources})
target_link_libraries(test_lima
    lima-common-datahandler
    lima-common-data
    lima-common-factory
    lima-common-fsaaccess
    lima-common-mediaprocessors
    lima-common-mediaticdata
    lima-common-misc
    lima-common-processunitframework
    lima-common
    lima-common-time
    lima-common-tools
    lima-common-xmlconfigurationfiles
    lima-linguisticprocessing
    lima-lp-analysisdict
    lima-lp-analysisdumpers
    lima-lp-analysishandlers
    lima-lp-applyrecognizer
    lima-lp-automatoncompiler
    lima-lp-automaton
    lima-lp-bagofwords
    lima-lp-client
    lima-lp-compounds
    lima-lp-conllureader
    lima-lp-corefsolver
    lima-lp-dictionary
    lima-lp-entitytracker
    lima-lp-eventanalyzer
    lima-lp-flattokenizer
    lima-lp-helpers
    lima-lp-langdetector
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
    lima-lp-semanticanalysis
    lima-lp-se-number
    lima-lp-se-person
    lima-lp-specificentities
    lima-lp-syntacticanalysis
    lima-lp-textsegmentation
    lima-lp-tgv
    lima-lp-wordsensetagger
    lima-xp-client
    lima-xp-core
    lima-xp-documentsreader
    lima-xp-qtsgmlentities

)

install(TARGETS test_lima
  DESTINATION bin
  COMPONENT runtime)

########### next test ###############

SET(LimaAnalyzerTest0_SRCS
    LimaAnalyzerTest0.cpp
    lima.cpp
)

set (LimaAnalyzerTest0_MOC_HEADERS LimaAnalyzerTest0.h)

add_executable(LimaAnalyzerTest0 ${LimaAnalyzerTest0_SRCS} ${LimaAnalyzerTest0_MOC_OUTFILES})
target_link_libraries(LimaAnalyzerTest0
    lima-linguisticprocessing
    Qt${QT_VERSION_MAJOR}::Test

    lima-common-datahandler
    lima-common-data
    lima-common-factory
    lima-common-fsaaccess
    lima-common-mediaprocessors
    lima-common-mediaticdata
    lima-common-misc
    lima-common-processunitframework
    lima-common
    lima-common-time
    lima-common-tools
    lima-common-xmlconfigurationfiles
    lima-linguisticprocessing
    lima-lp-analysisdict
    lima-lp-analysisdumpers
    lima-lp-analysishandlers
    lima-lp-applyrecognizer
    lima-lp-automatoncompiler
    lima-lp-automaton
    lima-lp-bagofwords
    lima-lp-client
    lima-lp-compounds
    lima-lp-conllureader
    lima-lp-corefsolver
    lima-lp-dictionary
    lima-lp-entitytracker
    lima-lp-eventanalyzer
    lima-lp-flattokenizer
    lima-lp-helpers
    lima-lp-langdetector
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
    lima-lp-semanticanalysis
    lima-lp-se-number
    lima-lp-se-person
    lima-lp-specificentities
    lima-lp-syntacticanalysis
    lima-lp-textsegmentation
    lima-lp-tgv
    lima-lp-wordsensetagger
    lima-xp-client
    lima-xp-core
    lima-xp-documentsreader
    lima-xp-qtsgmlentities
)

add_test(NAME LimaAnalyzerTest0 COMMAND LimaAnalyzerTest0)

