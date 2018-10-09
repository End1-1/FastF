#ifndef FF_ORDERDRV_H
#define FF_ORDERDRV_H

#define DISH_STATE_NORMAL 1
#define DISH_STATE_REMOVED_NORMAL 2
#define DISH_STATE_REMOVED_PRINTED 3
#define DISH_STATE_MOVED 4
#define DISH_STATE_MOVE_PARTIALY 5
#define DISH_STATE_REMOVE_AFTER_CHECKOUT 6

#define ORDER_STATE_OPEN 1
#define ORDER_STATE_CLOSED 2
#define ORDER_STATE_EMTPY1 3 //Full empty
#define ORDER_STATE_EMPTY2 4 //Contain printed and then removed dishes
#define ORDER_STATE_REMOVED 5
#define ORDER_STATE_MERGE 6

#define ORDER_READ_FAILED 0
#define ORDER_READ_NEW 1
#define ORDER_READ_EXISTS 2

#define DISH_MOD_NORMAL 1
#define DISH_MOD_NOINCDEC 2

#define OFLAG_IDRAM 1
#define OFLAG_MENU_AM 2
#define OFLAG_DELIVERY 3
#define OFLAG_KARAOKE_CLUB 4

#include "qsqldrv.h"
#include "ff_user.h"
#include "ff_halldrv.h"

class RemindThread : public QThread {
    Q_OBJECT
private:
    QString m_username;
    int m_staff;
    int m_table;
    int m_dish;
    float m_qty;

public:
    RemindThread(const QString &username);
    void setData(int staff, int table, int dish, float qty);

protected:
    virtual void run();
};

class FF_OrderDrv : public QSqlDrv
{
    Q_OBJECT

    struct OrderHeader {
        QString id;
        int state_id;
        int table_id;
        QString table_name;
        int staff_id;
        QString staff_name;
        float amount;
        float amount_inc;
        float amount_inc_value;
        float amount_dec;
        float amount_dec_value;
        QDateTime date_open;
        QDateTime date_close;
        QDate date_cash;
        int printed;
        int paymentType;
        int printed_tax;
        int current_staff_id;
        QString current_staff_name;
        QString comment;
    };

public:

    struct OrderDish {
        int id;
        int state_id;
        int dish_id;
        QString dish_name;
        float qty;
        float pqty;
        float price;
        int store_id;
        int print_schema;
        int payment_mod;
        int last_user;
        float rqty;
        bool saved;
        QString comment;
        bool remind;
        double price_inc;
        double price_dec;
    };

private:
    bool m_qtyEqualPrintQty;
    FF_HallDrv *m_hallDrv;
    bool insertDish(OrderDish &dish);

public:
    QList<OrderDish> m_dishes;
    OrderHeader m_header;
    explicit FF_OrderDrv(FF_HallDrv *hallDrv, const QString &username);
    int readTable(int tableId, FF_User *user);
    bool readOrder(QString id);
    void freeTable();
    int dishCount(int state);
    bool appendDish(OrderDish dish);
    int duplicateDish(int dishIndex);
    float decreaseDishQty(int dishIndex, float rmQty);
    void changeDishQty(int dishIndex, float qty);
    void increaseDishQty(int dishIndex, float qty);
    void getDishesForServicePrint(QMap<int, float> &list);
    bool saveOrder(bool closeTransaction = true);
    bool closeOrder(int state, const QDate &cashDate);
    void clearOrder();
    void countOrder();
    bool moveOrder(int dstTableId, FF_User *user, FF_HallDrv *hallDrv);
    void removeDishesFrom(int index);
    void incMod(float value);
    void decMod(int costumerId, float value);
    bool moveDish(int dstTableId, FF_User *user, int dishIndex, float qty);
    void makeTaxPrintCheck(bool print = true);
    int setOrderFlag(int flagId, const QString &data, bool unique = true);

signals:
    void counted();
    void dishRepaint(int dishIndex);
    void dishQtyEqualPrintQty(bool equal);

};

#endif // FF_ORDERDRV_H
