#ifndef FF_HALLDRV_H
#define FF_HALLDRV_H

#include "dbdriver.h"
#include <QDate>
#include <QTime>
#include <QTableWidget>
#include <QItemDelegate>

#define LOCK_SUCCESS 0
#define LOCK_LOCKED 1
#define LOCK_ERROR 2

#define TFLAG_NEW 0
#define TFLAG_CHECKOUT 1
#define TFLAG_DEFAULT "000000000000"

class FF_HallDrv : public DbDriver
{
        Q_OBJECT

public:
    struct Hall {
        int id;
        QString name;
    };

    struct Table {
        int id;
        QString name;
        Hall *hall;
        int queue;
        QString orderId;
        float amount;
        float printed;
        QString dateOpen;
        QString staffName;
        QString lock;
        QString orderComment;
        QString reserved;
        QString flags;
    };

    struct HallTotal {
        int qty;
        float amount;
        HallTotal(){qty = 0; amount = 0;}
    };

private:
    QList<Hall> m_hall;
    QMap<int, int> m_tableIdIndexMap;
    QMap<int, HallTotal> m_hallTotal;
    void loadHall();
    static QItemDelegate *m_itemDelegate;
    int m_filterHallId;
    int m_filterOnlyBusy;

public:
    explicit FF_HallDrv();
    Hall *getHall(int id);
    Table *table(int id);
    QList<Table> m_tables;
    QString total();
    QList<int> m_proxyTables;
    void filter(int hallId, bool onlyBusy);
    void refresh();
    void configGrid(QTableWidget *t, QItemDelegate *itemDelegate);
    QMap<QString, QVariant> getHallMap();
    bool tableFlag(const Table &t, int flag);
    void setFlag(int tableId, int flag, const QChar &value);

    int m_totalQty;
    float m_totalAmount;

};

#endif // FF_HALLDRV_H
