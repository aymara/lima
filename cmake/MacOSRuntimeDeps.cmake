# SPDX-FileCopyrightText: 2026 CEA LIST <gael.de-chalendar@cea.fr>
#
# SPDX-License-Identifier: MIT

# macOS: make the installation self-contained and relocatable (third-party
# libraries bundled in lib/, @rpath references; see MacOSRuntimeDeps.cmake.in).
# Must be included at the end of the top-level CMakeLists.txt: with policy
# CMP0082, install rules run in declaration order, so this runs last.

if (NOT APPLE)
  return()
endif()

option(LIMA_BUNDLE_RUNTIME_DEPS
  "Bundle the third-party libraries in the installation (relocatable install)" ON)
if (NOT LIMA_BUNDLE_RUNTIME_DEPS)
  return()
endif()

# Where the dependencies are searched (in addition to the binaries' rpaths)
set(_dirs "")
if (TARGET Qt${QT_VERSION_MAJOR}::Core)
  get_target_property(_qtcore Qt${QT_VERSION_MAJOR}::Core LOCATION)
  string(REGEX REPLACE "/[^/]+\\.framework/.*$" "" _qtlib "${_qtcore}")
  list(APPEND _dirs "${_qtlib}")
endif()
if (TORCH_INSTALL_PREFIX)
  list(APPEND _dirs "${TORCH_INSTALL_PREFIX}/lib")
endif()
foreach(_inc IN ITEMS "${Boost_INCLUDE_DIR}" "${ICU_INCLUDE_DIR}")
  if (_inc AND EXISTS "${_inc}/../lib")
    get_filename_component(_lib "${_inc}/../lib" ABSOLUTE)
    list(APPEND _dirs "${_lib}")
  endif()
endforeach()
list(REMOVE_DUPLICATES _dirs)
message("macOS runtime libraries will be searched in: ${_dirs}")

set(LIMA_RUNTIME_LIB_DIRS "")
foreach(_d IN LISTS _dirs)
  string(APPEND LIMA_RUNTIME_LIB_DIRS " \"${_d}\"")
endforeach()

configure_file(${CMAKE_CURRENT_LIST_DIR}/MacOSRuntimeDeps.cmake.in
  ${CMAKE_BINARY_DIR}/MacOSRuntimeDeps-install.cmake @ONLY)
install(SCRIPT ${CMAKE_BINARY_DIR}/MacOSRuntimeDeps-install.cmake
  COMPONENT runtime)
