#-------------------------------------------------
#
# Project created by QtCreator 2016-09-12T09:50:05
#
#-------------------------------------------------
QT += sql network gui widgets xml printsupport

TARGET = aclub
TEMPLATE = lib

CONFIG += dll

DEFINES += ACLUB_LIBRARY

SOURCES += aclub.cpp \
    ../../dbdriver.cpp \
    ../../orderdrv/od_drv.cpp \
    ../../orderdrv/od_base.cpp \
    ../../orderdrv/od_dish.cpp \
    ../../orderdrv/od_flags.cpp \
    ../../orderdrv/od_header.cpp \
    ../../orderdrv/od_print.cpp \
    ../../ff_settingsdrv.cpp \
    ../../../CafeV4/core/printing.cpp \
    ../../../common/qsystem.cpp \
    ../../../common/qsqldrv.cpp \
    ../../genqrcode.cpp \
    ../../../common/qsqldb.cpp \
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
    ../../../common/qlogwindow.cpp

HEADERS += aclub.h \
    dbdriver.h \
    ../../dbdriver.h \
    ../../orderdrv/od_drv.h \
    od_drv.h \
    ../../orderdrv/od_base.h \
    ../../orderdrv/od_dish.h \
    ../../orderdrv/od_flags.h \
    ../../orderdrv/od_header.h \
    ../../ff_settingsdrv.h \
    ff_settingsdrv.h \
    ../../orderdrv/od_print.h \
    ../../../CafeV4/core/printing.h \
    ../../../common/qsystem.h \
    ../../../common/qsqldrv.h \
    ../../genqrcode.h \
    ../../../common/qsqldb.h \
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
    ../../../common/qlogwindow.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}

LIBS += -lVersion

FORMS += \
    ../../../common/qlogwindow.ui
