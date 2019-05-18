#-------------------------------------------------
#
# Project created by QtCreator 2015-04-18T13:14:56
#
#-------------------------------------------------

QT += core gui sql network widgets xml printsupport

TARGET = karaoke_sale
TEMPLATE = lib

DEFINES += KARAOKE_SALE_LIBRARY

SOURCES += karaoke_sale.cpp \
    ../../dlgreportfilter.cpp \
    ../../orderdrv/od_base.cpp \
    ../../orderdrv/od_dish.cpp \
    ../../orderdrv/od_drv.cpp \
    ../../orderdrv/od_flags.cpp \
    ../../orderdrv/od_header.cpp \
    ../../orderdrv/od_print.cpp \
    ../../../CafeV4/core/printing.cpp \
    ../../dbdriver.cpp \
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
    ../../../common/qsystem.cpp \
    ../../genqrcode.cpp \
    ../../ff_settingsdrv.cpp \
    ../../../common/qsqldrv.cpp \
    ../../ff_moddrv.cpp \
    ../../../common/qsqldb.cpp \
    ../../../common/qlogwindow.cpp \
    ../../ff_orderdrv.cpp \
    ../../ff_halldrv.cpp \
    ../../cnfapp.cpp \
    ../../cnfmaindb.cpp

HEADERS += karaoke_sale.h \
    ../../dlgreportfilter.h \
    ../../orderdrv/od.h \
    ../../orderdrv/od_base.h \
    ../../orderdrv/od_config.h \
    ../../orderdrv/od_dish.h \
    ../../orderdrv/od_drv.h \
    ../../orderdrv/od_flags.h \
    ../../orderdrv/od_header.h \
    ../../orderdrv/od_print.h \
    ../../../CafeV4/core/printing.h \
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
    ../../../common/qsystem.h \
    ../../genqrcode.h \
    ../../ff_settingsdrv.h \
    dbdriver.h \
    ff_settingsdrv.h \
    ../../dbdriver.h \
    ../../../common/qsqldrv.h \
    ../../ff_moddrv.h \
    qsqldrv.h \
    ../../../common/qsqldb.h \
    ../../../common/qlogwindow.h \
    ../../ff_orderdrv.h \
    ../../ff_halldrv.h \
    ../../cnfapp.h \
    ../../cnfmaindb.h

CONFIG += dll

LIBS += -lVersion

FORMS += \
    ../../dlgreportfilter.ui \
    ../../../common/qlogwindow.ui

INCLUDEPATH += C:/projects/FastF

DEFINES += _ORGANIZATION_=\\\"Jazzve\\\"
DEFINES += _APPLICATION_=\\\"FastF\\\"
