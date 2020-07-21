#   Copyright 2002-2020 CEA LIST
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

