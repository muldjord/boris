TEMPLATE = app
TARGET = Boris
QT += widgets xml network

include(./VERSION)
DEFINES+=VERSION=\\\"$$VERSION\\\"

win32:INCLUDEPATH += $$OUT_PWD/SFML-2.5.1/include
win32:LIBS += -L$$OUT_PWD/SFML-2.5.1/lib
LIBS += -lsfml-audio

# Input
HEADERS += src/mainwindow.h \
           src/soundmixer.h \
           src/about.h \
           src/boris.h \
           src/item.h \
           src/scripthandler.h \
           src/behaviour.h \
           src/stats.h \
           src/loader.h \
           src/netcomm.h \
           src/bubble.h

SOURCES += src/mainwindow.cpp \
           src/soundmixer.cpp \
           src/about.cpp \
           src/boris.cpp \
           src/item.cpp \
           src/scripthandler.cpp \
           src/behaviour.cpp \
           src/main.cpp \
           src/stats.cpp \
           src/loader.cpp \
           src/netcomm.cpp \
           src/bubble.cpp

RESOURCES += boris.qrc
TRANSLATIONS = boris_da_DK.ts
RC_FILE = boris.rc
ICON = icon.icns
