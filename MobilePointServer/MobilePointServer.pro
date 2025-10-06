QT += core network sql printsupport gui

CONFIG += c++11

TARGET = MobilePointServer
CONFIG += console
CONFIG -= app_bundle

INCLUDEPATH += ../LibQREncode

RC_FILE = res.rc

TEMPLATE = app

SOURCES += main.cpp \
    mtcpserver.cpp \
    msqldatabase.cpp \
    databaseresult.cpp \
    utils.cpp \
    jsonutils.cpp \
    mtcpsocketthread.cpp \
    mudpserver.cpp \
    mdefined.cpp \
    mjsonhandler.cpp \
    mpdraftsocket.cpp \
    mtprintkitchen.cpp \
    pimage.cpp \
    pprintscene.cpp \
    ptextrect.cpp \
    genqrcode.cpp \
    ../LibQREncode/bitstream.c \
    ../LibQREncode/mask.c \
    ../LibQREncode/mmask.c \
    ../LibQREncode/mqrspec.c \
    ../LibQREncode/qrenc.c \
    ../LibQREncode/qrencode.c \
    ../LibQREncode/qrinput.c \
    ../LibQREncode/qrspec.c \
    ../LibQREncode/rscode.c \
    ../LibQREncode/split.c \
    mtfilelog.cpp \
    QRCodeGenerator.cpp

HEADERS += \
    mtcpserver.h \
    mdefined.h \
    msqldatabase.h \
    databaseresult.h \
    utils.h \
    jsonutils.h \
    mtcpsocketthread.h \
    mudpserver.h \
    mjsonhandler.h \
    mpdraftsocket.h \
    mtprintkitchen.h \
    pimage.h \
    pprintscene.h \
    ptextrect.h \
    pdefaults.h \
    genqrcode.h \
    ../LibQREncode/qrspec.h \
    ../LibQREncode/bitstream.h \
    ../LibQREncode/config.h \
    ../LibQREncode/mask.h \
    ../LibQREncode/mmask.h \
    ../LibQREncode/mqrspec.h \
    ../LibQREncode/qrencode.h \
    ../LibQREncode/qrencode_inner.h \
    ../LibQREncode/qrinput.h \
    ../LibQREncode/qrspec.h \
    ../LibQREncode/rscode.h \
    ../LibQREncode/split.h \
    mtfilelog.h \
    QRCodeGenerator.h

LIBS += -lVersion
LIBS += -lwsock32


#QMAKE_CXXFLAGS += -Werror

RESOURCES += \
    res.qrc
