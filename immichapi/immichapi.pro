TEMPLATE = lib
CONFIG += staticlib

# Uncomment for debug
#CONFIG -= release
#CONFIG += debug
#QMAKE_CXXFLAGS_DEBUG += -O0 -g3 -fno-omit-frame-pointer -fno-inline -D_GLIBCXX_DEBUG -Wall -Wextra
#QMAKE_CFLAGS_DEBUG   += -O0 -g
#DEFINES -= QT_NO_DEBUG
#DEFINES += QT_DEBUG
#QMAKE_STRIP =

TARGET = immichapi

DESTDIR = $$OUT_PWD

include(src/generated/client.pri)

