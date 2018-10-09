#ifndef ORDERWINDOWDRIVER_H
#define ORDERWINDOWDRIVER_H

#include <QObject>
#include "qnet.h"
#include "./orderdrv/od.h"

class OrderWindowDriver : public QObject
{
    Q_OBJECT

private:
    OD_Drv *m_drv;

public:
    explicit OrderWindowDriver(QObject *parent = 0);
    void checkOnlinePayment(OD_Drv *o);
    void checkDiscountApp(OD_Drv *o, const QString &query);
    void removeDiscountFromApp(const QString &query);

signals:
    void discountChecked();
    void discountCheckError();
    void discountRemoved();

public slots:
    void parseOnlinePaymentResponse(const QString &str, bool isError);
    void parseDiscountAppResponse(const QString &str, bool isError);
    void parseRemoveDiscountFromApp(const QString &str, bool isError);

};

#endif // ORDERWINDOWDRIVER_H
