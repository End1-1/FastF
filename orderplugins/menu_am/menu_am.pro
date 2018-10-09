#-------------------------------------------------
#
# Project created by QtCreator 2015-04-17T15:30:03
#
#-------------------------------------------------

QT       += sql network gui core xml printsupport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = menu_am
TEMPLATE = lib

DEFINES += MENU_AM_LIBRARY
DEFINES += _ORGANIZATION_=\\\"SmartHotel\\\"
DEFINES += _APPLICATION_=\\\"SmartHotel\\\"

SOURCES += menu_am.cpp \
    ../../../common/qsystem.cpp \
    ../../orderdrv/od_base.cpp \
    ../../orderdrv/od_dish.cpp \
    ../../orderdrv/od_drv.cpp \
    ../../orderdrv/od_flags.cpp \
    ../../orderdrv/od_header.cpp \
    ../../orderdrv/od_print.cpp \
    ../../dbdriver.cpp \
    ../../../CafeV4/core/printing.cpp \
    ../../genqrcode.cpp \
    ../../../LibQREncode/bitstream.c \
    ../../../LibQREncode/mask.c \
    ../../../LibQREncode/mmask.c \
    ../../../LibQREncode/mqrspec.c \
    ../../../LibQREncode/qrenc.c \
    ../../../LibQREncode/qrencode.c \
    ../../../LibQREncode/qrinput.c \
    ../../../LibQREncode/qrspec.c \
    ../../../LibQREncode/rscode.c \
    ../../../LibQREncode/split.c \
    ../../ff_settingsdrv.cpp \
    ../../../common/qsqldrv.cpp \
    ../../ff_moddrv.cpp \
    ../../../common/qsqldb.cpp \
    ../../../common/qlogwindow.cpp \
    ../../ff_orderdrv.cpp \
    ../../ff_halldrv.cpp \
    ../../cnfapp.cpp \
    ../../cnfmaindb.cpp

HEADERS += menu_am.h \
    ../../orderdrv/od.h \
    ../../orderdrv/od_base.h \
    ../../orderdrv/od_config.h \
    ../../orderdrv/od_dish.h \
    ../../orderdrv/od_drv.h \
    ../../orderdrv/od_flags.h \
    ../../orderdrv/od_header.h \
    ../../orderdrv/od_print.h \
    ../../dbdriver.h \
    ../../printing.h \
    ../../../CafeV4/core/printing.h \
    ../../genqrcode.h \
    ../../../LibQREncode/bitstream.h \
    ../../../LibQREncode/config.h \
    ../../../LibQREncode/mask.h \
    ../../../LibQREncode/mmask.h \
    ../../../LibQREncode/mqrspec.h \
    ../../../LibQREncode/qrencode.h \
    ../../../LibQREncode/qrencode_inner.h \
    ../../../LibQREncode/qrinput.h \
    ../../../LibQREncode/qrspec.h \
    ../../../LibQREncode/rscode.h \
    ../../../LibQREncode/split.h \
    dbdriver.h \
    ../../ff_settingsdrv.h \
    ff_settingsdrv.h \
    ../../../common/qsqldrv.h \
    ../../ff_moddrv.h \
    ../../../common/qsqldb.h \
    ../../../common/qlogwindow.h \
    ../../ff_orderdrv.h \
    ../../ff_halldrv.h \
    ../../cnfapp.h \
    ../../cnfmaindb.h

CONFIG += dll

LIBS += -lVersion

FORMS += \
    ../../dlgcalcchange.ui \
    ../../../common/qlogwindow.ui

INCLUDEPATH += C:/Qt/projects/FastF

