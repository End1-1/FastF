#ifndef QSQLDRV_H
#define QSQLDRV_H

#include <QSqlDatabase>
#include <QObject>
#include <QVariant>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlRecord>
#include <QThread>
#include <QMutex>
#include <QHostInfo>
#include <QTableWidget>
#include "qsystem.h"

#define dts(value) QString::number(value, 'f', 2)
#define its(value) QString::number(value)
#define datets(value) value.toString(DATE_FORMAT)
#define TABLE_HISTORY "SALE_HISTORY"

extern QHostInfo ___hostInfo;
extern QMutex ___dbMutex;

QString getHostName();
QString getHostUsername();
QString getLockName();

class QSqlLog
{
    static void parseQueryAndTable(const QString &data, QString &query, QString &tableName);
public:
    static void write(const QString &tableName, const QString &query, const QString &data,
                      const QString &userName, int msec, const QString &note = "");
};

class QSqlDrv : public QObject
{
    Q_OBJECT
private:
    int m_fieldNum;
    QMap<QString, QVariant> m_bindValues;
    void getFields(QMap<QString, int>& fieldsMap);

protected:
    QVariant m_empty;
    QString m_username;
public:

    bool openDB();
    void log(const QString &text, bool showwindow = true);

public:
    QString m_sql;
    QSqlQuery* m_query;
    QSqlDatabase m_db;
    QSqlDrv(const QString &username, const QString &dbName);
    void setDB(const QString &dbName);
    ~QSqlDrv();
    void close();
    void rollback();
    bool prepare();
    bool prepare(const QString &sql);
    void bind(const QString &key, QVariant value);
    bool execSQL();
    bool execSQL(const QString &sql);
    int insert(const QString &table, const QMap<QString, QVariant>& values);
    bool update(const QString &table, const QMap<QString, QVariant>& values, int recordId);
    void fillData(QMap<QString, int>& fieldsMap, QList<QList<QVariant> >& data);
    void fillTableWidget(QMap<QString, int> fieldsMap, QTableWidget *t);
    void fillTableWidget2(QTableWidget *t, int textField, int dataField);
    void fillTableWidget3(QTableWidget *t);
    int genId(const QString &name);
    QVariant prop(int index, const QString &name, const QMap<QString, int>& fieldMap, const QList<QList<QVariant> >& data) const;

    bool next();
    QVariant val();
    int valInt(const QString &fieldName);
    QString valStr(const QString &fieldName);
    QDateTime valDateTime(const QString &fieldName);
    QDate valDate(const QString &fieldName);
    float valFloat(const QString &fieldName);
};

#endif // QSQLDRV_H
