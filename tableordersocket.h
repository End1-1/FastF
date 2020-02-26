#ifndef TABLEORDERSOCKET_H
#define TABLEORDERSOCKET_H

#include <QSslSocket>
#include <QJsonObject>

class TableOrderSocket : public QObject
{
    Q_OBJECT
public:
    explicit TableOrderSocket(int tableId, QObject *parent = nullptr);
    explicit TableOrderSocket(const QString &orderId, QObject *parent = nullptr);
    void begin();
    QJsonObject fJson;
    int fTable;
    QString fOrderId;

private:
    TableOrderSocket(QObject *parent = nullptr);
    QSslSocket *fSocket;
    quint32 fDataSize;
    quint32 fDataRead;
    quint32 fDataType;
    QByteArray fData;
    void reset();
    void lockTable();

private slots:
    void readyRead();
    void disconnected();

signals:
    void socketDisconnected();
    void err(const QString &message);
    void tableLocked(int tableId);
};

#endif // TABLEORDERSOCKET_H
