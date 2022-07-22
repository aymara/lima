# Copyright 2002-2020 CEA LIST
# SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
#
# SPDX-License-Identifier: MIT

include_directories(BEFORE ${CMAKE_SOURCE_DIR}/src )

########### next target ###############

SET(lima-lp-conllureader_LIB_SRCS
    ConlluReader.cpp
)

DECLARE_LIMA_PLUGIN(lima-lp-conllureader)

target_compile_definitions(lima-lp-conllureader
    PRIVATE "-DLIMA_CONLLUREADER_EXPORTING"
)

target_link_libraries(lima-lp-conllureader
    lima-lp-linguisticdata
    lima-lp-linguisticresources
    lima-lp-annotationgraph
    lima-lp-linguisticanalysisstructure
    lima-lp-textsegmentation
    lima-common
)

set_target_properties(lima-lp-conllureader PROPERTIES
    VERSION ${LIMA_LP_LIB_VERSION}
    SOVERSION ${LIMA_LP_LIB_SOVERSION}
)

install(
    TARGETS lima-lp-conllureader
    DESTINATION ${LIB_INSTALL_DIR}
    COMPONENT runtime
)

install(
    FILES ConlluReaderExport.h
    DESTINATION include/linguisticProcessing/core/ConlluReader
    COMPONENT devel
)

