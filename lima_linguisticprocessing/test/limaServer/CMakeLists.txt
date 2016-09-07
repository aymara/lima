
# find_package(QHttpServer QUIET)
# if (NOT QHTTPSERVER_FOUND)
#   message(STATUS "QHttpServer Not found...")
# else(NOT QHTTPSERVER_FOUND)
#   message(STATUS "Found QHttpServer??")
#   message (STATUS,"QHTTPSERVER_NAMES: ${QHTTPSERVER_NAMES}")
#   message(STATUS "QHTTPSERVERCPP_INCLUDE_DIR=${QHTTPSERVERCPP_INCLUDE_DIR}")
#   message(STATUS "QHTTPSERVERCPP_LIBRARY_DIRS=${QHTTPSERVERCPP_LIBRARY_DIRS}")
#   include_directories($ENV{LIMA_EXTERNALS}/include)
#   link_directories($ENV{LIMA_EXTERNALS}/lib)
# endif (NOT QHTTPSERVER_FOUND)


########### next target ###############
include_directories(BEFORE ${CMAKE_SOURCE_DIR}/src ${CMAKE_SOURCE_DIR}/test ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR})

# get_property(dirs DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY INCLUDE_DIRECTORIES)
# foreach(dir ${dirs})
#   message(STATUS "dir='${dir}'")
# endforeach()

SET(limaserver_SRCS
   LimaServer.cpp
   main.cpp
   analysisthread.cpp
)

set (limaserver_MOC_HEADERS 
    LimaServer.h
    analysisthread.h )


qt5_wrap_cpp (limaserver_MOC_OUTFILES ${limaserver_MOC_HEADERS})

# qt5_add_resources(limaserver amoseserver.qrc)

# add_executable(limaserver ${amoseserver_SRCS} ${amoseserver_QRCS} ${amoseserver_MOC_OUTFILES})
add_executable(limaserver ${limaserver_SRCS} ${limaserver_MOC_OUTFILES})

target_link_libraries(limaserver
    lima-common
    lima-linguisticprocessing
    ${QHTTPSERVER_NAMES}
    ${QT_LIBRARIES}
    ${Boost_LIBRARIES})

install(TARGETS limaserver DESTINATION bin)

########### next target ###############

# SET(testQsLogger_SRCS
#    testQHttpServer.cpp
# )

# add_executable(testQsLogger ${testQsLogger_SRCS})

# target_link_libraries(testQHttpServer
#     lima-common
#     ${QHTTPSERVER_NAMES}
#     ${QT_LIBRARIES}
#     ${Boost_LIBRARIES})

# install(TARGETS testQHttpServer DESTINATION bin)

########### next target ###############
if ("${QT_DBUSCPP2XML_EXECUTABLE}" STREQUAL "QT_DBUSCPP2XML_EXECUTABLE-NOTFOUND")
  message(WARNING "QtDbus not found: cannot build Lima Dbus server.")
else ()

SET(limadbusserver_SRCS
  LimaDBusServer.cpp
  mainLimaDBusServer.cpp
  AnalysisWrapper.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/fr.cea.limadbusserver.xml
)

set (limadbusserver_MOC_HEADERS
    LimaDBusServer.h
    AnalysisWrapper.h
    )

qt5_generate_dbus_interface(
    LimaDBusServer.h
    fr.cea.limadbusserver.xml
    OPTIONS -a -M
)
# install(FILES ${CMAKE_CURRENT_BINARY_DIR}/fr.cea.limadbusserver.xml DESTINATION ${DBUS_INTERFACES_INSTALL_DIR})

qt5_add_dbus_adaptor(limadbusserver_SRCS ${CMAKE_CURRENT_BINARY_DIR}/fr.cea.limadbusserver.xml
                     LimaDBusServer.h LimaDBusServer)

qt5_wrap_cpp (limadbusserver_MOC_OUTFILES ${limadbusserver_MOC_HEADERS})

# qt5_add_resources(limadbusserver amoseserver.qrc)

# add_executable(limadbusserver ${amoseserver_SRCS} ${amoseserver_QRCS} ${amoseserver_MOC_OUTFILES})
add_executable(limadbusserver ${limadbusserver_SRCS} ${limadbusserver_MOC_OUTFILES})

target_link_libraries(limadbusserver
    lima-common
    lima-linguisticprocessing
    ${Boost_LIBRARIES}
    ${QT_LIBRARIES}
    ${QHTTPSERVER_NAMES})

install(TARGETS limadbusserver DESTINATION bin)
endif()
