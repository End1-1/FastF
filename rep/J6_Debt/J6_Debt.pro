#-------------------------------------------------
#
# Project created by QtCreator 2015-05-11T11:54:36
#
#-------------------------------------------------

QT       -= gui

TARGET = J6_Debt
TEMPLATE = lib

DEFINES += J6_DEBT_LIBRARY

SOURCES += j6_debt.cpp

HEADERS += j6_debt.h

CONFIG += dll

QMAKE_LFLAGS += -static-libgcc -static-libstdc++ -static -lpthread
