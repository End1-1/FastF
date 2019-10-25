#include "kitreminderthread.h"
#include "dbdriver.h"
#include "cnfmaindb.h"
#include <QTcpSocket>

KitReminderThread::KitReminderThread(const QStringList &list, QObject *parent) :
    QThread(parent)
{
    fData = list;
    connect(this, SIGNAL(finished()), this, SLOT(deleteLater()));
}

void KitReminderThread::run()
{
    DbDriver db;
    db.configureDb(__cnfmaindb.fHost, __cnfmaindb.fDatabase, __cnfmaindb.fUser, __cnfmaindb.fPassword);
    db.prepare("select faddress, fport from sys_reminders where fid=:fid");
    db.bindValue(":fid", 1);
    db.execSQL();
    QTcpSocket s;
    if (db.next()) {
        s.connectToHost(db.v_str(0), db.v_int(1));
    } else {
        return;
    }
    if (!s.waitForConnected(10000)) {
        return;
    }
    for (const QString &l: fData) {
        s.write(l.toUtf8());
        s.flush();
        s.waitForBytesWritten();
    }
    s.disconnectFromHost();
}
