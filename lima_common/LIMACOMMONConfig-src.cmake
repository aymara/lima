# - Find LimaCommon

set(LIMA_VERSION_MAJOR "2")
set(LIMA_VERSION_MINOR "1")
set(LIMA_VERSION_RELEASE @LIMA_VERSION_RELEASE@)
set(LIMA_VERSION "${LIMA_VERSION_MAJOR}.${LIMA_VERSION_MINOR}.${LIMA_VERSION_RELEASE}")

set(LIMA_GENERIC_LIB_VERSION ${LIMA_VERSION})
set(LIMA_GENERIC_LIB_SOVERSION ${LIMA_VERSION_MAJOR})

set(LIMA_COMMON_LIB_VERSION ${LIMA_GENERIC_LIB_VERSION})
set(LIMA_COMMON_LIB_SOVERSION ${LIMA_GENERIC_LIB_SOVERSION})

if ("x${LIB_INSTALL_DIR}" STREQUAL "x")
  set(LIB_INSTALL_DIR "lib")
endif ()

set(LIMACOMMON_INCLUDE_DIR @CMAKE_INSTALL_PREFIX@/include)
set(LIMACOMMON_LIBRARY_DIRS @CMAKE_INSTALL_PREFIX@/${LIB_INSTALL_DIR} )
set(LIMACOMMON_CONFIG_DIR @CMAKE_INSTALL_PREFIX@/share/config/lima)

message(STATUS "Lima version  ${LIMA_VERSION_MAJOR}.${LIMA_VERSION_MINOR}.${LIMA_VERSION_RELEASE} - LIMA_GENERIC_LIB_VERSION  ${LIMA_GENERIC_LIB_VERSION} ")

