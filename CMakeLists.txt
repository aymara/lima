project(Lima)
cmake_minimum_required(VERSION 3.4.0)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

enable_testing()

include(ExternalProject)

set(ENV{CTEST_OUTPUT_ON_FAILURE} "1")
include(${CMAKE_SOURCE_DIR}/SetCompilerFlags.cmake)
set(ENV{PATH} "${CMAKE_SOURCE_DIR}/lima_pelf/evalPosTagging/SVM/SVMTool-1.3.1/bin:$ENV{PATH}")
set(ENV{PERL5LIB} "${CMAKE_SOURCE_DIR}/lima_pelf/evalPosTagging/SVM/SVMTool-1.3.1/lib:$ENV{PERL5LIB}")
message("PERL5LIB=$ENV{PERL5LIB}")
message("PATH=$ENV{PATH}")

set(DEBUG_FLAGS "-DDEBUG_LP -DDEBUG_CD -DDEBUG_FACTORIES -DDEBUG_LIMA_GUI")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} ${DEBUG_FLAGS}" )
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} ${DEBUG_FLAGS}" )
if (WITH_DEBUG_MESSAGES)
  set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} ${DEBUG_FLAGS}" )
endif()

if(UNIX)
  # Try to find architecture
  execute_process(COMMAND uname -m OUTPUT_VARIABLE CPACK_PACKAGE_ARCHITECTURE)
  string(STRIP "${CPACK_PACKAGE_ARCHITECTURE}" CPACK_PACKAGE_ARCHITECTURE)
  # Try to find distro name and distro-specific arch
  execute_process(COMMAND lsb_release -is OUTPUT_VARIABLE LSB_ID)
  execute_process(COMMAND lsb_release -rs OUTPUT_VARIABLE LSB_RELEASE)
  string(STRIP "${LSB_ID}" LSB_ID)
  string(STRIP "${LSB_RELEASE}" LSB_RELEASE)
  set(LSB_DISTRIB "${LSB_ID}${LSB_RELEASE}")
  if(NOT LSB_DISTRIB)
    set(LSB_DISTRIB "unix")
  endif(NOT LSB_DISTRIB)
  set(CPACK_SYSTEM_NAME "${LSB_DISTRIB}-${CPACK_PACKAGE_ARCHITECTURE}")
endif()

if(${CMAKE_HOST_SYSTEM_NAME} MATCHES Windows)
  if(CMAKE_CL_64)
    set(CPACK_SYSTEM_NAME win64)
  else(CMAKE_CL_64)
    set(CPACK_SYSTEM_NAME win32)
  endif(CMAKE_CL_64)
endif()

message("CPACK_SYSTEM_NAME is ${CPACK_SYSTEM_NAME}")

# Allow to set LIMA_VERSION variables necessary to setup packages names
configure_file(lima_common/LIMACOMMONConfig-src.cmake LIMACOMMONConfig.cmake @ONLY)
find_package(LIMACOMMON PATHS ${CMAKE_CURRENT_BINARY_DIR} REQUIRED)

set(CPACK_PACKAGE_VENDOR "CEA LIST")
set(CPACK_PACKAGE_CONTACT "Gaël de Chalendar <Gael.de-Chalendar@cea.fr>")
SET(CPACK_PACKAGE_VERSION_MAJOR "${LIMA_VERSION_MAJOR}")
SET(CPACK_PACKAGE_VERSION_MINOR "${LIMA_VERSION_MINOR}")
SET(CPACK_PACKAGE_VERSION_PATCH "${LIMA_VERSION_RELEASE}")
SET(CPACK_PACKAGE_VERSION "${LIMA_VERSION_MAJOR}.${LIMA_VERSION_MINOR}.${LIMA_VERSION_RELEASE}")
SET(CPACK_PACKAGE_INSTALL_DIRECTORY "Lima-${LIMA_VERSION_MAJOR}.${LIMA_VERSION_MINOR}")

macro (CompileLimaSubproject _project _dependsOn)

  ## Add external project
  ExternalProject_Add(
    ${_project}
    PREFIX "${_project}"
    DOWNLOAD_COMMAND ""
    PATCH_COMMAND ""
    UPDATE_COMMAND ""
    SOURCE_DIR ${CMAKE_SOURCE_DIR}/${_project}/
    INSTALL_DIR $ENV{LIMA_DIST}
    USES_TERMINAL_DOWNLOAD 1
    USES_TERMINAL_UPDATE 1
    USES_TERMINAL_CONFIGURE 1
    USES_TERMINAL_BUILD 1
    USES_TERMINAL_TEST 1
    USES_TERMINAL_INSTALL 1
    TEST_AFTER_INSTALL 1
    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX:PATH=$ENV{LIMA_DIST}
        -DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE}
        -DLIMA_VERSION_RELEASE:STRING="${LIMA_VERSION_RELEASE}"
        -DBoost_DEBUG=ON
        -DWITH_ASAN:BOOL=${WITH_ASAN}
        -DWITH_ARCH:BOOL=${WITH_ARCH}
        -DSHORTEN_POR_CORPUS_FOR_SVMLEARN:BOOL=${SHORTEN_POR_CORPUS_FOR_SVMLEARN}
        -DCMAKE_IGNORE_PATH=${CMAKE_IGNORE_PATH}
        -DCMAKE_CXX_FLAGS:STRING=${CMAKE_CXX_FLAGS}
        -DCMAKE_CXX_FLAGS_DEBUG:STRING=${CMAKE_CXX_FLAGS_DEBUG}
        -DCMAKE_CXX_FLAGS_RELEASE:STRING=${CMAKE_CXX_FLAGS_RELEASE}
        -DCMAKE_CXX_FLAGS_RELWITHDEBINFO:STRING=${CMAKE_CXX_FLAGS_RELWITHDEBINFO}
        -DCPACK_SYSTEM_NAME:STRING=${CPACK_SYSTEM_NAME}
        -DCPACK_PACKAGE_ARCHITECTURE=${CPACK_PACKAGE_ARCHITECTURE}
        -DCPACK_PACKAGE_VENDOR=${CPACK_PACKAGE_VENDOR}
        -DCPACK_PACKAGE_CONTACT=${CPACK_PACKAGE_CONTACT}
        -DCPACK_PACKAGE_VERSION_MAJOR=${CPACK_PACKAGE_VERSION_MAJOR}
        -DCPACK_PACKAGE_VERSION_MINOR=${CPACK_PACKAGE_VERSION_MINOR}
        -DCPACK_PACKAGE_VERSION_PATCH=${CPACK_PACKAGE_VERSION_PATCH}
        -DCPACK_PACKAGE_VERSION=${CPACK_PACKAGE_VERSION}
        -DCPACK_PACKAGE_INSTALL_DIRECTORY=${CPACK_PACKAGE_INSTALL_DIRECTORY}
        -DBOOST_ROOT:STRING=${BOOST_ROOT}
        -DBOOST_LIBRARYDIR:STRING=${BOOST_LIBRARYDIR}
        -DENVPATH:STRING=$ENV{PATH}
        -DENVPERL5LIB:STRING=$ENV{PERL5LIB}
        -DTF_SOURCES_PATH=${TF_SOURCES_PATH}
  )

  ## Add specified dependencies
  if (NOT "x${_dependsOn}" STREQUAL "x")
    add_dependencies(${_project} ${_dependsOn})
  endif()

  ## Add packaging step
  ExternalProject_Add_Step(${_project} packaging
    COMMAND cmake --build . --target package --config ${CMAKE_BUILD_TYPE}
    COMMENT "Packaging ${_project}"
    WORKING_DIRECTORY <BINARY_DIR>
    DEPENDEES install
    USES_TERMINAL 1
  )


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

# if ("x${LIMA_RESOURCES}" STREQUAL "xbuild")
  CompileLimaSubproject(lima_linguisticdata lima_linguisticprocessing)
# endif()

CompileLimaSubproject(lima_pelf lima_linguisticprocessing)

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

INCLUDE(CPack)
