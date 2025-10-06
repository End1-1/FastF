#include "database.h"
#include "logwriter.h"
#include "cnfmaindb.h"
#include <QDateTime>
#include <QSqlRecord>
#include <QSettings>
#include <QUuid>

QMutex Database::fMutex;
int Database::fDatabaseCounter = 0;

Database::Database() :
    Database("QIBASE")
{
}

Database::Database(Database &other) :
    Database("QIBASE")
{
    open(other.fSqlDatabase.hostName(), other.fSqlDatabase.databaseName(), other.fSqlDatabase.userName(), other.fSqlDatabase.password());
}

Database::Database(const QString &driverName)
{
    fQuery = nullptr;
    fDatabaseDriver = driverName;
    QMutexLocker ml(&fMutex);
    fDatabaseNumber = QString::number(++fDatabaseCounter);
}

Database::~Database()
{
    if(fQuery) {
        fQuery->finish();
        delete fQuery;
    }

    fSqlDatabase.close();
    fSqlDatabase = QSqlDatabase::addDatabase(fDatabaseDriver);
    QSqlDatabase::removeDatabase(fDatabaseNumber);
}

bool Database::open()
{
    return open(__cnfmaindb.fHost,
                __cnfmaindb.fDatabase,
                __cnfmaindb.fUser,
                __cnfmaindb.fPassword);
}

bool Database::open(const QString &configFile)
{
    QSettings s(configFile, QSettings::IniFormat);
    return open(s.value("db/host").toString(), s.value("db/schema").toString(), s.value("db/username").toString(), s.value("db/password").toString());
}

bool Database::open(const QString &host, const QString &schema, const QString &username, const QString &password)
{
    fSqlDatabase = QSqlDatabase::addDatabase(fDatabaseDriver, fDatabaseNumber);
    fSqlDatabase.setHostName(host);
    fSqlDatabase.setDatabaseName(schema);
    fSqlDatabase.setUserName(username);
    fSqlDatabase.setPassword(password);

    if(!fSqlDatabase.open()) {
        LogWriter::write(LogWriterLevel::errors, "open database", fSqlDatabase.lastError().databaseText());
        return false;
    }

    fQuery = new QSqlQuery(fSqlDatabase);
    return true;
}

bool Database::open(const QJsonObject &o)
{
    return open(o["host"].toString(), o["schema"].toString(), o["username"].toString(), o["password"].toString());
}

bool Database::startTransaction()
{
    if(!fSqlDatabase.isOpen()) {
        if(!open()) {
            return false;
        }
    }

    return fSqlDatabase.transaction();
}

bool Database::commit()
{
    return fSqlDatabase.commit();
}

bool Database::rollback()
{
    return fSqlDatabase.rollback();
}

bool Database::exec(const QString &query)
{
    if(!fSqlDatabase.isOpen()) {
        if(!open()) {
            return false;
        }
    }

    if(!fQuery) {
        fQuery = new QSqlQuery(fSqlDatabase);
    }

    if(!fQuery->prepare(query)) {
        LogWriter::write(LogWriterLevel::errors, fSqlDatabase.databaseName(), fQuery->lastError().databaseText());
        return false;
    }

    QStringList keys = fBindValues.keys();

    for(const QString &key : qAsConst(keys)) {
        fQuery->bindValue(key, fBindValues[key]);
    }

    fBindValues.clear();
#ifdef QT_DEBUG
    LogWriter::write(LogWriterLevel::errors, fSqlDatabase.databaseName(), query);
#endif

    if(!fQuery->exec()) {
        LogWriter::write(LogWriterLevel::errors, fSqlDatabase.databaseName(), fQuery->lastError().databaseText());
        return false;
    }

    bool isSelect = fQuery->isSelect();

    if(isSelect) {
        fColumnsNames.clear();
        QSqlRecord rec = fQuery->record();

        for(int i = 0; i < rec.count(); i++) {
            fColumnsNames[rec.fieldName(i).toLower()] = i;
            fColumnsIndexes[i] = rec.fieldName(i).toLower();
        }
    }

    return true;
}

bool Database::execDirect(const QString &query)
{
    if(!fSqlDatabase.isOpen()) {
        if(!open()) {
            return false;
        }
    }

#ifdef QT_DEBUG
    LogWriter::write(LogWriterLevel::errors, fSqlDatabase.databaseName(), query);
#endif

    if(!fQuery->exec(query)) {
        LogWriter::write(LogWriterLevel::errors, "", fQuery->lastError().databaseText());
        LogWriter::write(LogWriterLevel::errors, "", query);
        return false;
    }

    return true;
}

bool Database::insert(const QString &table)
{
    QString sql = "insert into " + table;
    QString k, v;
    bool first = true;

    for(QMap<QString, QVariant>::const_iterator it = fBindValues.constBegin(); it != fBindValues.constEnd(); it++) {
        if(first) {
            first = false;
        } else {
            k += ",";
            v += ",";
        }

        k += QString(it.key()).remove(0, 1);
        v += it.key();
    }

    sql += QString("(%1) values (%2)").arg(k, v);
    return exec(sql);
}

bool Database::insert(const QString &table, int& id)
{
    id = 0;

    if(insert(table)) {
        fQuery->exec("select last_insert_id()");

        if(fQuery->next()) {
            id = fQuery->value(0).toInt();
        }
    }

    return id > 0;
}

bool Database::update(const QString &table)
{
    return update(table, "fid", fBindValues[":fid"]);
}

bool Database::update(const QString &table, const QString &field, const QVariant &value)
{
    fBindValues[":" + field] = value;
    QString sql = "update " + table + " set ";
    bool first = true;

    for(QMap<QString, QVariant>::const_iterator it = fBindValues.constBegin(); it != fBindValues.constEnd(); it++) {
        if(first) {
            first = false;
        } else {
            sql += ",";
        }

        QString f = it.key();
        sql += f.remove(0, 1) + "=" + it.key();
    }

    sql += QString(" where %1=:%1").arg(field);
    return exec(sql);
}

bool Database::deleteFromTable(const QString &table, const QString &field, const QVariant &value)
{
    fBindValues[":" + field] = value;
    return exec(QString("delete from %1 where %2=:%2").arg(table, field));
}

QString Database::uuid()
{
    return QUuid::createUuid().toString().replace("{", "").replace("}", "");
}

void Database::setBindValues(const QMap<QString, QVariant>& v)
{
    fBindValues = v;
}

QMap<QString, QVariant> Database::getBindValues()
{
    QMap<QString, QVariant> b;

    for(QHash<QString, int>::const_iterator it = fColumnsNames.constBegin(); it != fColumnsNames.constEnd(); it++) {
        b[":" + it.key()] = fQuery->value(fColumnsNames[it.key()]);
    }

    return b;
}

void Database::close()
{
    if(fQuery) {
        fQuery->clear();
    }

    fSqlDatabase.close();
}

QVariant& Database::operator[](const QString &name)
{
    return fBindValues[name];
}

QString Database::lastDbError() const
{
    if(fQuery) {
        return fQuery->lastError().databaseText();
    } else {
        return fSqlDatabase.lastError().databaseText();
    }
}

int Database::genFBID(const QString &name)
{
    if(exec("select gen_id(" + name + ",1) as fid from rdb$database")) {
        if(next()) {
            return integer("fid");
        }
    }

    return 0;
}

void Database::rowToMap(QMap<QString, QVariant>& map)
{
    for(int i = 0; i < columnCount(); i++) {
        map[columnName(i)] = value(i);
    }
}

QStringList Database::params()
{
    QStringList p;
    p.append(fSqlDatabase.hostName());
    p.append(fSqlDatabase.databaseName());
    p.append(fSqlDatabase.userName());
    p.append(fSqlDatabase.password());
    return p;
}
