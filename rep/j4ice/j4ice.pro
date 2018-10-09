#-------------------------------------------------
#
# Project created by QtCreator 2015-06-02T15:37:54
#
#-------------------------------------------------

QT       -= gui

TARGET = j4ice
TEMPLATE = lib

DEFINES += J4ICE_LIBRARY

SOURCES += j4ice.cpp

HEADERS += j4ice.h

CONFIG += dll

QMAKE_LFLAGS += -static-libgcc -static-libstdc++ -static -lpthread
