# Copyright 2002-2020 CEA LIST
# SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
#
# SPDX-License-Identifier: MIT

add_subdirectory(libautomatonCompiler)

include_directories(BEFORE ${CMAKE_SOURCE_DIR}/src )

# IF(NOT SVMTOOL_LIBRARIES STREQUAL "SVMTOOL_LIBRARIES-NOTFOUND")
# SET(optionalLibs ${optionalLibs} svmtool)
# ENDIF(NOT SVMTOOL_LIBRARIES STREQUAL "SVMTOOL_LIBRARIES-NOTFOUND")


########### next target ###############

SET(compile-rules_SRCS
    compile-rules.cpp)

add_executable(compile-rules ${compile-rules_SRCS})

target_link_libraries(compile-rules
    lima-lp-automatoncompiler
    lima-lp-specificentities
)

install(TARGETS compile-rules
        DESTINATION bin
        COMPONENT runtime)


########### install files ###############
