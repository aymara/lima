# SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
#
# SPDX-License-Identifier: MIT

# - Find LimaCommon

set(LIMA_VERSION_MAJOR "3")
set(LIMA_VERSION_MINOR "0")
set(LIMA_VERSION_RELEASE "0+@LIMA_VERSION_RELEASE@")
set(LIMA_VERSION "${LIMA_VERSION_MAJOR}.${LIMA_VERSION_MINOR}.${LIMA_VERSION_RELEASE}")

set(LIMA_GENERIC_LIB_VERSION ${LIMA_VERSION})
set(LIMA_GENERIC_LIB_SOVERSION ${LIMA_VERSION_MAJOR})

set(LIMA_COMMON_LIB_VERSION ${LIMA_GENERIC_LIB_VERSION})
set(LIMA_COMMON_LIB_SOVERSION ${LIMA_GENERIC_LIB_SOVERSION})

if ("x${LIB_INSTALL_DIR}" STREQUAL "x")
  set(LIB_INSTALL_DIR "@LIB_INSTALL_DIR@") # "bin" on Windows
endif ()

# In an installation (possibly relocated, e.g. an extracted package), this
# file is in <prefix>/share/apps/lima/cmake/Modules; in LIMA's build tree, the
# prefix is the configured one.
if (CMAKE_CURRENT_LIST_DIR MATCHES "/share/apps/lima/cmake/Modules$")
  get_filename_component(_LIMA_PREFIX "${CMAKE_CURRENT_LIST_DIR}/../../../../.." ABSOLUTE)
else()
  set(_LIMA_PREFIX "@CMAKE_INSTALL_PREFIX@")
endif()
set(LIMACOMMON_INCLUDE_DIR ${_LIMA_PREFIX}/include)
set(LIMACOMMON_LIBRARY_DIRS ${_LIMA_PREFIX}/${LIB_INSTALL_DIR} )
set(LIMACOMMON_CONFIG_DIR ${_LIMA_PREFIX}/share/config/lima)

message(STATUS "Lima version  ${LIMA_VERSION_MAJOR}.${LIMA_VERSION_MINOR}.${LIMA_VERSION_RELEASE} - LIMA_GENERIC_LIB_VERSION  ${LIMA_GENERIC_LIB_VERSION} ")

