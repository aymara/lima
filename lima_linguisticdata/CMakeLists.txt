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
if (LIMA_RESOURCES_DEEPLIMA_ONLY)
  # deeplima+ModEx pipelines only use the UD languages. Restrict every
  # per-language resource build (dictionaries, idioms, disambiguation matrices,
  # syntactic rules, SpecificEntities) to the UD languages. The legacy
  # eng/fre/por rule-based/statistical resources are not built.
  set(LIMA_LANGUAGES ud ud-eng)
  message("LIMA_RESOURCES_DEEPLIMA_ONLY: restricting linguistic resources to languages: ${LIMA_LANGUAGES}")
endif()
include(cmake/LinguisticData.cmake)
# include(${CMAKE_SOURCE_DIR}/../SystemSpecificInformations.cmake)
# for add_component macro (require 2.8.5)
include(CPackComponent)

configure_file(LIMALDATAConfig-src.cmake LIMALDATAConfig.cmake @ONLY)

LIMA_GENERIC_CONFIGENV(ud)
LIMA_GENERIC_CONFIGENV(ud-eng)
if (NOT LIMA_RESOURCES_DEEPLIMA_ONLY)
  LIMA_GENERIC_CONFIGENV(eng)
  LIMA_GENERIC_CONFIGENV(fre)
  LIMA_GENERIC_CONFIGENV(por)
endif()

add_custom_target(pretest-execEnv)
add_custom_target(disamb-pretest-execEnv)

add_subdirectory(cmake)
add_subdirectory(scratch)
add_subdirectory(SpecificEntities)            # ModEx / NER — always (deeplima-distinctive)
if (LIMA_RESOURCES_DEEPLIMA_ONLY)
  message("LIMA_RESOURCES_DEEPLIMA_ONLY: keeping SpecificEntities (ModEx/NER); skipping "
          "analysisDictionary, disambiguisationMatrices, rules-idiom, syntacticAnalysis, "
          "SRLIntegration.")
else()
  add_subdirectory(analysisDictionary)        # morpho dictionary — rule-based morphology
  add_subdirectory(disambiguisationMatrices)  # n-gram PoS matrices — statistical tagger
  add_subdirectory(rules-idiom)               # rule-based idiomatic expressions
  add_subdirectory(syntacticAnalysis)         # rule-based dependency parser rules
  add_subdirectory(SRLIntegration)            # semantic-role-labeling integration
endif()

# PRETEST_CONFIGENV wires the legacy rule-based execEnv (dictionary + idiom rule
# build targets) used by the eng/fre/por rule-based-pipeline tests. Not needed —
# and not buildable — under the deeplima-only resource profile.
if (NOT LIMA_RESOURCES_DEEPLIMA_ONLY)
  LIMA_PRETEST_CONFIGENV(eng)
  LIMA_PRETEST_CONFIGENV(fre)
  LIMA_PRETEST_CONFIGENV(por)
endif()
########### install files ###############
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/LIMALDATAConfig.cmake DESTINATION share/apps/lima/cmake/Modules
        COMPONENT devel)

