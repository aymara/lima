# SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
#
# SPDX-License-Identifier: MIT

SET(QT_USE_QTXML 1)


########### next target ###############

SET(annoqt_SRCS
           main.cpp
           annoqt.cpp
           annotationConfigurationHandler.cpp
           annotationEditWidget.cpp
           specificEntitiesHandler.cpp
           specificEntity.cpp)

qt5_add_resources(annoqt_QRCS annoqt.qrc)

add_executable(annoqt ${annoqt_SRCS} ${annoqt_QRCS})

target_link_libraries(annoqt ${Qt5_LIBRARIES})


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

qt5_add_resources(annoqtconfeditor_QRCS annoqtconfeditor.qrc)

add_executable(annoqtconfeditor ${annoqtconfeditor_SRCS} ${annoqtconfeditor_QRCS})

target_link_libraries(annoqtconfeditor ${Qt5_LIBRARIES})



########### install files ###############
install(TARGETS annoqt DESTINATION bin)
install(TARGETS annoqtconfeditor DESTINATION bin)
