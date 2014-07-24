project(Lima)
cmake_minimum_required(VERSION 2.8)

enable_testing()

include(ExternalProject)

macro (CompileLimaSubproject _project _dependsOn)

  ExternalProject_Add(
    ${_project}
    DOWNLOAD_COMMAND ""
    PATCH_COMMAND ""
    UPDATE_COMMAND ""
    TEST_COMMAND make test
    SOURCE_DIR ${CMAKE_SOURCE_DIR}/${_project}/
    INSTALL_DIR $ENV{LIMA_DIST}
    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX:PATH=$ENV{LIMA_DIST} -DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE}
  )
  if (NOT "x${_dependsOn}" STREQUAL "x")
    add_dependencies(${_project} ${_dependsOn})
  endif()

  ExternalProject_Add_Step(${_project} packaging
   COMMAND make package
   COMMENT "Packaging ${_project}"
   WORKING_DIRECTORY <BINARY_DIR>
   DEPENDEES install
) 
  
endmacro ()

CompileLimaSubproject(lima_common "")
CompileLimaSubproject(lima_linguisticprocessing lima_common)

if ("x${LIMA_RESOURCES}" STREQUAL "xbuild")
  CompileLimaSubproject(lima_linguisticdata lima_linguisticprocessing)
endif()

CompileLimaSubproject(lima_pelf lima_linguisticprocessing)

CompileLimaSubproject(lima_annoqt "")

install(FILES)

########### packaging ###############
include(SystemSpecificInformations.cmake)
include(InstallRequiredSystemLibraries)

set(CPACK_MONOLITHIC_INSTALL 1)
set(CPACK_PACKAGE_NAME "lima")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Lima root package")
set(CPACK_PACKAGE_VENDOR "CEA LIST")
set(CPACK_PACKAGE_CONTACT "Gaël de Chalendar <Gael.de-Chalendar@cea.fr>")
set(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_CURRENT_SOURCE_DIR}/README.md")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/COPYING")
set(CPACK_PACKAGE_VERSION_MAJOR "1")
set(CPACK_PACKAGE_VERSION_MINOR "0")
set(CPACK_PACKAGE_VERSION_PATCH "0")
set(CPACK_PACKAGE_INSTALL_DIRECTORY "CMake ${CMake_VERSION_MAJOR}.${CMake_VERSION_MINOR}")
set(CPACK_STRIP_FILES "")
set(CPACK_SOURCE_STRIP_FILES "")
set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON)
set(CPACK_GENERATOR ${SPECIFIC_SYSTEM_PREFERED_CPACK_GENERATOR})
if (CMAKE_SYSTEM_PROCESSOR MATCHES "x86_64")
  set(CPACK_RPM_PACKAGE_REQUIRES "")
  set(CPACK_DEBIAN_PACKAGE_DEPENDS "")
else (CMAKE_SYSTEM_PROCESSOR MATCHES "x86_64")
  set(CPACK_RPM_PACKAGE_REQUIRES "")
  set(CPACK_DEBIAN_PACKAGE_DEPENDS "")
endif(CMAKE_SYSTEM_PROCESSOR MATCHES "x86_64")

include(CPack)
