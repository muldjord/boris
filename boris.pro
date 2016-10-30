TEMPLATE = app
TARGET = Boris
DEPENDPATH += .
INCLUDEPATH += .
#CONFIG += console debug
QT += widgets multimedia

include(./VERSION)
DEFINES+=VERSION=\\\"$$VERSION\\\"

# Input
HEADERS += mainwindow.h \
           about.h \
           boris.h \
           behaviour.h \
           stats.h \
           loader.h \
           weathercomm.h \
           chatter.h

SOURCES += mainwindow.cpp \
           about.cpp \
           boris.cpp \
           main.cpp \
           stats.cpp \
           loader.cpp \
           weathercomm.cpp \
           chatter.cpp

RESOURCES += boris.qrc
TRANSLATIONS = boris_da_DK.ts
RC_FILE = boris.rc
ICON = icon.icns
