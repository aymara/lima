SET(QT_USE_QTXML 1)


########### next target ###############

SET(annoqt_SRCS
           main.cpp
           annoqt.cpp
           annotationConfigurationHandler.cpp
           annotationEditWidget.cpp
           specificEntitiesHandler.cpp
           specificEntity.cpp)

if(QtCore_VERSION VERSION_GREATER 5.14.0)
  qt_add_resources(annoqt_QRCS annoqt.qrc)
else()
  qt5_add_resources(annoqt_QRCS annoqt.qrc)
endif()

add_executable(annoqt ${annoqt_SRCS} ${annoqt_QRCS})

target_link_libraries(annoqt ${Qt_LIBRARIES})


########### next target ###############

SET(annoqtconfeditor_SRCS
           confeditor.cpp
           confeditormain.cpp
           annotationConfigurationHandler.cpp
           kxyselector.cpp
           kselector.cpp
           kcolorvalueselector.cpp
           kcolorcollection.cpp
           kcolormimedata.cpp
           khuesaturationselect.cpp
           kcolordialog.cpp)


if(QtCore_VERSION VERSION_GREATER 5.14.0)
  qt_add_resources(annoqtconfeditor_QRCS annoqtconfeditor.qrc)
else()
  qt5_add_resources(annoqtconfeditor_QRCS annoqtconfeditor.qrc)
endif()

add_executable(annoqtconfeditor ${annoqtconfeditor_SRCS} ${annoqtconfeditor_QRCS})

target_link_libraries(annoqtconfeditor ${Qt_LIBRARIES})



########### install files ###############
install(TARGETS annoqt DESTINATION bin)
install(TARGETS annoqtconfeditor DESTINATION bin)
