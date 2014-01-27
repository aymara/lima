TEMPLATE = app
TARGET = ../bin/annoqt
SOURCES += annoqt.cpp \
           main.cpp \
           annotationConfigurationHandler.cpp \
           annotationEditWidget.cpp \
           specificEntitiesHandler.cpp \
           specificEntity.cpp
HEADERS += annoqt.h \
annotationConfigurationHandler.h \
annotationEditWidget.h \
specificEntitiesHandler.h \
specificEntity.h
CONFIG += warn_on \
	  thread \
          qt \
	  debug_and_release
RESOURCES = annoqt.qrc
QT += gui \
xml
QMAKE_CXXFLAGS_DEBUG += -O0 \
-g3
