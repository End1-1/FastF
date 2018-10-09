#ifndef OD_PRINT_H
#define OD_PRINT_H

#include "od_header.h"
#include "od_dish.h"

class OD_Drv;

class OD_Print : public QObject
{
    Q_OBJECT
private:
    static QMap<int, QStringList> m_printSchema;
public:
    OD_Print();
    ~OD_Print();
    void getPrinterSchema(OD_Base *db);
    bool printService(int remind, const QString &objName, QList<OD_Dish*> &dishes, OD_Header &header, DbDriver &db);
    void printCheckout(const QString &prnName, OD_Drv *d);
    void printRemoved(int index, float qty, OD_Drv *d, const QString &reason);
    void printTax(const QString &ip, const QString &port, const QString &pass, OD_Drv *d, bool print = true);
    void printTax(const QString &orderId, DbDriver &db);
    static QFont mfFont;
};

#endif // OD_PRINT_H
