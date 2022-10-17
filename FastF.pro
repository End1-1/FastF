#-------------------------------------------------
#
# Project created by QtCreator 2015-01-30T16:32:27
#
#-------------------------------------------------

QT       += core gui sql network xml printsupport

TARGET = FastF
TEMPLATE = app

RC_FILE = res/res.rc

INCLUDEPATH += C:/projects/MobilePointServer
INCLUDEPATH += C:/projects/NewTax/Src
INCLUDEPATH += C:/Soft/OpenSSL-Win32/include
INCLUDEPATH += C:/Soft/OpenSSL-Win32/include/openssl
INCLUDEPATH += C:/projects/common
INCLUDEPATH += C:/projects/FastF/orderdrv

SOURCES += main.cpp\
    c5printing.cpp \
    configmobilewidget.cpp \
    dbmutexlocker.cpp \
    dlgapprovecorrectionmessage.cpp \
    dlgconfigmobile.cpp \
    dlgcorrection.cpp \
    dlgqty.cpp \
    dlgselecttaxreport.cpp \
    orderdrv/od_config.cpp \
    qelineedit.cpp \
    ff_user.cpp \
    dlgorder.cpp \
    ff_dishesdrv.cpp \
    dlgmessage.cpp \
    ff_settingsdrv.cpp \
    ff_halldrv.cpp \
    ../CafeV4/core/printing.cpp \
    dlgface.cpp \
    ../common/qlogwindow.cpp \
    ../common/qsqldrv.cpp \
    ../common/qsystem.cpp \
    ../common/dlgconnection.cpp \
    dlgtableformovement.cpp \
    ../common/qetablewidget.cpp \
    ../common/dlginput.cpp \
    ff_discountdrv.cpp \
    dlgreports.cpp \
    ../common/dlggetpassword.cpp \
    ../common/wdtnumpad.cpp \
    dlgchangepass.cpp \
    dlgreportfilter.cpp \
    dlgdishcomment.cpp \
    dlghistory.cpp \
    ../common/qsqldb.cpp \
    dlglist.cpp \
    dlgcalcchange.cpp \
    ../CafeV4/CafeV4_Manager/logwriter.cpp \
    ../common/qqrengine.cpp \
    orderwindowdriver.cpp \
    qnet.cpp \
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
    dlgtimelimit.cpp \
    dlgsplash.cpp \
    orderdrv/od_dish.cpp \
    orderdrv/od_base.cpp \
    orderdrv/od_header.cpp \
    orderdrv/od_drv.cpp \
    orderdrv/od_flags.cpp \
    orderdrv/od_print.cpp \
    dbdriver.cpp \
    ff_correcttime.cpp \
    tableordersocket.cpp \
    taxcheckout.cpp \
    dlgremovereason.cpp \
    dlgkinoparkcall.cpp \
    logthread.cpp \
    dlg14.cpp \
    dlgpaymentjazzve.cpp \
    ../MobilePointServer/msqldatabase.cpp \
    ../MobilePointServer/mtcpserver.cpp \
    cnfmaindb.cpp \
    ../MobilePointServer/mjsonhandler.cpp \
    ../MobilePointServer/databaseresult.cpp \
    ../MobilePointServer/mtcpsocketthread.cpp \
    ../MobilePointServer/mtprintkitchen.cpp \
    ../NewTax/Src/printtaxn.cpp \
    ../MobilePointServer/pimage.cpp \
    ../MobilePointServer/pprintscene.cpp \
    ../MobilePointServer/ptextrect.cpp \
    ../MobilePointServer/QRCodeGenerator.cpp \
    ../MobilePointServer/mtfilelog.cpp \
    dlgpayment.cpp \
    elineedit.cpp \
    utils.cpp \
    cnfapp.cpp \
    mptcpsocket.cpp \
    classes.cpp

HEADERS  += \
    ../common/fastfprotocol.h \
    c5printing.h \
    configmobilewidget.h \
    dbmutexlocker.h \
    dlgapprovecorrectionmessage.h \
    dlgconfigmobile.h \
    dlgcorrection.h \
    dlgqty.h \
    dlgselecttaxreport.h \
    qelineedit.h \
    ff_user.h \
    dlgorder.h \
    ff_dishesdrv.h \
    dlgmessage.h \
    ff_settingsdrv.h \
    ff_halldrv.h \
    ../CafeV4/core/printing.h \
    dlgface.h \
    qsqldrv.h \
    ../common/qlogwindow.h \
    ../common/qsqldrv.h \
    ../common/qsystem.h \
    qsystem.h \
    ../common/dlgconnection.h \
    dlgconnection.h \
    dlgtableformovement.h \
    qetablewidget.h \
    ../common/qetablewidget.h \
    dlginput.h \
    ../common/dlginput.h \
    ff_discountdrv.h \
    dlgreports.h \
    dlggetpassword.h \
    ../common/dlggetpassword.h \
    ../common/wdtnumpad.h \
    dlgchangepass.h \
    dlgreportfilter.h \
    tableordersocket.h \
    wdtnumpad.h \
    dlgdishcomment.h \
    dlghistory.h \
    qsqldb.h \
    ../common/qsqldb.h \
    printing.h \
    dlglist.h \
    dlgcalcchange.h \
    ../CafeV4/CafeV4_Manager/logwriter.h \
    logwriter.h \
    ../common/qqrengine.h \
    orderwindowdriver.h \
    qnet.h \
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
    dlgtimelimit.h \
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
    dlg14.h \
    dlgpaymentjazzve.h \
    ../MobilePointServer/msqldatabase.h \
    ../MobilePointServer/mtcpserver.h \
    cnfmaindb.h \
    ../MobilePointServer/mjsonhandler.h \
    ../MobilePointServer/databaseresult.h \
    ../MobilePointServer/mtcpsocketthread.h \
    ../MobilePointServer/mtprintkitchen.h \
    ../NewTax/Src/printtaxn.h \
    ../MobilePointServer/pimage.h \
    ../MobilePointServer/pprintscene.h \
    ../MobilePointServer/ptextrect.h \
    ../MobilePointServer/QRCodeGenerator.h \
    ../MobilePointServer/mtfilelog.h \
    dlgpayment.h \
    elineedit.h \
    utils.h \
    cnfapp.h \
    ../LibQREncode/getopt.h \
    mptcpsocket.h \
    classes.h


FORMS    += \
    configmobilewidget.ui \
    dlgapprovecorrectionmessage.ui \
    dlgconfigmobile.ui \
    dlgcorrection.ui \
    dlgorder.ui \
    dlgmessage.ui \
    dlgface.ui \
    ../common/qlogwindow.ui \
    ../common/dlgconnection.ui \
    dlgqty.ui \
    dlgselecttaxreport.ui \
    dlgtableformovement.ui \
    ../common/dlginput.ui \
    dlgreports.ui \
    ../common/dlggetpassword.ui \
    ../common/wdtnumpad.ui \
    dlgchangepass.ui \
    dlgreportfilter.ui \
    dlgdishcomment.ui \
    dlghistory.ui \
    dlglist.ui \
    dlgcalcchange.ui \
    dlgtimelimit.ui \
    dlgsplash.ui \
    dlgremovereason.ui \
    dlgkinoparkcall.ui \
    dlg14.ui \
    dlgpaymentjazzve.ui \
    dlgpayment.ui

RESOURCES += \
    Res.qrc

ICON = res/app.ico

DEFINES += FASTF
DEFINES += _ORGANIZATION_=\\\"Jazzve\\\"
DEFINES += _APPLICATION_=\\\"FastF\\\"

LIBS += -lVersion

LIBS += -LC:/Soft/OpenSSL-Win64/lib
LIBS += -lopenssl
LIBS += -llibcrypto
LIBS += -ladvapi32
LIBS += -lwsock32






