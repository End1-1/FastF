#-------------------------------------------------
#
# Project created by QtCreator 2015-05-08T10:52:38
#
#-------------------------------------------------

QT       -= gui

TARGET = j6_special
TEMPLATE = lib

DEFINES += J6_SPECIAL_LIBRARY

SOURCES += j6_special.cpp

HEADERS += j6_special.h

CONFIG += dll

QMAKE_LFLAGS += -static-libgcc -static-libstdc++ -static -lpthread
