#-------------------------------------------------
#
# Project created by QtCreator 2015-04-16T14:46:08
#
#-------------------------------------------------

QT       -= gui

TARGET = karaoke
TEMPLATE = lib

DEFINES += KARAOKE_LIBRARY

SOURCES += karaoke.cpp

HEADERS += karaoke.h

CONFIG += dll

QMAKE_LFLAGS += -static-libgcc -static-libstdc++ -static -lpthread
