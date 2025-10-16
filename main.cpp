#include "dlgface.h"
#include "idram.h"
#include "qsystem.h"
#include "dlgconnection.h"
#include "qsqldb.h"
#include "dlgsplash.h"
#include "cnfmaindb.h"
#include "msqldatabase.h"
#include "cnfapp.h"
#include <QTranslator>
#include <QLockFile>
#include <QFontDatabase>
#include <QFile>
#include <QApplication>
#include <QSqlDatabase>
#include <QScreen>
#include <QDesktopServices>
#include <QSslSocket>

int main(int argc, char* argv[])
{
    QApplication a(argc, argv);
#ifndef QT_DEBUG
    QStringList libPath = QCoreApplication::libraryPaths();
    libPath << a.applicationDirPath();
    libPath << a.applicationDirPath() + "/platforms";
    libPath << a.applicationDirPath() + "/sqldrivers";
    libPath << a.applicationDirPath() + "/printsupport";
    libPath << a.applicationDirPath() + "/imageformats";
    QCoreApplication::setLibraryPaths(libPath);
#endif
    LOG(QSslSocket::supportsSsl() ? "SSL: true" : "SSL: false");
    LOG("Support SSL version:  "  + QSslSocket::sslLibraryBuildVersionString());
    LOG("Supper runtime: " + QSslSocket::sslLibraryVersionString());
    QFont f = a.font();
    int fontId = QFontDatabase::addApplicationFont(a.applicationDirPath() + "/ahuni.ttf");

    if(fontId < 0) {
        f.setFamily("Arial Latarm Unicode");
        f.setPointSize(12);
        a.setFont(f);
    } else {
        QString fontFamily = QFontDatabase::applicationFontFamilies(fontId).at(0);
        f = QFontDatabase::font(fontFamily, "Normal", 11);
        a.setFont(f);
    }

    QTranslator t;

    if(t.load(":/FastF.qm")) {
        a.installTranslator(&t);
    }

    QStringList args;

    for(int i = 0; i < argc; i++) {
        args << argv[i];
    }

    auto d = new DlgSplash();

    for(const QString &s : args) {
        if(s.startsWith("/monitor")) {
            QList<QScreen*> screens = a.screens();
            int monitor = 0;
            QStringList mon = s.split("=");

            if(mon.length() == 2) {
                monitor = mon.at(1).toInt();
            }

            d->move(screens.at(monitor)->geometry().topLeft());
        }
    }

    d->show();
    a.processEvents();
    d->setText("Load system settings...");
    QSystem system(argv, a.applicationName());
    d->setText("Lock file...");
    QFile file(system.homePath() + "/lock.pid");
    file.remove();
    QLockFile lockFile(system.homePath() + "/lock.pid");

    if(!lockFile.tryLock())
        return -1;

    d->setText("Get databases...");
    QSqlDB::setDbName(QSqlDB::addDatabase(__cnfmaindb.fHost, __cnfmaindb.fDatabase, __cnfmaindb.fUser, __cnfmaindb.fPassword), MAIN);
    d->setText("Init ssl socket");
    d->setText("All done... Starting...");
    d->hide();
    delete d;
    CnfApp::init(__cnfmaindb.fHost, __cnfmaindb.fDatabase, __cnfmaindb.fUser, __cnfmaindb.fPassword, "FASTF");
    MSqlDatabase::setConnectionParams(__cnfmaindb.fHost, __cnfmaindb.fDatabase, __cnfmaindb.fUser, __cnfmaindb.fPassword);
    auto w = new DlgFace();
    auto *idram = new IDram(w);

    for(const QString &s : std::as_const(args)) {
        if(s.startsWith("/monitor")) {
            QList<QScreen*> screens = a.screens();
            int monitor = 0;
            QStringList mon = s.split("=");

            if(mon.length() == 2) {
                monitor = mon.at(1).toInt();
            }

            w->move(screens.at(monitor)->geometry().topLeft());
        }
    }

    w->setWindowIcon(QIcon(":/res/app.ico"));
    w->show();
    a.exec();
    return 0;
}
