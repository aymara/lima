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
project(LimaLinguisticData NONE)
cmake_minimum_required(VERSION 3.4.0)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

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
if(POLICY CMP0046)
cmake_policy(SET CMP0046 OLD)
endif()

enable_testing()


include(Languages.cmake)
include(cmake/LinguisticData.cmake)
# include(${CMAKE_SOURCE_DIR}/../SystemSpecificInformations.cmake)
# for add_component macro (require 2.8.5)
include(CPackComponent)

FIND_PACKAGE(LIMACOMMON PATHS $ENV{LIMA_DIST}/share/apps/lima/cmake/Modules ${CMAKE_INSTALL_PREFIX}/share/apps/lima/cmake/Modules /usr/share/apps/lima/cmake/Modules share/apps/lima/cmake/Modules)
IF(LIMACOMMON_FOUND)
  include_directories(${LIMACOMMON_INCLUDE_DIR})
  link_directories(${LIMACOMMON_LIBRARY_DIRS})
ELSE (LIMACOMMON_FOUND)
  MESSAGE(FATAL_ERROR "Could not find limacommon")
ENDIF(LIMACOMMON_FOUND)

FIND_PACKAGE(LIMALP PATHS $ENV{LIMA_DIST}/share/apps/lima/cmake/Modules ${CMAKE_INSTALL_PREFIX}/share/apps/lima/cmake/Modules /usr/share/apps/lima/cmake/Modules share/apps/lima/cmake/Modules)
IF(LIMALP_FOUND)
  include_directories(${LIMALP_INCLUDE_DIR})
  link_directories(${LIMALP_LIBRARY_DIRS})
ELSE (LIMALP_FOUND)
  MESSAGE(FATAL_ERROR "Could not find limalp")
ENDIF(LIMALP_FOUND)

configure_file(LIMALDATAConfig-src.cmake LIMALDATAConfig.cmake @ONLY)


LIMA_GENERIC_CONFIGENV(eng)
LIMA_GENERIC_CONFIGENV(fre)
LIMA_GENERIC_CONFIGENV(por)
SPECIFICENTITIES_GENERIC_CONFIGENV()

add_subdirectory(cmake)
add_subdirectory(scratch)
add_subdirectory(analysisDictionary)
add_subdirectory(disambiguisationMatrices)
add_subdirectory(rules-idiom)
add_subdirectory(SpecificEntities)
add_subdirectory(syntacticAnalysis)
add_subdirectory(SRLIntegration)


########### install files ###############
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/LIMALDATAConfig.cmake DESTINATION share/apps/lima/cmake/Modules COMPONENT devel)


