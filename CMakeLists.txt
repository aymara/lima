project(Lima)
cmake_minimum_required(VERSION 2.8.3)

enable_testing()

include(ExternalProject)

include(${CMAKE_SOURCE_DIR}/SetCompilerFlags.cmake)
set(ENV{PATH} "${CMAKE_SOURCE_DIR}/lima_pelf/evalPosTagging/SVM/SVMTool-1.3.1/bin:$ENV{PATH}")
set(ENV{PERL5LIB} "${CMAKE_SOURCE_DIR}/lima_pelf/evalPosTagging/SVM/SVMTool-1.3.1/lib:$ENV{PERL5LIB}")
message("PERL5LIB=$ENV{PERL5LIB}")
message("PATH=$ENV{PATH}")
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
        -DBOOST_ROOT:STRING=${BOOST_ROOT}
        -DBOOST_LIBRARYDIR:STRING=${BOOST_LIBRARYDIR}

  )

  ## Add specified dependencies
  if (NOT "x${_dependsOn}" STREQUAL "x")
    add_dependencies(${_project} ${_dependsOn})
  endif()

    ## Add packaging command
#   if ("x${CMAKE_BUILD_TYPE}" STREQUAL "xRelease" AND NOT ${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
  if (NOT ${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
      ExternalProject_Add_Step(${_project} packaging
      COMMAND make package
      COMMENT "Packaging ${_project}"
      WORKING_DIRECTORY <BINARY_DIR>
      DEPENDEES install
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
  
endmacro ()

CompileLimaSubproject(lima_common "")

CompileLimaSubproject(lima_linguisticprocessing lima_common)

if ("x${LIMA_RESOURCES}" STREQUAL "xbuild")
  CompileLimaSubproject(lima_linguisticdata lima_linguisticprocessing)
endif()

#CompileLimaSubproject(lima_pelf lima_linguisticprocessing)

CompileLimaSubproject(lima_annoqt lima_common)

add_definitions(-D QT_NO_KEYWORDS)
include(${CMAKE_SOURCE_DIR}/manageQt5.cmake)
addQt5Modules(Core)
if (Qt5Core_FOUND)
  if (Qt5Core_VERSION VERSION_GREATER 5.5.0)
    CompileLimaSubproject(lima_gui lima_linguisticdata)
  else()
    message("Minimum supported Qt5 version for lima_gui is 5.5.0. You have Qt5 ${Qt5Core_VERSION}. lima_gui will no be built.")
  endif()
endif()

