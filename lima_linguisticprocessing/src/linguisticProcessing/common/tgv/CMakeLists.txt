# Copyright 2002-2019 CEA LIST
# SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
#
# SPDX-License-Identifier: MIT

include_directories(BEFORE ${CMAKE_SOURCE_DIR}/src )

add_definitions(-DLIMA_TGV_EXPORTING)

########### next target ###############

SET(lima-lp-tgv_LIB_SRCS
   TestCaseError.cpp
   TestCase.cpp
   TestCaseProcessor.cpp
   TestCasesHandler.cpp
)

add_library(lima-lp-tgv SHARED ${lima-lp-tgv_LIB_SRCS})
ADD_DLL_TO_PATH(lima-lp-tgv)


target_link_libraries(lima-lp-tgv
  lima-lp-misc
  Qt5::XmlPatterns
)

set_target_properties(lima-lp-tgv PROPERTIES VERSION ${LIMA_LP_LIB_VERSION} SOVERSION ${LIMA_LP_LIB_SOVERSION})

install(TARGETS lima-lp-tgv DESTINATION ${LIB_INSTALL_DIR}
  COMPONENT runtime)


########### install files ###############

install(FILES
    TestCase.h
    TestCaseProcessor.hpp
    TestCaseError.hpp
    TestCasesHandler.h
  DESTINATION include/linguisticProcessing/common/tgv
  COMPONENT devel
)
