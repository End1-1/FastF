#ifndef OD_DISH_H
#define OD_DISH_H

#include "od_base.h"

class OD_Dish : public QObject
{
    Q_OBJECT
private:
    bool m_qty05;
    float m_oldQty;
public:
    int m_index;
    int f_id;
    QString f_orderId;
    int f_stateId;
    int f_dishId;
    QString f_adgCode;
    QString f_dishName;
    float f_totalQty;
    float f_printedQty;
    float f_price;
    float f_amount;
    float f_amountInc;
    float f_amountDec;
    int f_lastUser;
    QString f_lastUserName;
    int f_storeId;
    QString f_comments;
    int f_remind;
    int f_paymentMod;
    float f_priceInc;
    float f_priceDec;
    int f_removeReason;
    QString f_print1;
    QString f_print2;
    int flag14;

    OD_Dish();
    OD_Dish *copy();
    void loadFromDb(DbDriver &db);
    bool saveToDB(DbDriver &db);

    void setPriceMod(float inc, float dec);
    float getTotal();
    float getTotalInc();
    float getTotalDec();
    void setOldQty();
    void commitDecQty();
    void rollbackDecQty();
    float availableForRemove();
    bool m_saved;
    bool m_removePrinted;

signals:
    void update(int index);
    void removed(int index, float qty);
    void message(const QString &msg);
};

#endif // OD_DISH_H
