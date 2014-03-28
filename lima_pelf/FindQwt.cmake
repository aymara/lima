# - Try to find the Qwt includes and library
# which defines
#
# QWT_FOUND - system has Qwt
# QWT_INCLUDE_DIR - where to find qwt.h
# QWT_LIBRARIES - the libraries to link against to use Qwt
# QWT_LIBRARY - where to find the Qwt library (not for general use)

# copyright (c) 2006 Thomas Moenicke thomas.moenicke@kdemail.net
#
# Redistribution and use is allowed according to the terms of the BSD license.

IF(NOT QT4_FOUND)
    INCLUDE(FindQt4)
ENDIF(NOT QT4_FOUND)

SET(QWT_FOUND "NO")

IF(QT4_FOUND)
    FIND_PATH(QWT_INCLUDE_DIR qwt.h
        /usr/local/qwt/include
        /usr/local/include
        /usr/include/qwt
        /usr/include
    )
    SET(QWT_NAMES ${QWT_NAMES} qwt libqwt)
    FIND_LIBRARY(QWT_LIBRARY
        NAMES ${QWT_NAMES}
        PATHS /usr/local/qwt/lib /usr/local/lib /usr/lib
    )
    IF (QWT_LIBRARY)
        SET(QWT_LIBRARIES ${QWT_LIBRARY})
        SET(QWT_FOUND "YES")
        IF (CYGWIN)
            IF(BUILD_SHARED_LIBS)
            # No need to define QWT_USE_DLL here, because it's default for Cygwin.
            ELSE(BUILD_SHARED_LIBS)
            SET (QWT_DEFINITIONS -DQWT_STATIC)
            ENDIF(BUILD_SHARED_LIBS)
        ENDIF (CYGWIN)

    ENDIF (QWT_LIBRARY)
ENDIF(QT4_FOUND)

IF (QWT_FOUND)
  IF (NOT QWT_FIND_QUIETLY)
    MESSAGE(STATUS "Found Qwt: ${QWT_LIBRARY}")
  ENDIF (NOT QWT_FIND_QUIETLY)
ELSE (QWT_FOUND)
  IF (QWT_FIND_REQUIRED)
    MESSAGE(FATAL_ERROR "Could not find Qwt library")
  ENDIF (QWT_FIND_REQUIRED)
ENDIF (QWT_FOUND)

MARK_AS_ADVANCED(QWT_INCLUDE_DIR QWT_LIBRARY)
