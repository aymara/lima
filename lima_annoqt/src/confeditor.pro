SOURCES += confeditor.cpp \
           confeditormain.cpp \
           annotationConfigurationHandler.cpp \
           kxyselector.cpp \
           kselector.cpp \
           kcolorvalueselector.cpp \
           kcolorcollection.cpp \
           kcolormimedata.cpp \
           khuesaturationselect.cpp \
           kcolordialog.cpp
HEADERS += confeditor.h \
annotationConfigurationHandler.h \
kselector.h \
kxyselector.h \
kcolorvalueselector.h \
kcolorcollection.h \
kcolormimedata.h \
khuesaturationselect.h \
kcolordialog.h \
kcolordialog_p.h
TEMPLATE = app
CONFIG += warn_on \
	  thread \
          qt \
	  debug_and_release
TARGET = ../bin/annoqtconfeditor
RESOURCES = annoqtconfeditor.qrc
QT += gui \
xml
QMAKE_CXXFLAGS_DEBUG += -O0 \
-g3
