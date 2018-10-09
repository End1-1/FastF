#ifndef OD_BASE_H
#define OD_BASE_H

#include "od_config.h"
//#include "logthread.h"

class OD_Base : public QObject
{
    Q_OBJECT

private:


protected:

public:
    DbDriver m_dbDrv;
    bool m_saved;
    OD_Base();
    void configureDB(const QString &dbHost, const QString &dbPath, const QString &dbUser, const QString &dbPass);
    void configureOtherDB(QSqlDatabase &db);
    bool openDB();
    void closeDB();
    bool next();
    int v_int(int index);
    QString v_str(int index);
    double v_dbl(int index);
    QDateTime v_dateTime(int index);
    QDate v_date(int index);
    bool prepare(const QString &sql);
    bool execSQL();
    bool bindValue(const QString &name, const QVariant &value);
    int genId(const QString &name);
    QString mfOrderIdPrefix;
    QString mfObjectName;
};

#endif // OD_BASE_H
