project(Lima)
cmake_minimum_required(VERSION 3.4.0)

enable_testing()

include(ExternalProject)

option(WITH_ASAN "Enable address sanitizer" OFF)
message("WITH_ASAN=${WITH_ASAN}")
option(WITH_ARCH "Enable architecture optimizations" OFF)
message("WITH_ARCH=${WITH_ARCH}")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pipe")

if (NOT ("x${CMAKE_SYSTEM_NAME}" STREQUAL "xWindows"))
    if (${CMAKE_BUILD_TYPE} STREQUAL "Release")
      if (WITH_ARCH)
        set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -march=native -O2")
      else()
        set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -mtune=generic -march=x86-64")
      endif()
    endif()
endif()

macro (CompileLimaSubproject _project _dependsOn )

  ## Define test command.
#   if (NOT (${CMAKE_SYSTEM_NAME} STREQUAL "Windows"))
#     set(_TEST_COMMAND make test)
#   else()
    set(_TEST_COMMAND cmake --build . --target test --config ${CMAKE_BUILD_TYPE})
#   endif()
  
  ## Add external project
  ExternalProject_Add(
    ${_project}
    PREFIX "${_project}"
    DOWNLOAD_COMMAND ""
    PATCH_COMMAND ""
    UPDATE_COMMAND ""
    TEST_COMMAND "${_TEST_COMMAND}"
    SOURCE_DIR ${CMAKE_SOURCE_DIR}/${_project}/
    INSTALL_DIR $ENV{LIMA_DIST}
    USES_TERMINAL_DOWNLOAD 1
    USES_TERMINAL_UPDATE 1
    USES_TERMINAL_CONFIGURE 1
    USES_TERMINAL_BUILD 1
    USES_TERMINAL_TEST 1
    USES_TERMINAL_INSTALL 1
    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX:PATH=$ENV{LIMA_DIST} 
        -DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE} 
        -DLIMA_VERSION_RELEASE:STRING="${LIMA_VERSION_RELEASE}" 
        -DBoost_DEBUG=ON -DWITH_ASAN:BOOL=${WITH_ASAN} 
        -DCMAKE_CXX_FLAGS_RELEASE:STRING=${CMAKE_CXX_FLAGS_RELEASE}

  )

  ## Add specified dependencies
  if (NOT "x${_dependsOn}" STREQUAL "x")
    add_dependencies(${_project} ${_dependsOn})
  endif()

    ## Add packaging command
#   if ("x${CMAKE_BUILD_TYPE}" STREQUAL "xRelease" AND NOT ${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
#   if (NOT ${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
#       ExternalProject_Add_Step(${_project} packaging
#       COMMAND make package
#       COMMENT "Packaging ${_project}"
#       WORKING_DIRECTORY <BINARY_DIR>
#       DEPENDEES install
#       )
# 
#   elseif(${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
    ExternalProject_Add_Step(${_project} packaging
    COMMAND cmake --build . --target package --config ${CMAKE_BUILD_TYPE}
    COMMENT "Packaging ${_project}"
    WORKING_DIRECTORY <BINARY_DIR>
    DEPENDEES install
    USES_TERMINAL 1
    )
#   endif()

  ## Force rebuilding of the main subproject every time building from super structure
  ExternalProject_Add_Step(${_project} forcebuild
    COMMAND ${CMAKE_COMMAND} -E remove
    ${CMAKE_CURRENT_BUILD_DIR}/${_project}-prefix/src/${_project}-stamp/${_project}-build
    DEPENDEES configure
    DEPENDERS build
    ALWAYS 1
    USES_TERMINAL 1
  )
  
endmacro ()

CompileLimaSubproject(lima_common "")
CompileLimaSubproject(lima_linguisticprocessing lima_common)

if ("x${LIMA_RESOURCES}" STREQUAL "xbuild")
  CompileLimaSubproject(lima_linguisticdata lima_linguisticprocessing)
endif()

# FIXME: Qt5 supported only (Qt4 not supported)
find_package(Qt5 COMPONENTS Core)
if (Qt5_FOUND)
  CompileLimaSubproject(lima_pelf lima_linguisticprocessing)
  CompileLimaSubproject(lima_annoqt lima_common)
endif (Qt5_FOUND)

install(FILES)

INCLUDE(CPack)
