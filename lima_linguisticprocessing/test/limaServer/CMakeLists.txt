INCLUDE(${QT_USE_FILE})

########### next target ###############

include_directories( ${CMAKE_SOURCE_DIR}/src ${CMAKE_SOURCE_DIR}/test ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR})

SET(limaserver_SRCS
   LimaServer.cpp
   main.cpp
   analysisthread.cpp
)

set (limaserver_MOC_HEADERS 
    LimaServer.h
    analysisthread.h )


qt4_wrap_cpp (limaserver_MOC_OUTFILES ${limaserver_MOC_HEADERS})

# qt4_add_resources(limaserver amoseserver.qrc)

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

qt4_generate_dbus_interface(
    LimaDBusServer.h
    fr.cea.limadbusserver.xml
    OPTIONS -a -M
)
# install(FILES ${CMAKE_CURRENT_BINARY_DIR}/fr.cea.limadbusserver.xml DESTINATION ${DBUS_INTERFACES_INSTALL_DIR})

qt4_add_dbus_adaptor(limadbusserver_SRCS ${CMAKE_CURRENT_BINARY_DIR}/fr.cea.limadbusserver.xml
                     LimaDBusServer.h LimaDBusServer)

qt4_wrap_cpp (limadbusserver_MOC_OUTFILES ${limadbusserver_MOC_HEADERS})

# qt4_add_resources(limadbusserver amoseserver.qrc)

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
