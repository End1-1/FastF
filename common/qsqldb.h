#ifndef QSQLDB_H
#define QSQLDB_H

#include <QSqlDatabase>
#include <QMap>

#define MAIN "main"

extern QString dbNumber;
QString getDbNumber();

class QSqlDB
{
private:
    static QMap<QString, QSqlDatabase> m_db;
    static QMap<QString, QString> m_dbNames;

public:
    QSqlDB();
    static QString addDatabase(const QString &dbHost, const QString &dbPath, const QString &dbUser, const QString &dbPass, bool connect = true);
    static void setDbName(const QString &number, const QString &name);
    static void removeDatabase(const QString &number);
    static void removeDatabaseByName(const QString &name);
    static QSqlDatabase &dbByNumber(const QString &number);
    static QSqlDatabase &dbByName(const QString &name);
};

#endif // QSQLDB_H
