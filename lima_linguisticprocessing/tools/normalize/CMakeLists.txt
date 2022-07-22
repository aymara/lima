# Copyright 2002-2019 CEA LIST
# SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
#
# SPDX-License-Identifier: MIT

include_directories(BEFORE ${CMAKE_SOURCE_DIR}/src )

########### next target ###############

SET(normalizeTerm_SRCS
   normalizeTerm.cpp
)

add_executable(normalizeTerm ${normalizeTerm_SRCS})

target_link_libraries(normalizeTerm
  lima-lp-analysishandlers
  lima-lp-client
  lima-lp-bagofwords
)

install(TARGETS normalizeTerm DESTINATION bin
  COMPONENT runtime)


########### next target ###############

SET(desaccent_SRCS
   desaccent.cpp
)

add_executable(desaccent ${desaccent_SRCS})

target_link_libraries(desaccent
  lima-lp-linguisticresources
  lima-lp-flattokenizer
  lima-lp-client
  Boost::program_options
)

install(TARGETS desaccent DESTINATION bin
  COMPONENT runtime)


########### install files ###############
