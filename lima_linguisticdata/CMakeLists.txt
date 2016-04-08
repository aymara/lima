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

cmake_minimum_required(VERSION 2.8.3)


#cmake_policy
if(POLICY CMP0046)
cmake_policy(SET CMP0046 OLD)
endif()

enable_testing()


include(Languages.cmake)
include(cmake/LinguisticData.cmake)
include(SystemSpecificInformations.cmake)
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
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/LIMALDATAConfig.cmake DESTINATION share/apps/lima/cmake/Modules)


########### packaging ###############
INCLUDE(InstallRequiredSystemLibraries)

# set to ignore component and create only one package (limadata.1.0.0-Linux.rpm for example)
# comment to take into account component declaration and create one package
# for each language (limadata-common.1.0.0-Linux.rpm, limadata-fre.1.0.0-Linux.rpm and
# limadata-eng.1.0.0-Linux.rpm for example)
#SET(CPACK_MONOLITHIC_INSTALL 1)
SET(CPACK_PACKAGE_NAME "limalinguisticdata")
SET(CPACK_PACKAGE_INSTALL_REGISTRY_KEY  "limalinguisticdata-fre 1.0.0")
SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY "LIMA linguistic processing resources")
SET(CPACK_PACKAGE_VENDOR "CEA LIST")
SET(CPACK_PACKAGE_CONTACT "Gael de Chalendar <gael.de-chalendar AT cea.fr>")
SET(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_CURRENT_SOURCE_DIR}/README")
SET(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/COPYING")
SET(CPACK_PACKAGE_VERSION_MAJOR "${LIMA_VERSION_MAJOR}")
SET(CPACK_PACKAGE_VERSION_MINOR "${LIMA_VERSION_MINOR}")
SET(CPACK_PACKAGE_VERSION_PATCH "${LIMA_VERSION_RELEASE}")
SET(CPACK_PROJECT_CONFIG_FILE ${${CMAKE_SOURCE_DIR}}/cmake/CPackOptions.cmake)
SET(CPACK_NSIS_INSTALL_ROOT "$PROGRAMFILES64")
SET(CPACK_PACKAGE_INSTALL_DIRECTORY "Lima-${LIMA_VERSION_MAJOR}.${LIMA_VERSION_MINOR}")
SET(CPACK_NSIS_ENABLE_UNINSTALL_BEFORE_INSTALL "ON")
SET(CPACK_NSIS_MODIFY_PATH "ON")
SET(CPACK_STRIP_FILES "")
SET(CPACK_SOURCE_STRIP_FILES ${CPACK_STRIP_FILES})
SET(CPACK_GENERATOR ${SPECIFIC_SYSTEM_PREFERED_CPACK_GENERATOR})
# SET(CPACK_PACKAGE_FILE_NAME "limadata")

# set of commands to define component based packaging
# The idea is to define one component with files common to all languages (common)
# and one component for files specific to any languages (fre, eng...)
# notify cpack of component all names
SET(CPACK_COMPONENTS_ALL common fre eng)
# we do not define groups
# SET(CPACK_COMPONENTS_IGNORE_GROUPS 1)
# by default create one package for each component
# SET(CPACK_COMPONENTS_ALL_IN_ONE_PACKAGE 1)

SET(CPACK_RPM_PACKAGE_REQUIRES "limacommon, limalinguisticprocessing")
# by default behaviour of rpm packager is to disable components
SET(CPACK_RPM_COMPONENT_INSTALL ON)
# mangle CPACK_PACKAGE_FILE_NAME and component name
SET(CPACK_RPM_USE_DISPLAY_NAME_IN_FILENAME ON)

SET(CPACK_DEBIAN_PACKAGE_DEPENDS "limacommon, limalinguisticprocessing")
SET(CPACK_DEBIAN_COMPONENT_INSTALL ON)
SET(CPACK_DEBIAN_USE_DISPLAY_NAME_IN_FILENAME ON)


INCLUDE(CPack)
