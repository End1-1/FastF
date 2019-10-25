#include "dbdriver.h"
#include "dbmutexlocker.h"
#include <QMapIterator>
#include <QMutex>

int DbDriver::m_number = 100000;
QStringList m_lastQuery;

void DbDriver::log(const QString &message)
{
    QString finalMessage = QDateTime::currentDateTime().toString() + ": " + message;
#ifdef QT_DEBUG
    m_lastQuery << finalMessage;
    qDebug() << message;
#else
    Q_UNUSED(finalMessage)
#endif
    if (m_errorFlag)
        emit errorMsg(message);
}

DbDriver::DbDriver()
{
    QMutexLocker ml(&__mxDb);
    m_dbnumber = m_number++;
    m_db = QSqlDatabase::addDatabase("QIBASE", QString::number(m_dbnumber));
    m_query = nullptr;
}

DbDriver::~DbDriver()
{
    if (m_db.isOpen()) {
        m_db.close();
    }
    m_db = QSqlDatabase::addDatabase("QIBASE");
    if (m_query)
        delete m_query;
    QSqlDatabase::removeDatabase(QString::number(m_dbnumber));
}

void DbDriver::configureDb(const QString &dbHost, const QString &dbPath, const QString &dbUser, const QString &dbPass)
{
    m_db.setHostName(dbHost);
    m_db.setDatabaseName(dbPath);
    m_db.setUserName(dbUser);
    m_db.setPassword(dbPass);
}

bool DbDriver::openDB()
{
    if (m_db.isOpen())
        closeDB();
    if (m_db.open()) {
        m_errorFlag = false;
        m_db.transaction();
        m_query = new QSqlQuery(m_db);
        return true;
    } else {
        m_errorFlag = true;
        log(m_db.lastError().databaseText());
        return false;
    }
}

void DbDriver::closeDB()
{
    if (m_errorFlag)
        m_db.rollback();
    else
        m_db.commit();
    delete m_query;
    m_query = nullptr;
    m_db.close();
}

bool DbDriver::prepare(const QString &sql)
{
    if (!m_query) {
        if (!openDB()) {
            log(m_db.lastError().databaseText());
            return false;
        }
    }
    if (m_query->prepare(sql)) {
        return true;
    } else {
        m_errorFlag = true;
        log(sql);
        log(m_query->lastError().databaseText());
        closeDB();
        return false;
    }
}

bool DbDriver::bindValue(const QString &name, const QVariant &value)
{
    if (!m_query) {
        m_errorFlag = true;
        log("Cannot bind value. Database is not opened");
        return false;
    }
    m_query->bindValue(name, value);
    return true;
}

bool DbDriver::execSQL()
{
    if (!m_query)
        return false;
    if (m_query->exec()) {
        log(lastQuery());
        return true;
    } else {
        m_errorFlag = true;
        log(lastQuery());
        log(m_query->lastError().databaseText());
        closeDB();
        return false;
    }
}

bool DbDriver::execSQL(const QString &sql)
{
    if (prepare(sql)) {
        return execSQL();
    } else {
        return false;
    }
}

bool DbDriver::next()
{
    return m_query->next();
}

QSqlRecord DbDriver::record()
{
    if (!m_query)
        return QSqlRecord();
    return m_query->record();
}

int DbDriver::genId(const QString &name)
{
    if (!m_query) {
        return 0;
    }
    if (execSQL("select gen_id(" + name + ",1) from rdb$database")) {
        next();
        return v_int(0);
    } else {
        return 0;
    }
}

QVariant DbDriver::v(int index)
{
    return m_query->value(index);
}

QString DbDriver::v_str(int index)
{
    return m_query->value(index).toString();
}

int DbDriver::v_int(int index)
{
    return m_query->value(index).toInt();
}

double DbDriver::v_dbl(int index)
{
    return m_query->value(index).toDouble();
}

QDateTime DbDriver::v_dateTime(int index)
{
    return m_query->value(index).toDateTime();
}

QDate DbDriver::v_date(int index)
{
    return m_query->value(index).toDate();
}

QString DbDriver::lastQuery()
{
    QString sql = m_query->lastQuery();
    QMapIterator<QString, QVariant> it(m_query->boundValues());
    while (it.hasNext()) {
        it.next();
        QVariant value = it.value();
        switch (it.value().type()) {
        case QVariant::String:
            value = QString("'%1'").arg(value.toString().replace("'", "''"));
            break;
        case QVariant::Date:
            value = QString("'%1'").arg(value.toDate().toString(DATE_FORMAT));
            break;
        case QVariant::DateTime:
            value = QString("'%1'").arg(value.toDateTime().toString(DATETIME_FORMAT));
            break;
        case QVariant::Double:
            value = QString("%1").arg(value.toDouble());
            break;
        case QVariant::Int:
            value = QString("%1").arg(value.toInt());
            break;
        default:
            break;
        }
        sql.replace(it.key(), value.toString());
    }
    return sql;
}

void DbDriver::commit()
{
    m_db.commit();
}

