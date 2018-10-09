#include "od_base.h"
#include "cnfmaindb.h"

OD_Base::OD_Base()
{
    m_saved = true;
}

void OD_Base::configureDB(const QString &dbHost, const QString &dbPath, const QString &dbUser, const QString &dbPass)
{
    m_dbDrv.configureDb(dbHost, dbPath, dbUser, dbPass);
}

void OD_Base::configureOtherDB(QSqlDatabase &db)
{
    db.setDatabaseName(m_dbDrv.m_db.databaseName());
    db.setPort(m_dbDrv.m_db.port());
    db.setUserName(m_dbDrv.m_db.userName());
    db.setPassword(m_dbDrv.m_db.password());
    db.setConnectOptions(m_dbDrv.m_db.connectOptions());
}

bool OD_Base::openDB()
{
    return m_dbDrv.openDB();
}

void OD_Base::closeDB()
{
    m_dbDrv.closeDB();
}

bool OD_Base::next()
{
    return m_dbDrv.next();
}

int OD_Base::v_int(int index)
{
    return m_dbDrv.v_int(index);
}

QString OD_Base::v_str(int index)
{
    return m_dbDrv.v_str(index);
}

double OD_Base::v_dbl(int index)
{
    return m_dbDrv.v_dbl(index);
}

QDateTime OD_Base::v_dateTime(int index)
{
    return m_dbDrv.v_dateTime(index);
}

QDate OD_Base::v_date(int index)
{
    return m_dbDrv.v_date(index);
}

bool OD_Base::prepare(const QString &sql)
{
    return m_dbDrv.prepare(sql);
}

bool OD_Base::execSQL()
{
    return m_dbDrv.execSQL();
}

bool OD_Base::bindValue(const QString &name, const QVariant &value)
{
    return m_dbDrv.bindValue(name, value);
}

int OD_Base::genId(const QString &name)
{
    return m_dbDrv.genId(name);
}

