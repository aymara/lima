project(Lima)
cmake_minimum_required(VERSION 2.8.3)

enable_testing()

include(ExternalProject)

macro (CompileLimaSubproject _project _dependsOn )

  ## Define test command.
  if (NOT (${CMAKE_SYSTEM_NAME} STREQUAL "Windows"))
    set(_TEST_COMMAND make test)
  else()
    set(_TEST_COMMAND "cmake --build . --target TEST.vcxproj --config Release")
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
    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX:PATH=$ENV{LIMA_DIST} -DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE} -DLIMA_VERSION_RELEASE:STRING="${LIMA_VERSION_RELEASE}" -DBoost_DEBUG=ON
  )

  ## Add specified dependencies
  if (NOT "x${_dependsOn}" STREQUAL "x")
    add_dependencies(${_project} ${_dependsOn})
  endif()

    ## Add packaging command
  if (NOT (${CMAKE_SYSTEM_NAME} STREQUAL "Windows"))
    if ("x${CMAKE_BUILD_TYPE}" STREQUAL "xRelease")
      ExternalProject_Add_Step(${_project} packaging
      COMMAND make package
      COMMENT "Packaging ${_project}"
      WORKING_DIRECTORY <BINARY_DIR>
      DEPENDEES install
      )
    else()
      ExternalProject_Add_Step(${_project} packaging
      COMMAND cmake --build . --target PACKAGE.vcxproj --config Release
      COMMENT "Packaging ${_project}"
      WORKING_DIRECTORY <BINARY_DIR>
      DEPENDEES install
      )
    endif()
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

message("Before BOOST_ROOT=$ENV{BOOST_ROOT}")
message("Before BOOST_LIBRARYDIR=$ENV{BOOST_LIBRARYDIR}")

CompileLimaSubproject(lima_common "")
CompileLimaSubproject(lima_linguisticprocessing lima_common)

if ("x${LIMA_RESOURCES}" STREQUAL "xbuild")
  CompileLimaSubproject(lima_linguisticdata lima_linguisticprocessing)
endif()

CompileLimaSubproject(lima_pelf lima_linguisticprocessing)

CompileLimaSubproject(lima_annoqt lima_common)

install(FILES)

# ########### packaging ###############
# include(SystemSpecificInformations.cmake)
# include(InstallRequiredSystemLibraries)
#
# set(CPACK_MONOLITHIC_INSTALL 1)
# set(CPACK_PACKAGE_NAME "lima")
# set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Lima root package")
# set(CPACK_PACKAGE_VENDOR "CEA LIST")
# set(CPACK_PACKAGE_CONTACT "Gaël de Chalendar <Gael.de-Chalendar@cea.fr>")
# set(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_CURRENT_SOURCE_DIR}/README.md")
# set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/COPYING")
# SET(CPACK_PACKAGE_VERSION_MAJOR "${LIMA_VERSION_MAJOR}")
# SET(CPACK_PACKAGE_VERSION_MINOR "${LIMA_VERSION_MINOR}")
# SET(CPACK_PACKAGE_VERSION_PATCH "${LIMA_VERSION_RELEASE}")
# set(CPACK_PACKAGE_INSTALL_DIRECTORY "CMake ${CMake_VERSION_MAJOR}.${CMake_VERSION_MINOR}")
# set(CPACK_STRIP_FILES "")
# set(CPACK_SOURCE_STRIP_FILES "")
# set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON)
# set(CPACK_GENERATOR ${SPECIFIC_SYSTEM_PREFERED_CPACK_GENERATOR})
# if (CMAKE_SYSTEM_PROCESSOR MATCHES "x86_64")
#   set(CPACK_RPM_PACKAGE_REQUIRES "")
#   set(CPACK_DEBIAN_PACKAGE_DEPENDS "")
# else (CMAKE_SYSTEM_PROCESSOR MATCHES "x86_64")
#   set(CPACK_RPM_PACKAGE_REQUIRES "")
#   set(CPACK_DEBIAN_PACKAGE_DEPENDS "")
# endif(CMAKE_SYSTEM_PROCESSOR MATCHES "x86_64")
#
# include(CPack)
