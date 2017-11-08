TEMPLATE = app
TARGET = Boris
DEPENDPATH += .
INCLUDEPATH += .
#CONFIG += console debug
QT += widgets multimedia xml

include(./VERSION)
DEFINES+=VERSION=\\\"$$VERSION\\\"

# Input
HEADERS += mainwindow.h \
           about.h \
           boris.h \
           behaviour.h \
           stats.h \
           loader.h \
           netcomm.h \
           chatter.h

SOURCES += mainwindow.cpp \
           about.cpp \
           boris.cpp \
           main.cpp \
           stats.cpp \
           loader.cpp \
           netcomm.cpp \
           chatter.cpp

RESOURCES += boris.qrc
TRANSLATIONS = boris_da_DK.ts
RC_FILE = boris.rc
ICON = icon.icns
