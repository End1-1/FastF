#include "dlgface.h"
#include <QApplication>

#include "qsqldrv.h"
#include "../CafeV4/core/printing.h"
#include "qsystem.h"
#include "dlgconnection.h"
#include "qsqldb.h"
#include <QTranslator>
#include "ff_settingsdrv.h"
#include <QLockFile>
#include "dlgsplash.h"
#include "ff_messanger.h"
#include "logthread.h"
#include <QFontDatabase>
#include "mtcpserver.h"
#include "mudpserver.h"
#include "msqldatabase.h"
#include "cnfmaindb.h"
#include "cnfapp.h"
#include "mjsonhandler.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

#ifndef QT_DEBUG
    QStringList libPath;
    libPath << "./";
    libPath << "./platforms";
    libPath << "./sqldrivers";
    QCoreApplication::setLibraryPaths(libPath);
#endif

    QFont f = a.font();
    QFontDatabase fdb;
    int fontId = fdb.addApplicationFont("./ahuni.ttf");
    if (fontId < 0) {
        f.setFamily("Arial Latarm Unicode");
        f.setPointSize(12);
        a.setFont(f);
    } else {
        QString fontFamily = fdb.applicationFontFamilies(fontId).at(0);
        f = fdb.font(fontFamily, "Normal", 11);
        a.setFont(f);
    }

    OD_Print::mfFont.setFamily(f.family());
    OD_Print::mfFont.setPointSize(f.pointSize());

    QTranslator t;
    t.load(":/FastF.qm");
    a.installTranslator(&t);

    DlgSplash *d = new DlgSplash();
    d->show();
    a.processEvents();

    d->setText("Load system settings...");
    QSystem system(argv, a.applicationName());

    d->setText("Lock file...");
    QFile file(system.homePath() + "/lock.pid");
    file.remove();

    QLockFile lockFile(system.homePath() + "/lock.pid");
    if (!lockFile.tryLock())
        return -1;

    d->setText("Get databases...");
    QSqlDB::setDbName(QSqlDB::addDatabase(__cnfmaindb.fHost, __cnfmaindb.fDatabase, __cnfmaindb.fUser, __cnfmaindb.fPassword), MAIN);

    d->setText("Get printers...");
    ___printerInfo = new PrinterInfo();
    FF_Messanger::updatePrintersList();

    d->setText("All done... Starting...");
    d->hide();
    delete d;

    CnfApp::init(__cnfmaindb.fHost, __cnfmaindb.fDatabase, __cnfmaindb.fUser, __cnfmaindb.fPassword, "FASTF");
    MJsonHandler::fServerIp = __cnfmaindb.fServerIP;
    if (__cnfmaindb.fServerMode.toInt() > 0) {
        MSqlDatabase::setConnectionParams(__cnfmaindb.fHost, __cnfmaindb.fDatabase, __cnfmaindb.fUser, __cnfmaindb.fPassword);
        MTcpServer *s = new MTcpServer(nullptr);
        s->start();
        MUdpServer *su = new MUdpServer(nullptr);
        su->start();
    }

    DlgFace w;
    w.setWindowIcon(QIcon(":/res/app.ico"));
    w.show();

    a.exec();

    return 0;
}
