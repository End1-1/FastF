#include "qsqldrv.h"
#include "cnfmaindb.h"
#include "qsqldb.h"
#include "logwriter.h"
#include <windows.h>
#include <QMessageBox>
#include <QDateTime>
#include <QHeaderView>
#include <QTime>
#include <QFile>

QHostInfo ___hostInfo;
QMutex ___dbMutex;

void QSqlDrv::getFields(QMap<QString, int>& fieldsMap)
{
    fieldsMap.clear();

    for(int i = 0, count = m_query->record().count(); i < count; i++)
        fieldsMap[m_query->record().fieldName(i)] = i;
}

bool QSqlDrv::openDB()
{
    if(m_db.isOpen()) {
        if(!m_query)
            m_query = new QSqlQuery(m_db);

        return true;
    }

    if(!m_db.open()) {
        log("[QSqlDrv] " + m_db.lastError().databaseText());
        return false;
    }

    if(m_query)
        delete m_query;

    // m_db.transaction();
    m_query = new QSqlQuery(m_db);
    return true;
}

void QSqlDrv::log(const QString &text, bool showwindow)
{
#ifdef QT_DEBUG
    qDebug() << text;
#endif

    if(showwindow)
        QMessageBox::critical(0, "Error", text);
}

void QSqlDrv::fillData(QMap<QString, int>& fieldsMap, QList<QList<QVariant> >& data)
{
    getFields(fieldsMap);
    data.clear();

    while(m_query->next()) {
        QList<QVariant> record;

        for(int i = 0; i < fieldsMap.count(); i++)
            record.append(m_query->value(i));

        data.append(record);
    }
}

void QSqlDrv::fillTableWidget(QMap<QString, int> fieldsMap, QTableWidget *t)
{
    getFields(fieldsMap);
    t->clear();
    t->setColumnCount(fieldsMap.count());
    QStringList captions(fieldsMap.count());

    for(QMap<QString, int>::const_iterator it = fieldsMap.constBegin(); it != fieldsMap.constEnd(); it++)
        captions[it.value()] = it.key();

    t->setHorizontalHeaderLabels(captions);
    int r = 0, colCount = fieldsMap.count(), rows = 500;
    t->setRowCount(rows);

    while(m_query->next()) {
        for(int i = 0; i < colCount; i++) {
            switch(m_query->value(i).type()) {
            case QVariant::DateTime:
                t->setItem(r, i, new QTableWidgetItem(m_query->value(i).toDateTime().toString(DATETIME_FORMAT)));
                break;

            case QVariant::Date:
                t->setItem(r, i, new QTableWidgetItem(m_query->value(i).toDate().toString(DATE_FORMAT)));
                break;

            default:
                t->setItem(r, i, new QTableWidgetItem(m_query->value(i).toString()));
                break;
            }
        }

        r++;

        if(r >= rows) {
            rows += 500;
            t->setRowCount(rows);
        }
    }

    t->setRowCount(r);
}

void QSqlDrv::fillTableWidget2(QTableWidget *t, int textField, int dataField)
{
    int colCount = (t->width() - 5) / t->horizontalHeader()->defaultSectionSize();
    t->clear();
    t->setColumnCount(colCount);
    int c = 0, r = 0, rows = 500;
    t->setRowCount(rows);

    while(m_query->next()) {
        if(c > colCount - 1) {
            c = 0;
            r++;
        }

        QTableWidgetItem *item = new QTableWidgetItem();
        item->setData(Qt::UserRole, m_query->value(dataField));
        item->setData(Qt::DisplayRole, m_query->value(textField));
        t->setItem(r, c++, item);

        if(r >= rows) {
            rows += 500;
            t->setRowCount(rows);
        }
    }

    t->setRowCount(r + 1);
}

void QSqlDrv::fillTableWidget3(QTableWidget *t)
{
    t->clearContents();
    int r = 0, rows = 500;
    t->setRowCount(rows);
    int colCount = m_query->record().count();

    while(m_query->next()) {
        for(int i = 0; i < colCount; i++)
            t->setItem(r, i, new QTableWidgetItem(m_query->value(i).toString()));

        if(++r >= rows) {
            rows += 500;
            t->setRowCount(rows);
        }
    }

    t->setRowCount(r);
}

int QSqlDrv::genId(const QString &name)
{
    QElapsedTimer t;
    t.start();
    bool dbState = m_db.isOpen();

    if(!dbState)
        m_db.open();

    QSqlQuery q(m_db);
    q.exec("select gen_id(" + name + ", 1) from rdb$database");
    q.next();
    int id = q.value(0).toInt();

    if(!dbState)
        m_db.close();

    QSqlLog::write(name, "", q.lastQuery(), m_username, t.elapsed());
    return id;
}

QVariant QSqlDrv::prop(int index, const QString &name, const QMap<QString, int>& fieldMap, const QList<QList<QVariant> >& data) const
{
    if(index > data.count() - 1)
        return m_empty;

    return data.at(index).at(fieldMap[name]);
}

bool QSqlDrv::next()
{
    m_fieldNum = -1;
    return m_query->next();
}

QVariant QSqlDrv::val()
{
    return m_query->value(++m_fieldNum);
}

int QSqlDrv::valInt(const QString &fieldName)
{
    return m_query->value(fieldName).toInt();
}

QString QSqlDrv::valStr(const QString &fieldName)
{
    return m_query->value(fieldName).toString();
}

QDateTime QSqlDrv::valDateTime(const QString &fieldName)
{
    return m_query->value(fieldName).toDateTime();
}

QDate QSqlDrv::valDate(const QString &fieldName)
{
    return m_query->value(fieldName).toDate();
}

float QSqlDrv::valFloat(const QString &fieldName)
{
    return m_query->value(fieldName).toDouble();
}

void QSqlDrv::close()
{
    m_query->finish();
    delete m_query;
    m_query = 0;
    m_db.commit();
    m_db.close();
}

void QSqlDrv::rollback()
{
    m_db.rollback();
}

bool QSqlDrv::prepare()
{
    if(!openDB())
        return false;

    m_bindValues.clear();

    if(!m_query->prepare(m_sql)) {
        m_db.rollback();
        QString errorText = m_query->lastError().databaseText();
        log(errorText + " --- " + m_sql);
        return false;
    }

    return true;
}

bool QSqlDrv::prepare(const QString &sql)
{
    m_sql = sql;
    return prepare();
}

void QSqlDrv::bind(const QString &key, QVariant value)
{
    m_bindValues[key] = value;
    m_query->bindValue(key, value);
}

bool QSqlDrv::execSQL()
{
    QElapsedTimer t;
    t.start();

    if(!openDB())
        return false;

#ifdef QT_DEBUG
    LogWriter::write(LogWriterLevel::errors, "qsqldrv", m_sql);
#endif

    if(!m_query->exec()) {
        log("Query error:\r\n" + m_query->lastError().databaseText());
        m_db.rollback();
        return false;
    }

    m_fieldNum = -1;
    return true;
}

bool QSqlDrv::execSQL(const QString &sql)
{
    QElapsedTimer t;
    t.start();

    if(!prepare(sql))
        return false;

    if(!execSQL())
        return false;

    return true;
}

int QSqlDrv::insert(const QString &table, const QMap<QString, QVariant>& values)
{
    int id = 0;
    m_sql = "insert into " + table + " (";
    QString fieldsNames, fieldsValues;

    for(QMap<QString, QVariant>::const_iterator it = values.begin(); it != values.end(); it++) {
        fieldsNames += it.key() + ",";
        fieldsValues += ":" + it.key() + ",";
    }

    fieldsNames.remove(fieldsNames.length() - 1, 1);
    fieldsValues.remove(fieldsValues.length() - 1, 1);
    //m_sql += fieldsNames + ") values (" + fieldsValues + ") returning id";
    m_sql += fieldsNames + ") values (" + fieldsValues + ")";

    if(!prepare())
        return 0;

    for(QMap<QString, QVariant>::const_iterator it = values.begin(); it != values.end(); it++)
        m_query->bindValue(":" + it.key(), it.value());

    if(!execSQL())
        return 0;

    if(m_query->next())
        id = m_query->value(0).toInt();

    return id;
}

bool QSqlDrv::update(const QString &table, const QMap<QString, QVariant>& values, int recordId)
{
    m_sql = "update " + table + " set ";

    for(QMap<QString, QVariant>::const_iterator it = values.begin(); it != values.end(); it++)
        m_sql += it.key() + "=:" + it.key() + ",";

    m_sql = m_sql.remove(m_sql.length() - 1, 1) + " where id=:id";

    if(!prepare())
        return false;

    for(QMap<QString, QVariant>::const_iterator it = values.begin(); it != values.end(); it++)
        m_query->bindValue(":" + it.key(), it.value());

    m_query->bindValue(":id", recordId);

    if(!execSQL())
        return false;

    return true;
}

QSqlDrv::QSqlDrv(const QString &username, const QString &dbName)
{
    m_username = username;
    m_db = QSqlDB::dbByName(dbName);
    m_query = new QSqlQuery(m_db);
}

void QSqlDrv::setDB(const QString &dbName)
{
    m_db = QSqlDB::dbByName(dbName);

    if(m_query)
        delete m_query;

    m_query = new QSqlQuery(m_db);
}

QSqlDrv::~QSqlDrv()
{
    m_db.commit();
    delete m_query;
}

QString getHostName()
{
    return ___hostInfo.localHostName();
}

QString getHostUsername()
{
    QString name = getenv("USER");

    if(name.isEmpty()) {
        name = getenv("USERNAME");
    }

    return  name;
}

QString getLockName()
{
    return getHostName() + ":" + getHostUsername();
}

void QSqlLog::parseQueryAndTable(const QString &data, QString &query, QString &tableName)
{
    query = data.left(6);

    if(query == "select") {
        tableName = "";
    } else if(query == "insert") {
        int l = QString("insert into ").length();
        int r = data.indexOf(" ", l + 1);
        tableName = data.mid(l, r - l);
    } else if(query == "update") {
        int l = QString("update ").length();
        int r = data.indexOf(" ", l + 1);
        tableName = data.mid(l, r - l);
    } else if(query ==  "delete") {
        int l = QString("delete from ").length();
        int r = data.indexOf(" ", l + 1);
        tableName = data.mid(l, r - l);
    }
}

void QSqlLog::write(const QString &tableName, const QString &query, const QString &data,
                    const QString &userName, int msec, const QString &note)
{
    QString t, q;

    if(!query.length())
        parseQueryAndTable(data, q, t);
    else {
        q = query;
        t = tableName;
    }

    QString logStr = __cnfmaindb.fHost + ":" + __cnfmaindb.fDatabase;
    logStr += QDateTime::currentDateTime().toString(DATETIME_FORMAT) + "\t";
    logStr += t + "\t";
    logStr += q + "\t";
    logStr += data + "\t";
    logStr += userName + "\t";
    logStr += getLockName() + "\t";
    logStr += QString::number(msec) + "\t";
    logStr += note + "\t";
#ifdef QT_DEBUG
    QFile f(QSystem::homePath() + "sqllog.txt");

    if(f.open(QIODevice::Append)) {
        f.write(logStr.toUtf8());
        f.write("\r\n");
        f.close();
    }

#endif
}
