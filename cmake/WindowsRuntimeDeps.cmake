# SPDX-FileCopyrightText: 2026 CEA LIST <gael.de-chalendar@cea.fr>
#
# SPDX-License-Identifier: MIT

# Windows: install the third-party DLLs needed by LIMA next to its binaries, so
# that the install tree (and the CPack packages made from it) is self-contained.
# Must be included at the end of the top-level CMakeLists.txt: with policy
# CMP0082, install rules run in declaration order, so this runs last.

if (NOT WIN32)
  return()
endif()

set(LIMA_RUNTIME_DLL_DIRS "" CACHE STRING
  "Directories searched for third-party DLLs at install time (default: derived from the packages found)")

if (NOT LIMA_RUNTIME_DLL_DIRS)
  set(_dirs "")
  # Qt
  if (TARGET Qt${QT_VERSION_MAJOR}::Core)
    get_target_property(_qtcore Qt${QT_VERSION_MAJOR}::Core LOCATION)
    get_filename_component(_qtbin "${_qtcore}" DIRECTORY)
    list(APPEND _dirs "${_qtbin}")
  endif()
  # Boost, ICU and their dependencies (e.g. vcpkg's installed/<triplet>/bin)
  foreach(_inc IN ITEMS "${Boost_INCLUDE_DIR}" "${ICU_INCLUDE_DIR}")
    if (_inc AND EXISTS "${_inc}/../bin")
      get_filename_component(_bin "${_inc}/../bin" ABSOLUTE)
      list(APPEND _dirs "${_bin}")
    endif()
  endforeach()
  # libtorch
  if (TORCH_INSTALL_PREFIX)
    list(APPEND _dirs "${TORCH_INSTALL_PREFIX}/lib")
  endif()
  list(REMOVE_DUPLICATES _dirs)
  set(LIMA_RUNTIME_DLL_DIRS "${_dirs}")
endif()
message("Windows runtime DLLs will be searched in: ${LIMA_RUNTIME_DLL_DIRS}")

# Quote each directory for the configured script
set(_quoted "")
foreach(_d IN LISTS LIMA_RUNTIME_DLL_DIRS)
  string(APPEND _quoted " \"${_d}\"")
endforeach()
set(LIMA_RUNTIME_DLL_DIRS "${_quoted}")

configure_file(${CMAKE_CURRENT_LIST_DIR}/WindowsRuntimeDeps.cmake.in
  ${CMAKE_BINARY_DIR}/WindowsRuntimeDeps-install.cmake @ONLY)
install(SCRIPT ${CMAKE_BINARY_DIR}/WindowsRuntimeDeps-install.cmake
  COMPONENT runtime)
