#ifndef OD_DRV_H
#define OD_DRV_H

#include "od_base.h"
#include "od_dish.h"
#include "od_header.h"
#include "od_flags.h"
#include "od_print.h"

class OD_Drv : public OD_Base
{
    Q_OBJECT
private:

public:
    QList<OD_Dish*> m_dishes;
    OD_Flags m_flags;
    OD_Header m_header;
    OD_Print m_print;

    bool mfSpecialActionsDishes;
    bool mfSpecialActionsOrders;
    bool mfPayOnlyOwnOrders;

    float mfDefaultPriceInc;
    float mfDefaultPriceDec;

    OD_Drv();
    ~OD_Drv();

    bool openTable(const QString &orderId, bool isClosed, int userId);
    bool updateTableInfo(const QString &orderId, int tableId, int state);
    bool closeTable();
    bool saveAll();
    bool saveGat();

    bool discount(int id, float value);
    void countAmounts();
    int appendDish(OD_Dish *newDish);
    int appendDish2(OD_Dish *newDish);
    OD_Dish *dish(int index);

signals:
    void counted(const QMap<QString, QString> &values);
    void dishRepaint(int index);
};

#endif // OD_DRV_H
