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
add_subdirectory(libautomatonCompiler)

include_directories(BEFORE ${CMAKE_SOURCE_DIR}/src )

# IF(NOT SVMTOOL_LIBRARIES STREQUAL "SVMTOOL_LIBRARIES-NOTFOUND")
# SET(optionalLibs ${optionalLibs} svmtool)
# ENDIF(NOT SVMTOOL_LIBRARIES STREQUAL "SVMTOOL_LIBRARIES-NOTFOUND")


########### next target ###############

SET(compile-rules_SRCS
  compile-rules.cpp
  )

add_executable(compile-rules ${compile-rules_SRCS})

target_link_libraries(compile-rules
  lima-common-mediaprocessors
  lima-common-mediaticdata
  lima-common-processunitframework
  lima-common-xmlconfigurationfiles
  lima-common-data
  lima-common-factory
  lima-common-misc
  lima-common-tools
  lima-common-fsaaccess
  lima-common-qslog
  lima-lp-automatoncompiler
  lima-lp-bagofwords
  lima-lp-applyrecognizer
  lima-lp-automaton
  lima-lp-annotationgraph
  lima-lp-linguisticdata
  lima-lp-client
  lima-lp-linguisticprocessing-core
  lima-lp-linguisticanalysisstructure
  lima-lp-linguisticprocessors
  lima-lp-linguisticresources
  lima-lp-misc
  lima-lp-analysisdict
  lima-lp-semanticanalysis
  lima-lp-morphologicanalysis
  lima-lp-postagger
  lima-lp-propertycode
  lima-lp-specificentities
  lima-lp-syntacticanalysis
  lima-lp-flattokenizer
  ${optionalLibs}
  ${Boost_LIBRARIES}
  ${QT_LIBRARIES}
)

install(TARGETS compile-rules DESTINATION bin)


########### install files ###############
