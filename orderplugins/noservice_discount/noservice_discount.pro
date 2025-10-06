#-------------------------------------------------
#
# Project created by QtCreator 2015-04-17T15:30:03
#
#-------------------------------------------------

QT       += sql network gui core xml printsupport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = noservice_discount
TEMPLATE = lib

DEFINES += MENU_AM_LIBRARY
DEFINES += _ORGANIZATION_=\\\"SmartHotel\\\"
DEFINES += _APPLICATION_=\\\"SmartHotel\\\"

SOURCES += menu_am.cpp \
    ../../c5printing.cpp \
    ../../cnfmaindb.cpp \
    ../../common/qlogwindow.cpp \
    ../../common/qsqldb.cpp \
    ../../common/qsqldrv.cpp \
    ../../common/qsystem.cpp \
    ../../dbdriver.cpp \
    ../../dbmutexlocker.cpp \
    ../../ff_settingsdrv.cpp \
    ../../logthread.cpp \
    ../../orderdrv/od_base.cpp \
    ../../orderdrv/od_config.cpp \
    ../../orderdrv/od_dish.cpp \
    ../../orderdrv/od_drv.cpp \
    ../../orderdrv/od_flags.cpp \
    ../../orderdrv/od_header.cpp \
    ../../orderdrv/od_print.cpp


HEADERS += menu_am.h \
    ../../c5printing.h \
    ../../cnfmaindb.h \
    ../../common/qlogwindow.h \
    ../../common/qsqldb.h \
    ../../common/qsqldrv.h \
    ../../common/qsystem.h \
    ../../dbdriver.h \
    ../../dbmutexlocker.h \
    ../../ff_settingsdrv.h \
    ../../logthread.h \
    ../../orderdrv/od.h \
    ../../orderdrv/od_base.h \
    ../../orderdrv/od_config.h \
    ../../orderdrv/od_dish.h \
    ../../orderdrv/od_drv.h \
    ../../orderdrv/od_flags.h \
    ../../orderdrv/od_header.h \
    ../../orderdrv/od_print.h

CONFIG += dll

LIBS += -lVersion

INCLUDEPATH += C:/projects/FastF
INCLUDEPATH += C:/projects/FastF/common

FORMS += \
    ../../common/qlogwindow.ui

