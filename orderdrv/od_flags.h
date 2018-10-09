#ifndef OD_FLAGS_H
#define OD_FLAGS_H

#include "od_base.h"

class OD_Drv;

class OD_Flags
{
private:
    QMap<int, QString> m_flags;
    QString m_orderId;

public:
    OD_Flags();
    bool loadFromDB(const QString &orderId, DbDriver &db);
    int setFlag(int flagId, const QString &data, DbDriver &db);
};

#endif // OD_FLAGS_H
