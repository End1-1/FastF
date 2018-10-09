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

void LogThread::logOrderThread(int user, const QString &order, const QString &action, const QString &data)
{
    LogThread *log = new LogThread(0);
    log->fLogMode = logOrder;
    log->fUser = user;
    log->fOrder = order;
    log->fAction = action;
    log->fData = data;
    log->start();
}

void LogThread::logDiscountFailureThread(int user, const QString &code)
{
    LogThread *log = new LogThread(0);
    log->fLogMode = logDiscountFailure;
    log->fUser = user;
    log->fData = code;
    log->start();
}

void LogThread::run()
{
    DbDriver d;
    d.configureDb(__cnfmaindb.fHost, __cnfmaindb.fDatabase, __cnfmaindb.fUser, __cnfmaindb.fPassword);
    d.openDB();
    QString query;
    switch (fLogMode) {
    case logOrder:
        query = QString("insert into o_log (f_user, f_order, f_action, f_data, f_comp) values (%1, '%2', '%3', '%4', '%5')")
                .arg(fUser)
                .arg(fOrder)
                .arg(fAction)
                .arg(fData)
                .arg(QSystem::hostInfo());
        break;
    case logDiscountFailure:
        query = QString("insert into o_disc_code_failure (f_user, f_code) values (%1, '%2')")
                .arg(fUser)
                .arg(fData);
        break;
    }
    d.execSQL(query);
    d.closeDB();
    quit();
}
