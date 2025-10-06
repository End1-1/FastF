#include "logthread.h"
#include "dbdriver.h"
#include "qsystem.h"
#include "cnfmaindb.h"

QString LogDb;
QString LogUser;
QString LogPass;

LogThread::LogThread(QObject *parent) :
    QThread(parent)
{
    connect(this, SIGNAL(finished()), this, SLOT(deleteLater()));
}

LogThread::~LogThread()
{
    qDebug() << "LogThread quit";
}

void LogThread::logOrderThread(const QString &user, const QString &order, const QString &body, const QString &action, const QString &data)
{
    LogThread *log = new LogThread(0);
    log->fLogMode = logOrder;
    log->fUser = user;
    log->fOrder = order;
    log->fBody = body;
    log->fAction = action;
    log->fData = data;
    log->start();
}

void LogThread::logDiscountFailureThread(int user, const QString &code)
{
    LogThread *log = new LogThread(0);
    log->fLogMode = logDiscountFailure;
    log->fUser = "user";
    log->fData = code;
    log->start();
}

void LogThread::run()
{
    DbDriver d;
    d.configureDb(__cnfmaindb.fHost, __cnfmaindb.fDatabase, __cnfmaindb.fUser, __cnfmaindb.fPassword);
    d.openDB();

    switch(fLogMode) {
    case logOrder:
        d.prepare("insert into o_tr(fcomp, fuser, forder, fbody, ftr, fdata) values (:fcomp, :fuser, :forder, :fbody, :ftr, :fdata)");
        d.bindValue(":fcomp", QSystem::hostInfo());
        d.bindValue(":fuser", fUser);
        d.bindValue(":forder", fOrder);
        d.bindValue(":fbody", fBody);
        d.bindValue(":ftr", fAction);
        d.bindValue(":fdata", fData);
        break;

    case logDiscountFailure:
        d.prepare("insert into o_disc_code_failure (f_user, f_code) values (:f_user, :f_code)");
        d.bindValue(":f_user", fUser);
        d.bindValue(":f_code", fData);
        break;
    }

    d.execSQL();
    d.closeDB();
    quit();
}
