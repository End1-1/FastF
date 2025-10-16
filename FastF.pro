#-------------------------------------------------
#
# Project created by QtCreator 2015-01-30T16:32:27
#
#-------------------------------------------------

QT       += core gui sql network xml printsupport

TARGET = FastF
TEMPLATE = app

RC_FILE = res/res.rc

include(c:/projects/IDramAPI/Idram.pri)

INCLUDEPATH += C:/projects/NewTax/Src
INCLUDEPATH += C:/Soft/OpenSSLWin64/include
INCLUDEPATH += C:/Soft/OpenSSLWin64/include/openssl
INCLUDEPATH += ./common
INCLUDEPATH += ./orderdrv
INCLUDEPATH += ./MobilePointServer

SOURCES += main.cpp\
    QRCodeGenerator.cpp \
    common/qlogwindow.cpp \
    c5printing.cpp \
    configmobilewidget.cpp \
    database.cpp \
    dbmutexlocker.cpp \
    dlgapprovecorrectionmessage.cpp \
    dlgcardtips.cpp \
    dlgconfigmobile.cpp \
    dlgcorrection.cpp \
    dlgqty.cpp \
    dlgregistercard.cpp \
    dlgselecttaxreport.cpp \
    fastfdialog.cpp \
    htable.cpp \
    logwriter.cpp \
    orderdrv/od_config.cpp \
    qelineedit.cpp \
    ff_user.cpp \
    dlgorder.cpp \
    ff_dishesdrv.cpp \
    dlgmessage.cpp \
    ff_settingsdrv.cpp \
    ff_halldrv.cpp \
    dlgface.cpp \
    common/qsqldrv.cpp \
    common/qsystem.cpp \
    common/dlgconnection.cpp \
    dlgtableformovement.cpp \
    common/qetablewidget.cpp \
    common/dlginput.cpp \
    ff_discountdrv.cpp \
    dlgreports.cpp \
    common/dlggetpassword.cpp \
    common/wdtnumpad.cpp \
    dlgchangepass.cpp \
    dlgreportfilter.cpp \
    dlgdishcomment.cpp \
    common/qsqldb.cpp \
    dlglist.cpp \
    dlgcalcchange.cpp \
    common/qqrengine.cpp \
    qnet.cpp \
    LibQREncode/bitstream.c \
    LibQREncode/mask.c \
    LibQREncode/mmask.c \
    LibQREncode/mqrspec.c \
    LibQREncode/qrenc.c \
    LibQREncode/qrencode.c \
    LibQREncode/qrinput.c \
    LibQREncode/qrspec.c \
    LibQREncode/rscode.c \
    LibQREncode/split.c \
    dlgsplash.cpp \
    orderdrv/od_dish.cpp \
    orderdrv/od_base.cpp \
    orderdrv/od_header.cpp \
    orderdrv/od_drv.cpp \
    orderdrv/od_flags.cpp \
    orderdrv/od_print.cpp \
    dbdriver.cpp \
    ff_correcttime.cpp \
    taxcheckout.cpp \
    dlgremovereason.cpp \
    dlgkinoparkcall.cpp \
    logthread.cpp \
    dlgpaymentjazzve.cpp \
    MobilePointServer/msqldatabase.cpp \
    cnfmaindb.cpp \
    MobilePointServer/mjsonhandler.cpp \
    MobilePointServer/databaseresult.cpp \
    MobilePointServer/mtprintkitchen.cpp \
    ../NewTax/Src/printtaxn.cpp \
    MobilePointServer/pimage.cpp \
    MobilePointServer/ptextrect.cpp \
    MobilePointServer/mtfilelog.cpp \
    dlgpayment.cpp \
    elineedit.cpp \
    utils.cpp \
    cnfapp.cpp \
    mptcpsocket.cpp \
    classes.cpp

HEADERS  += \
    QRCodeGenerator.h \
    common/fastfprotocol.h \
    common/qlogwindow.h \
    c5printing.h \
    configmobilewidget.h \
    consts.h \
    database.h \
    dbmutexlocker.h \
    dlgapprovecorrectionmessage.h \
    dlgcardtips.h \
    dlgconfigmobile.h \
    dlgcorrection.h \
    dlgqty.h \
    dlgregistercard.h \
    dlgselecttaxreport.h \
    fastfdialog.h \
    htable.h \
    qelineedit.h \
    ff_user.h \
    dlgorder.h \
    ff_dishesdrv.h \
    dlgmessage.h \
    ff_settingsdrv.h \
    ff_halldrv.h \
    dlgface.h \
    qsqldrv.h \
    common/qsqldrv.h \
    common/qsystem.h \
    qsystem.h \
    common/dlgconnection.h \
    dlgconnection.h \
    dlgtableformovement.h \
    qetablewidget.h \
    common/qetablewidget.h \
    dlginput.h \
    common/dlginput.h \
    ff_discountdrv.h \
    dlgreports.h \
    dlggetpassword.h \
    common/dlggetpassword.h \
    common/wdtnumpad.h \
    dlgchangepass.h \
    dlgreportfilter.h \
    wdtnumpad.h \
    dlgdishcomment.h \
    qsqldb.h \
    common/qsqldb.h \
    dlglist.h \
    dlgcalcchange.h \
    logwriter.h \
    common/qqrengine.h \
    qnet.h \
    LibQREncode/bitstream.h \
    LibQREncode/config.h \
    LibQREncode/mask.h \
    LibQREncode/mmask.h \
    LibQREncode/mqrspec.h \
    LibQREncode/qrencode.h \
    LibQREncode/qrencode_inner.h \
    LibQREncode/qrinput.h \
    LibQREncode/qrspec.h \
    LibQREncode/rscode.h \
    LibQREncode/split.h \
    dlgsplash.h \
    orderdrv/od_dish.h \
    orderdrv/od_base.h \
    orderdrv/od_header.h \
    orderdrv/od_drv.h \
    orderdrv/od.h \
    orderdrv/od_flags.h \
    orderdrv/od_print.h \
    orderdrv/od_config.h \
    dbdriver.h \
    ff_correcttime.h \
    taxcheckout.h \
    dlgremovereason.h \
    dlgkinoparkcall.h \
    logthread.h \
    dlgpaymentjazzve.h \
    MobilePointServer/msqldatabase.h \
    cnfmaindb.h \
    MobilePointServer/mjsonhandler.h \
    MobilePointServer/databaseresult.h \
    MobilePointServer/mtprintkitchen.h \
    ../NewTax/Src/printtaxn.h \
    MobilePointServer/pimage.h \
    MobilePointServer/ptextrect.h \
    MobilePointServer/mtfilelog.h \
    dlgpayment.h \
    elineedit.h \
    utils.h \
    cnfapp.h \
    LibQREncode/getopt.h \
    mptcpsocket.h \
    classes.h


FORMS    += \
    common/qlogwindow.ui \
    configmobilewidget.ui \
    dlgapprovecorrectionmessage.ui \
    dlgcardtips.ui \
    dlgconfigmobile.ui \
    dlgcorrection.ui \
    dlgorder.ui \
    dlgmessage.ui \
    dlgface.ui \
    common/dlgconnection.ui \
    dlgqty.ui \
    dlgregistercard.ui \
    dlgselecttaxreport.ui \
    dlgtableformovement.ui \
    common/dlginput.ui \
    dlgreports.ui \
    common/dlggetpassword.ui \
    common/wdtnumpad.ui \
    dlgchangepass.ui \
    dlgreportfilter.ui \
    dlgdishcomment.ui \
    dlglist.ui \
    dlgcalcchange.ui \
    dlgsplash.ui \
    dlgremovereason.ui \
    dlgkinoparkcall.ui \
    dlgpaymentjazzve.ui \
    dlgpayment.ui

RESOURCES += \
    Res.qrc

ICON = res/app.ico

DEFINES += FASTF
DEFINES += _ORGANIZATION_=\\\"Jazzve\\\"
DEFINES += _APPLICATION_=\\\"FastF\\\"
DEFINES += _MODULE_=\\\"FASTf\\\"

LIBS += -lVersion

LIBS += -LC:/soft/OpenSSLWin64/lib/VC/x64/MD
LIBS += -lopenssl
LIBS += -llibcrypto
LIBS += -ladvapi32
LIBS += -lwsock32







