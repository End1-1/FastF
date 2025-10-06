#include "qsqldb.h"
#include "qlogwindow.h"
#include <QSqlError>

QString dbNumber = "0";

QMap<QString, QSqlDatabase> QSqlDB::m_db;
QMap<QString, QString> QSqlDB::m_dbNames;

QString getDbNumber()
{
    dbNumber = QString::number(dbNumber.toInt() + 1);
    return dbNumber;
}

QSqlDB::QSqlDB()
{
}

QString QSqlDB::addDatabase(const QString &dbHost, const QString &dbPath, const QString &dbUser, const QString &dbPass, bool connect)
{
    QString number = getDbNumber();
    m_db[number] = QSqlDatabase::addDatabase("QIBASE", number + "OLD");
    m_db[number].setHostName(dbHost);
    m_db[number].setDatabaseName(dbPath);
    m_db[number].setUserName(dbUser);
    m_db[number].setPassword(dbPass);
    m_db[number].setConnectOptions("lc_ctype=UTF-8");
    if (connect)
        if (!m_db[number].open()) {
            QLogWindow::writeLog(m_db[number].lastError().driverText());
            return "0";
        }
    return number;
}

void QSqlDB::setDbName(const QString &number, const QString &name)
{
    m_dbNames[name] = number;
}

void QSqlDB::removeDatabase(const QString &number)
{
    m_db[number].commit();
    m_db[number].close();
    m_db.remove(number);
    m_db.remove(m_dbNames.key(number));
    QSqlDatabase::removeDatabase(number);
}

void QSqlDB::removeDatabaseByName(const QString &name)
{
    QString number = m_dbNames[name];
    removeDatabase(number + "OLD");
}

QSqlDatabase &QSqlDB::dbByNumber(const QString &number)
{
    return m_db[number];
}

QSqlDatabase &QSqlDB::dbByName(const QString &name)
{
    if (!m_dbNames.contains(name))
        setDbName(addDatabase("", "", "", "", false), name);
    return m_db[m_dbNames[name]];
}
