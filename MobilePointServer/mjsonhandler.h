#ifndef MJSONHANDLER_H
#define MJSONHANDLER_H

#include <QObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMap>
#include <QHostAddress>
#include "msqldatabase.h"

class MJsonHandler : public QObject
{
    Q_OBJECT
public:
    QJsonObject handleDish(const QJsonObject &o);
    QJsonObject handleDishType(const QJsonObject &o);
    QJsonObject handleHall(const QJsonObject &o);
    QJsonObject handleDishToOrder(const QJsonObject &o);
    QJsonObject handleOpenTable(const QJsonObject &o);
    QJsonObject handleLoadOrder(const QJsonObject &o);
    QJsonObject handleDishChange(const QJsonObject &o);
    QJsonObject handleDishRemove(const QJsonObject &o);
    QJsonObject handlePrintKitchen(const QJsonObject &o);
    QJsonObject handleReceipt(const QJsonObject &o);
    QString handleDishInfo(const QJsonObject &o);
    QJsonObject handleSettings(const QJsonObject &o);
    QJsonObject handleListReceipt(const QJsonObject &o);
    QJsonObject handleCloseOrder(const QJsonObject &o);
    QJsonObject handleDishComment(const QJsonObject &o);
    QJsonObject handleCommentsList(const QJsonObject &o);
    QJsonObject handleReadyDishes(const QJsonObject &o);
    QJsonObject handleReadyDishDone(const QJsonObject &o);
    QJsonObject handleMyMoney(const QJsonObject &o);
    QJsonObject handlePrintFiscal(const QJsonObject &o);
    QJsonObject handleTaxCancel(const QJsonObject &o);
    QString updateOrderAmount(const QString &id);
    int checkPassword(const QString &pwd);
    QString sessionNew();
    QJsonObject jsonError(const QString &err);

public:
    explicit MJsonHandler(QObject *parent = nullptr);

private:
    QString fError;
    MSqlDatabase fDb;

signals:

public slots:
};

#endif // MJSONHANDLER_H
