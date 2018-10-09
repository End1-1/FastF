#ifndef QNET_H
#define QNET_H

#include <QNetworkAccessManager>
#include <QNetworkReply>
#define boundary QString("----7d935033608e2")

class QNet : public QNetworkAccessManager
{
    Q_OBJECT
public:
    explicit QNet(QObject *parent = 0);
    ~QNet();
    QString URL;
    QMap<QString, QString> rawHeader;
    QString ContentType;
    void go();
    void addData(const QString &name, const QString &data);
    void addData(const QByteArray &data);
    QByteArray m_rawData;
    void downloadRawData(const QString &url);

signals:
    void getResponse(const QString &data, bool isError);
    void getRawResponse(QByteArray &data, bool isError);

public slots:
    void finished(QNetworkReply *reply);
    void rawFinished(QNetworkReply *reply);

public:
    QByteArray m_data;
};

#endif // QNET_H
