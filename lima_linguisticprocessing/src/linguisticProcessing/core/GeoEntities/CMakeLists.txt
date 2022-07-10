# Copyright 2002-2013 CEA LIST
# SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
#
# SPDX-License-Identifier: MIT

include_directories(BEFORE ${CMAKE_SOURCE_DIR}/src )

# FIND_PACKAGE(SOCI PATHS $ENV{MM_EXTERNALS}/etc)
# SET(MYSQL_LIBRARIES NOTFOUND)
# SET(SOCIMYSQL_LIBRARIES NOTFOUND)
# SET(SOCIPOSTGRE_LIBRARIES NOTFOUND)
# SET(SOCISQLITE3_LIBRARIES NOTFOUND)
# SET(GEOENTITIES NOTFOUND)
# 
# FIND_LIBRARY(MYSQL_LIBRARIES NAMES mysqlclient  PATHS /usr/lib /usr/lib64 /usr/lib64/mysql /usr/lib/mysql)
# FIND_LIBRARY(SOCIMYSQL_LIBRARIES NAMES soci_mysql-3  PATHS $ENV{MM_EXTERNALS}/lib)
# FIND_LIBRARY(SOCIPOSTGRE_LIBRARIES NAMES soci_postgresql-3  PATHS $ENV{MM_EXTERNALS}/lib)
# FIND_LIBRARY(SOCISQLITE3_LIBRARIES NAMES soci_sqlite3 soci_sqlite-3 PATHS $ENV{MM_EXTERNALS}/lib)
# 
# SET(optionalLibs)
# 
# IF (SOCI_FOUND)
#   SET(optionalLibs ${optionalLibs} soci_core-3)
#   #SET(optionalLibs ${optionalLibs} lima-lp-geoentitiestagger)
# 
#   IF(NOT MYSQL_LIBRARIES STREQUAL "MYSQL_LIBRARIES-NOTFOUND")
#   SET(optionalLibs ${optionalLibs} ${MYSQL_LIBRARIES})
#   ENDIF(NOT MYSQL_LIBRARIES STREQUAL "MYSQL_LIBRARIES-NOTFOUND")
# 
#   IF(NOT SOCIMYSQL_LIBRARIES STREQUAL "SOCIMYSQL_LIBRARIES-NOTFOUND")
#   SET(optionalLibs ${optionalLibs}  soci_mysql-3)
#   ENDIF(NOT SOCIMYSQL_LIBRARIES STREQUAL "SOCIMYSQL_LIBRARIES-NOTFOUND")
# 
#   IF(NOT SOCIPOSTGRE_LIBRARIES STREQUAL "SOCIPOSTGRE_LIBRARIES-NOTFOUND")
#   SET(optionalLibs ${optionalLibs} soci_postgresql-3 dl pq)
#   ENDIF(NOT SOCIPOSTGRE_LIBRARIES STREQUAL "SOCIPOSTGRE_LIBRARIES-NOTFOUND")
# 
#   IF(NOT SOCISQLITE3_LIBRARIES STREQUAL "SOCISQLITE3_LIBRARIES-NOTFOUND")
#   SET(optionalLibs ${optionalLibs}  soci_sqlite3 sqlite3)
#   ENDIF(NOT SOCISQLITE3_LIBRARIES STREQUAL "SOCISQLITE3_LIBRARIES-NOTFOUND")
# ENDIF(SOCI_FOUND)


########### next target ###############

SET(lima-lp-geoentitiestagger_LIB_SRCS
  GeoEntitiesTagger.cpp 
  GeoEntityAnnotation.cpp
  GeoDumper.cpp
  GeoDbContext.cpp
)

DECLARE_LIMA_PLUGIN(lima-lp-geoentitiestagger)

target_link_libraries(lima-lp-geoentitiestagger ${optionalLibs})

set_target_properties(lima-lp-geoentitiestagger PROPERTIES VERSION ${LIMA_LP_LIB_VERSION} SOVERSION ${LIMA_LP_LIB_SOVERSION})

install(TARGETS lima-lp-geoentitiestagger DESTINATION ${LIB_INSTALL_DIR}
  COMPONENT runtime)

SET(optionalLibs ${optionalLibs} lima-lp-geoentitiestagger)


########### install files ###############
