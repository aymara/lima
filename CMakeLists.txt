project(Lima)
cmake_minimum_required(VERSION 2.8.3)

enable_testing()

include(ExternalProject)

include(${CMAKE_SOURCE_DIR}/SetCompilerFlags.cmake)

option(CLEAN_AFTER_BUILD "Clean build dir after installing" OFF)
message("CLEAN_AFTER_BUILD=${CLEAN_AFTER_BUILD}")

macro (CompileLimaSubproject _project _dependsOn)

  ## Define test command.
  if (NOT (${CMAKE_SYSTEM_NAME} STREQUAL "Windows"))
    set(_TEST_COMMAND make test)
  else()
    set(_TEST_COMMAND cmake --build . --target RUN_TESTS --config ${CMAKE_BUILD_TYPE})
  endif()
  
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
    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX:PATH=$ENV{LIMA_DIST} 
        -DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE} 
        -DLIMA_VERSION_RELEASE:STRING="${LIMA_VERSION_RELEASE}" 
        -DBoost_DEBUG=ON 
        -DWITH_ASAN:BOOL=${WITH_ASAN} 
        -DWITH_ARCH:BOOL=${WITH_ARCH} 
        -DSHORTEN_POR_CORPUS_FOR_SVMLEARN:BOOL=${SHORTEN_POR_CORPUS_FOR_SVMLEARN}
        -DCMAKE_CXX_FLAGS:STRING=${CMAKE_CXX_FLAGS}
        -DCMAKE_CXX_FLAGS_DEBUG:STRING=${CMAKE_CXX_FLAGS_DEBUG}
        -DCMAKE_CXX_FLAGS_RELEASE:STRING=${CMAKE_CXX_FLAGS_RELEASE}

  )

  ## Add specified dependencies
  if (NOT "x${_dependsOn}" STREQUAL "x")
    add_dependencies(${_project} ${_dependsOn})
  endif()

    ## Add packaging command
  if ("x${CMAKE_BUILD_TYPE}" STREQUAL "xRelease" AND NOT ${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
      ExternalProject_Add_Step(${_project} packaging
      COMMAND make package
      COMMENT "Packaging ${_project}"
      WORKING_DIRECTORY <BINARY_DIR>
      DEPENDEES install
      )

    ExternalProject_Add_Step(${_project} packageinstall
      COMMAND install -D  ${CMAKE_BINARY_DIR}/*/src/*-build/*.deb ${CMAKE_INSTALL_PREFIX}/share/apps/lima/packages
      COMMENT "Installing package for ${_project}"
      WORKING_DIRECTORY <BINARY_DIR>
      DEPENDEES packaging
    )
  elseif(${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
    ExternalProject_Add_Step(${_project} packaging
    COMMAND cmake --build . --target PACKAGE --config ${CMAKE_BUILD_TYPE}
    COMMENT "Packaging ${_project}"
    WORKING_DIRECTORY <BINARY_DIR>
    DEPENDEES INSTALL
    )
  endif()


  ## Force rebuilding of the main subproject every time building from super structure
  ExternalProject_Add_Step(${_project} forcebuild
    COMMAND ${CMAKE_COMMAND} -E remove
    ${CMAKE_CURRENT_BUILD_DIR}/${_project}-prefix/src/${_project}-stamp/${_project}-build
    DEPENDEES configure
    DEPENDERS build
    ALWAYS 1
  )
  
if (CLEAN_AFTER_BUILD)
  ExternalProject_Add_Step(${_project} distclean
    COMMAND ${CMAKE_COMMAND} -E remove
    ${CMAKE_CURRENT_BUILD_DIR}/${_project}-prefix/src/${_project}-stamp/${_project}-build
    DEPENDEES packageinstall
    ALWAYS 1
    LOG 1
  )
endif()

endmacro ()

# macro (RemoveBuildDir _project)
#   if (CLEAN_AFTER_BUILD)
# 
#   add_custom_target(
#     distclean${_project}
#     COMMAND rm -Rf ${CMAKE_CURRENT_BUILD_DIR}/${_project}-prefix/src/${_project}-stamp/${_project}-build
#     DEPENDS 
#                   [BYPRODUCTS [files...]]
#                   [WORKING_DIRECTORY dir]
#                   [COMMENT comment]
#                   [VERBATIM] [USES_TERMINAL]
#                   [COMMAND_EXPAND_LISTS]
#                   [SOURCES src1 [src2...]])
#     ALL
#     DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/convjys.txt code-${_lang}.xml ${ARGN}
#   )
# 
# 
#   file(REMOVE_RECURSE ${CMAKE_CURRENT_BUILD_DIR}/${_project}-prefix/src/${_project}-stamp/${_project}-build)
#   endif()
# endmacro()

CompileLimaSubproject(lima_common "")
# RemoveBuildDir(lima_common)

CompileLimaSubproject(lima_linguisticprocessing lima_common)
# RemoveBuildDir(lima_linguisticprocessing)

if ("x${LIMA_RESOURCES}" STREQUAL "xbuild")
  CompileLimaSubproject(lima_linguisticdata lima_linguisticprocessing)
#   RemoveBuildDir(lima_linguisticdata)
endif()

CompileLimaSubproject(lima_pelf lima_linguisticprocessing)
# RemoveBuildDir(lima_pelf)

CompileLimaSubproject(lima_annoqt lima_common)
# RemoveBuildDir(lima_annoqt)

CompileLimaSubproject(lima_gui lima_linguisticdata)
# RemoveBuildDir(lima_gui)

