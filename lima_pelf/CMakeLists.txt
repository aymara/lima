# Define project
project(pelf)
cmake_minimum_required(VERSION 2.8.3)

enable_testing()

include(${CMAKE_SOURCE_DIR}/../SystemSpecificInformations.cmake)


cmake_policy(SET CMP0017 NEW)

SET(optionalLibs)

if (NOT (${CMAKE_SYSTEM_NAME} STREQUAL "Windows"))
  message("Linux flags")
  
  # Flags needed for the plugins mechanism to work: our libs are dynamically loaded by
  # factories, thus their symbols must be known even if not used by the binary
  set(CMAKE_EXE_LINKER_FLAGS "-Wl,--no-as-needed")
  set(CMAKE_SHARED_LINKER_FLAGS "-Wl,--no-as-needed")
  set(CMAKE_MODULE_LINKER_FLAGS "-Wl,--no-as-needed")


  include(CheckCXXCompilerFlag)
  CHECK_CXX_COMPILER_FLAG(-std=c++0x HAVE_STDCPP0X)
  set(CMAKE_CXX_FLAGS "-W -Wall ${CMAKE_CXX_FLAGS}")
  if (HAVE_STDCPP0X)
    message("C++0x supported")
    set(CMAKE_CXX_FLAGS "-std=c++0x -DBOOST_NO_HASH ${CMAKE_CXX_FLAGS}")
  else (HAVE_STDCPP0X)
    message("C++0x NOT supported")
    set(CMAKE_CXX_FLAGS "-DNO_STDCPP0X ${CMAKE_CXX_FLAGS}")
  endif (HAVE_STDCPP0X)

  set(LIB_INSTALL_DIR "lib")
else (NOT (${CMAKE_SYSTEM_NAME} STREQUAL "Windows"))
  message("Windows flags")
  add_definitions(-D WIN32)

  # By default, do not warn when built on machines using only VS Express:
  IF(NOT DEFINED CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS_NO_WARNINGS)
    SET(CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS_NO_WARNINGS ON)
  ENDIF(NOT DEFINED CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS_NO_WARNINGS)

  set(CMAKE_C_FLAGS "/Zc:wchar_t- /EHsc /GR ${CMAKE_C_FLAGS}")
  set(CMAKE_CXX_FLAGS "/Zc:wchar_t- /EHsc /GR /W4 /MP /FAu ${CMAKE_CXX_FLAGS}")

  set(LIB_INSTALL_DIR "bin")
endif (NOT (${CMAKE_SYSTEM_NAME} STREQUAL "Windows"))

    
# Find*.cmake will be searched in the following dirs if not found in system dirs
set(CMAKE_MODULE_PATH
  "${CMAKE_SOURCE_DIR}"
  "${CMAKE_SOURCE_DIR}/cmake/Modules" # for those available in this project
  "${CMAKE_INSTALL_PREFIX}/share/apps/lima/cmake/Modules" # for those available in the destination dir
  "$ENV{LIMA_EXTERNALS}/share/apps/cmake/Modules" # for those available in the externals dir
  "/usr/share/apps/lima/cmake/Modules" # for those available on the system
  "/usr/share/cmake/Modules" # for those available on the system
  )

# find_path and find_library will search in these dirs too
set(CMAKE_PREFIX_PATH
    "${CMAKE_INSTALL_PREFIX}"
    "$ENV{LIMA_EXTERNALS}"
)

SET(optionalLibs)

FIND_PACKAGE(SVMTOOL++ PATHS $ENV{LIMA_EXTERNALS}/share/cmake/Modules /usr/share/cmake/Modules)
IF(SVMTOOL++_FOUND)
  SET(optionalLibs ${optionalLibs} ${SVMTOOL++_NAMES})
  include_directories($ENV{LIMA_EXTERNALS}/include)
  link_directories($ENV{LIMA_EXTERNALS}/lib)
ENDIF(SVMTOOL++_FOUND)

include(${CMAKE_SOURCE_DIR}/../manageQt5.cmake)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
addQt5Modules(Core Xml XmlPatterns Gui Widgets)

include_directories(${Qt5_INCLUDES})
set(QT_LIBRARIES "${Qt5_LIBRARIES}")

# Find required packages
message("Searching for Qwt")
find_package(Qwt)
if(Qwt_FOUND)
    include_directories(QWT_INCLUDE_DIR)
else()
message(FATAL "Could not find Qwt")
endif()

add_definitions( -DBOOST_ALL_DYN_LINK )
set( Boost_USE_STATIC_LIBS OFF )
set( Boost_USE_STATIC_RUNTIME OFF )
set( BOOST_LIB_DIAGNOSTIC )
FIND_PACKAGE(Boost 1.46 REQUIRED COMPONENTS regex filesystem program_options system thread unit_test_framework)
include_directories(${Boost_INCLUDE_DIR})
link_directories(${Boost_LIBRARY_DIRS})


# find_package(LimaCommon REQUIRED PATHS ${CMAKE_INSTALL_PREFIX}/share/apps/lima/cmake/Modules /usr/share/apps/lima/cmake/Modules)
# include_directories(${LIMACOMMON_INCLUDE_DIR})
# link_directories(${LIMACOMMON_LIBRARY_DIRS})
# include(LimaMacros)
# 
FIND_PACKAGE(LIMACOMMON PATHS ${CMAKE_INSTALL_PREFIX}/share/apps/lima/cmake/Modules)
IF(LIMACOMMON_FOUND)
  include_directories( AFTER ${LIMACOMMON_INCLUDE_DIR})
  link_directories( ${LIMACOMMON_LIBRARY_DIRS})
ELSE (LIMACOMMON_FOUND)
  MESSAGE(FATAL_ERROR "Could not find lima_common")
ENDIF(LIMACOMMON_FOUND)


FIND_PACKAGE(LIMALP PATHS ${CMAKE_INSTALL_PREFIX}/share/apps/lima/cmake/Modules)
IF(LIMALP_FOUND)
  include_directories( AFTER ${LIMALP_INCLUDE_DIR})
  link_directories( ${LIMALP_LIBRARY_DIRS})
ELSE (LIMALP_FOUND)
  MESSAGE(FATAL_ERROR "Could not find lima_linguisticprocessing")
ENDIF(LIMALP_FOUND)

set(LIMA_PELF_LIB_VERSION ${LIMA_GENERIC_LIB_VERSION})
set(LIMA_PELF_LIB_SOVERSION ${LIMA_GENERIC_LIB_SOVERSION})


# Go to subdirectories
add_subdirectory(easyprocessor)
if (DEFINED ENV{PELF_EVALUATOR} AND DEFINED ENV{PELF_ALIGNER} AND Qwt_FOUND)
  set(PELF_EVALUATOR $ENV{PELF_EVALUATOR})
  set(PELF_ALIGNER $ENV{PELF_ALIGNER})
  add_subdirectory(benchmarkingTool)
else ()
  message("Environment variable PELF_ALIGNER and PELF_EVALUATOR should be set to build the benchmarking tool")
endif()


if (Qwt_FOUND)
add_subdirectory(resourceTool)
endif()
add_subdirectory(evalPosTagging)
add_subdirectory(commands)



########### install files ###############
# install(FILES LimaPelfConfig.cmake DESTINATION share/apps/lima/cmake/Modules COMPONENT devel)

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
INCLUDE(InstallRequiredSystemLibraries)

set(CPACK_MONOLITHIC_INSTALL 1)
SET(CPACK_PACKAGE_NAME "limapelf")
SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY "LIMA performance evaluation framework")
SET(CPACK_PACKAGE_VENDOR "CEA LIST")
SET(CPACK_PACKAGE_CONTACT "Gaël de Chalendar <Gael.de-Chalendar@cea.fr>")
SET(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_CURRENT_SOURCE_DIR}/README")
SET(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/COPYING")
SET(CPACK_PACKAGE_VERSION_MAJOR "${LIMA_VERSION_MAJOR}")
SET(CPACK_PACKAGE_VERSION_MINOR "${LIMA_VERSION_MINOR}")
SET(CPACK_PACKAGE_VERSION_PATCH "${LIMA_VERSION_RELEASE}")
SET(CPACK_PACKAGE_INSTALL_DIRECTORY "CMake ${CMake_VERSION_MAJOR}.${CMake_VERSION_MINOR}")
SET(CPACK_STRIP_FILES "bin/testDict16;bin/testLexicon")
SET(CPACK_SOURCE_STRIP_FILES "")
set(CPACK_GENERATOR ${SPECIFIC_SYSTEM_PREFERED_CPACK_GENERATOR})
INCLUDE(CPack)
