#-------------------------------------------------
#
# Project created by QtCreator 2015-04-15T01:31:40
#
#-------------------------------------------------

QT       += core

TARGET = nargile
TEMPLATE = lib

DEFINES += ICE_LIBRARY

SOURCES += ice.cpp

HEADERS += ice.h

CONFIG += dll

QMAKE_LFLAGS += -static-libgcc -static-libstdc++ -static -lpthread
