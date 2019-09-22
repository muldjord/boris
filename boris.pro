TEMPLATE = app
TARGET = Boris
DEPENDPATH += .
INCLUDEPATH += .
#CONFIG += console debug
QT += widgets xml network

include(./VERSION)
DEFINES+=VERSION=\\\"$$VERSION\\\"

LIBS += -lSDL2 -lSDL2_mixer

# Input
HEADERS += src/mainwindow.h \
           src/about.h \
           src/boris.h \
           src/behaviour.h \
           src/stats.h \
           src/loader.h \
           src/netcomm.h \
           src/chatter.h

SOURCES += src/mainwindow.cpp \
           src/about.cpp \
           src/boris.cpp \
           src/behaviour.cpp \
           src/main.cpp \
           src/stats.cpp \
           src/loader.cpp \
           src/netcomm.cpp \
           src/chatter.cpp

RESOURCES += boris.qrc
TRANSLATIONS = boris_da_DK.ts
RC_FILE = boris.rc
ICON = icon.icns
