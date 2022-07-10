# SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
#
# SPDX-License-Identifier: MIT

# - Find LimaLP

if ("x${LIB_INSTALL_DIR}" STREQUAL "x")
  set(LIB_INSTALL_DIR "lib")
endif ()

message(STATUS "Lima version  ${LIMA_VERSION_MAJOR}.${LIMA_VERSION_MINOR}.${LIMA_VERSION_RELEASE} - LIMA_GENERIC_LIB_VERSION  ${LIMA_GENERIC_LIB_VERSION} ")
