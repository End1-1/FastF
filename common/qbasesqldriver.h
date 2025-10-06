#ifndef QBASESQLDRIVER_H
#define QBASESQLDRIVER_H

#include "SqlField.h"
#include "qsqldrv.h"



class QBaseSqlDriver
{
public:
    QString m_sqlString;
    QSqlDrv *m_sqlDriver;
    QBaseSqlDriver();
    ~QBaseSqlDriver();

    QList<SqlField *> m_fields;
    int fieldIndexByName(const QString &fieldName);
    int fieldIndexInGrid(const QString &fieldName);
    SqlField *fieldByName(const QString &fieldName);
    QString whereClause();
};

#endif // QBASESQLDRIVER_H
