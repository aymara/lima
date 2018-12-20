project(Lima)
cmake_minimum_required(VERSION 3.4.0)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

enable_testing()
include(${CMAKE_SOURCE_DIR}/SystemSpecificInformations.cmake)

include(ExternalProject)

set(ENV{CTEST_OUTPUT_ON_FAILURE} "1")
include(${CMAKE_SOURCE_DIR}/SetCompilerFlags.cmake)
set(ENV{PATH} "${CMAKE_SOURCE_DIR}/lima_pelf/evalPosTagging/SVM/SVMTool-1.3.1/bin:$ENV{PATH}")
set(ENV{PERL5LIB} "${CMAKE_SOURCE_DIR}/lima_pelf/evalPosTagging/SVM/SVMTool-1.3.1/lib:$ENV{PERL5LIB}")
message("PERL5LIB=$ENV{PERL5LIB}")
message("PATH=$ENV{PATH}")

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

add_definitions(-D QT_NO_KEYWORDS)
include(${CMAKE_SOURCE_DIR}/manageQt5.cmake)
addQt5Modules(Core Xml Test Qml Quick QuickTest Gui Widgets XmlPatterns Network)

include(${CMAKE_SOURCE_DIR}/lima_linguisticdata/cmake/LinguisticData.cmake)

SPECIFICENTITIES_GENERIC_CONFIGENV()

macro (ADD_DLL_TO_PATH _lib)
    message("ADD_DLL_TO_PATH ${_lib}")
    message("Will copy $<TARGET_FILE_DIR:tgt>/${_lib}.dll to ${CMAKE_BINARY_DIR}/execEnv/lib/${_lib}.dll")
    if(WIN32)
      add_custom_command(
        OUTPUT ${CMAKE_BINARY_DIR}/execEnv/lib/${_lib}.dll
        COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/execEnv/lib
        COMMAND ${CMAKE_COMMAND} -E copy 
          $<TARGET_FILE_DIR:tgt>/${_lib}.dll
          ${CMAKE_BINARY_DIR}/execEnv/lib/${_lib}.dll
        DEPENDS
          ${_lib}
        COMMENT "Copy $<TARGET_FILE_DIR:tgt>/${_lib}.dll to ${CMAKE_BINARY_DIR}/execEnv/lib/${_lib}.dll"
        VERBATIM
      )
      add_custom_target(
        ${_lib}_dll
        ALL
        DEPENDS ${CMAKE_BINARY_DIR}/execEnv/lib/${_lib}.dll
      )
      add_dependencies(rules-configEnv ${_lib}_dll)

      set(ENV{PATH} "$ENV{PATH};${CMAKE_BINARY_DIR}/execEnv/lib")

    endif()
endmacro()

include_directories(
  ${CMAKE_BINARY_DIR}/lima_common 
  ${CMAKE_BINARY_DIR}/lima_common/src
  lima_common 
  lima_common/src
)
add_subdirectory(lima_common)

include_directories(
  {CMAKE_BINARY_DIR}/lima_linguisticprocessing
  {CMAKE_BINARY_DIR}/lima_linguisticprocessing/src
  {CMAKE_BINARY_DIR}/lima_linguisticprocessing/src/linguisticProcessing
  lima_linguisticprocessing
  lima_linguisticprocessing/src
  lima_linguisticprocessing/src/linguisticProcessing
)
add_subdirectory(lima_linguisticprocessing)

# if ("x${LIMA_RESOURCES}" STREQUAL "xbuild")
  add_subdirectory(lima_linguisticdata)
# endif()

add_subdirectory(lima_pelf)

# add_subdirectory(lima_annoqt)

if (Qt5Core_FOUND)
  if (Qt5Core_VERSION VERSION_GREATER 5.5.0)
    add_subdirectory(lima_gui)
  else()
    message("Minimum supported Qt5 version for lima_gui is 5.5.0. You have Qt5 ${Qt5Core_VERSION}. lima_gui will no be built.")
  endif()
endif()



########### documentation ###############

# add a target to generate API documentation with Doxygen
find_package(Doxygen)
if(DOXYGEN_FOUND)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile @ONLY)
add_custom_target(doc
${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
COMMENT "Generating API documentation with Doxygen" VERBATIM
)
endif(DOXYGEN_FOUND)


########### packaging ###############

message("CPACK_INSTALL_CMAKE_PROJECTS=${CPACK_INSTALL_CMAKE_PROJECTS}")

# Specify additional runtime libraries that may not be detected. After 
# inclusion any detected libraries will be appended to this.
set(CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS 
  ${QtCore_location}
  ${QtXml_location}
  ${QtTest_location}
  ${Boost_SYSTEM_LIBRARY_dll}
  ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY_dll})

set(CMAKE_INSTALL_SYSTEM_RUNTIME_COMPONENT "runtime")
set(CMAKE_INSTALL_DEBUG_LIBRARIES TRUE)
set(MAKE_INSTALL_UCRT_LIBRARIES TRUE)

if (${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
  set(CMAKE_INSTALL_UCRT_LIBRARIES TRUE)
endif()
include(InstallRequiredSystemLibraries)

set(CPACK_MONOLITHIC_INSTALL 1)
set(CPACK_PACKAGE_NAME "lima")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "LIMA Linguistic Analyzer")
SET(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_CURRENT_SOURCE_DIR}/README.md")
SET(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/COPYING")
set(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}-${CPACK_SYSTEM_NAME}")

# set of commands to define component based packaging
# notify cpack of component all names
set(CMAKE_INSTALL_DEFAULT_COMPONENT_NAME "runtime")
# SET(CPACK_COMPONENTS_ALL devel runtime common fre eng por)
set(CPACK_RPM_COMPONENT_INSTALL ON)
SET(CPACK_RPM_USE_DISPLAY_NAME_IN_FILENAME ON)
set(CPACK_DEB_COMPONENT_INSTALL ON)
SET(CPACK_DEBIAN_USE_DISPLAY_NAME_IN_FILENAME ON)


SET(CPACK_NSIS_INSTALL_ROOT "$PROGRAMFILES64")
SET(CPACK_NSIS_ENABLE_UNINSTALL_BEFORE_INSTALL "ON")
SET(CPACK_NSIS_MODIFY_PATH "ON")
SET(CPACK_STRIP_FILES "")
SET(CPACK_SOURCE_STRIP_FILES "")
if ("x$ENV{LIMA_DISABLE_CPACK_DEBIAN_PACKAGE_SHLIBDEPS}" STREQUAL "x")
  set(CPACK_DEBIAN_runtime_PACKAGE_SHLIBDEPS ON)
endif()
set(CPACK_GENERATOR ${SPECIFIC_SYSTEM_PREFERED_CPACK_GENERATOR})

list(APPEND CPACK_NSIS_EXTRA_INSTALL_COMMANDS "WriteRegExpandStr \\\${env_hklm} LIMA_DIST $INSTDIR")
list(APPEND CPACK_NSIS_EXTRA_INSTALL_COMMANDS "WriteRegExpandStr \\\${env_hklm} LIMA_CONF $INSTDIR\\\\share\\\\config\\\\lima")
list(APPEND CPACK_NSIS_EXTRA_INSTALL_COMMANDS "WriteRegExpandStr \\\${env_hklm} LIMA_RESOURCES $INSTDIR\\\\share\\\\apps\\\\lima\\\\resources")
list(APPEND CPACK_NSIS_EXTRA_INSTALL_COMMANDS "SendMessage \\\${HWND_BROADCAST} \\\${WM_WININICHANGE} 0 \\\"STR:Environment\\\" /TIMEOUT=5000")
string (REPLACE ";" "\n" CPACK_NSIS_EXTRA_INSTALL_COMMANDS "${CPACK_NSIS_EXTRA_INSTALL_COMMANDS}")

list(APPEND CPACK_NSIS_EXTRA_UNINSTALL_COMMANDS "DeleteRegValue \\\${env_hklm} LIMA_DIST")
list(APPEND CPACK_NSIS_EXTRA_UNINSTALL_COMMANDS "DeleteRegValue \\\${env_hklm} LIMA_CONF")
list(APPEND CPACK_NSIS_EXTRA_UNINSTALL_COMMANDS "DeleteRegValue \\\${env_hklm} LIMA_RESOURCES")
list(APPEND CPACK_NSIS_EXTRA_UNINSTALL_COMMANDS "SendMessage \\\${HWND_BROADCAST} \\\${WM_WININICHANGE} 0 \\\"STR:Environment\\\" /TIMEOUT=5000")
string (REPLACE ";" "\n" CPACK_NSIS_EXTRA_UNINSTALL_COMMANDS "${CPACK_NSIS_EXTRA_UNINSTALL_COMMANDS}")

include(CPack)

