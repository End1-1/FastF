#include "qbasesqldriver.h"

QBaseSqlDriver::QBaseSqlDriver()
{
    m_sqlDriver = new QSqlDrv("admin", "clientdb");
}

QBaseSqlDriver::~QBaseSqlDriver()
{
    delete m_sqlDriver;
    qDeleteAll(m_fields);
}


int QBaseSqlDriver::fieldIndexByName(const QString &fieldName)
{
    for (int i = 0, count = m_fields.count(); i < count; i++)
        if (m_fields[i]->name == fieldName)
            return i;

    return -1;
}

int QBaseSqlDriver::fieldIndexInGrid(const QString &fieldName)
{
    int result = -1;
    for (int i = 0, count = m_fields.count(); i < count; i++) {
        if (m_fields.at(i)->isIncluded())
            result++;
        if (m_fields[i]->name == fieldName)
            return result;
    }

    return -1;
}

SqlField *QBaseSqlDriver::fieldByName(const QString &fieldName)
{
    int index = fieldIndexByName(fieldName);
    if (index < 0)
        return 0;
    return m_fields.at(index);
}

QString QBaseSqlDriver::whereClause()
{
    return "";
}

