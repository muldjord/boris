TEMPLATE = app
TARGET = Boris
INCLUDEPATH += $$PWD/SDL2-2.0.10/x86_64-w64-mingw32/include
#CONFIG += console debug
QT += widgets xml network
CONFIG-= windows
QMAKE_LFLAGS += $$QMAKE_LFLAGS_WINDOWS
win32:QMAKE_LFLAGS += -shared

include(./VERSION)
DEFINES+=VERSION=\\\"$$VERSION\\\"

win32:LIBS += -L$$OUT_PWD/SDL2-2.0.10/x86_64-w64-mingw32/lib
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
