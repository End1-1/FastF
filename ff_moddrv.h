#ifndef FF_MODDRV_H
#define FF_MODDRV_H

#include "qsqldrv.h"
#include "ff_orderdrv.h"

#define MOD_TYPE_INC 1
#define MOD_TYPE_DEC 2

class FF_ModDrv : public QSqlDrv
{
    Q_OBJECT
public:
    explicit FF_ModDrv();
    void init();
    void autoMod(FF_OrderDrv *orderDrv);
    FF_ModDrv &operator=(const FF_ModDrv &src);

signals:

private:
    QMap<QString, int> m_fields;
    QList<QList<QVariant> > m_data;

};

#endif // FF_MODDRV_H
