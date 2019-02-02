#   Copyright 2002-2019 CEA LIST
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
    compile-rules.cpp)

add_executable(compile-rules ${compile-rules_SRCS})

target_link_libraries(compile-rules lima-lp-automatoncompiler)

install(TARGETS compile-rules
        DESTINATION bin
        COMPONENT runtime)


########### install files ###############
