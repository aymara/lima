# Copyright 2002-2020 CEA LIST
# SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
#
# SPDX-License-Identifier: MIT

project(LimaLinguisticData)

if (ENVPATH)
  set(ENV{PATH} "${ENVPATH}:$ENV{PATH}")
endif()
if (ENVPERL5LIB)
  set(ENV{PERL5LIB} "${ENVPERL5LIB}:$ENV{PERL5LIB}")
endif()
message("lima_linguisticdata PERL5LIB=$ENV{PERL5LIB}")
message("lima_linguisticdata PATH=$ENV{PATH}")

# message("CMAKE_BUILD_TYPE is ${CMAKE_BUILD_TYPE}")
# set(CMAKE_VERBOSE_MAKEFILE on)

#cmake_policy
# if(POLICY CMP0046)
# cmake_policy(SET CMP0046 OLD)
# endif()

enable_testing()

include(Languages.cmake)
if (NOT WITH_LEGACY_PIPELINES)
  # Only the deeplima-backed UD media are built. The deprecated legacy eng/fre/por
  # resources (morphological dictionaries, PoS n-gram matrices, idioms, syntactic
  # rules, their ModEx rule sets) are skipped. Every per-language loop below
  # (analysisDictionary codes, SpecificEntities, SRLIntegration) follows this list.
  list(REMOVE_ITEM LIMA_LANGUAGES ${LIMA_LEGACY_LANGUAGES})
endif()
message("Linguistic resources will be built for: ${LIMA_LANGUAGES}")
include(cmake/LinguisticData.cmake)
# include(${CMAKE_SOURCE_DIR}/../SystemSpecificInformations.cmake)
# for add_component macro (require 2.8.5)
include(CPackComponent)

configure_file(LIMALDATAConfig-src.cmake LIMALDATAConfig.cmake @ONLY)

foreach(LANG ${LIMA_LANGUAGES})
  LIMA_GENERIC_CONFIGENV(${LANG})
endforeach()

add_custom_target(pretest-execEnv)
add_custom_target(disamb-pretest-execEnv)

add_subdirectory(cmake)
add_subdirectory(scratch)
add_subdirectory(SpecificEntities)            # ModEx named entities
# Property codes of every media (code-ud*.xml are loaded at startup by the UD
# media); the morphological dictionaries are only built for legacy languages.
add_subdirectory(analysisDictionary)
add_subdirectory(SRLIntegration)              # CoNLL tag mapping, VerbNet/FrameNet
if (WITH_LEGACY_PIPELINES)
  add_subdirectory(disambiguisationMatrices)  # n-gram PoS matrices — statistical tagger
  add_subdirectory(rules-idiom)               # rule-based idiomatic expressions
  add_subdirectory(syntacticAnalysis)         # rule-based dependency parser rules

  # PRETEST_CONFIGENV wires the legacy rule-based execEnv (dictionary + idiom
  # rule build targets) used by the eng/fre/por rule-based-pipeline tests.
  foreach(LANG ${LIMA_LEGACY_LANGUAGES})
    LIMA_PRETEST_CONFIGENV(${LANG})
  endforeach()
endif()
########### install files ###############
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/LIMALDATAConfig.cmake DESTINATION share/apps/lima/cmake/Modules
        COMPONENT devel)

