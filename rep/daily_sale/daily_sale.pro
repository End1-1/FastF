#-------------------------------------------------
#
# Project created by QtCreator 2015-04-14T21:23:35
#
#-------------------------------------------------

QT       += core

TARGET = daily_sale
TEMPLATE = lib

DEFINES += DAILY_SALE_LIBRARY

SOURCES += daily_sale.cpp

HEADERS += daily_sale.h

CONFIG += dll

QMAKE_LFLAGS += -static-libgcc -static-libstdc++ -static -lpthread
