#include "qnet.h"
#include <QDebug>

QNet::QNet(QObject *parent) :
    QNetworkAccessManager(parent)
{
    ContentType = "multipart/form-data, boundary=" + boundary;
}

QNet::~QNet()
{
}

void QNet::goSSL()
{
    connect(this, SIGNAL(finished(QNetworkReply*)), this, SLOT(finished(QNetworkReply*)));
    QNetworkRequest nr = QNetworkRequest(QUrl(URL));
    QSslConfiguration sslConf = nr.sslConfiguration();
    sslConf.setPeerVerifyMode(QSslSocket::VerifyNone);
    sslConf.setProtocol(QSsl::AnyProtocol);
    nr.setSslConfiguration(sslConf);

    nr.setRawHeader("Content-Type", ContentType.toLatin1());
    nr.setRawHeader("Content-Length", QString::number(m_data.length()).toLatin1());
    nr.setRawHeader("Cache-Control", "no-cache");
    nr.setRawHeader("Accept", "*/*");

    for (QMap<QString, QString>::const_iterator it = rawHeader.begin(); it != rawHeader.end(); it++)
        nr.setRawHeader(it.key().toLatin1(), it.value().toLatin1());
    post(nr, m_data);
}

void QNet::go()
{
    connect(this, SIGNAL(finished(QNetworkReply*)), this, SLOT(finished(QNetworkReply*)));
    QNetworkRequest nr = QNetworkRequest(QUrl(URL));
    nr.setRawHeader("Content-Type", ContentType.toLatin1());
    nr.setRawHeader("Content-Length", QString::number(m_data.length()).toLatin1());
    nr.setRawHeader("Cache-Control", "no-cache");
    nr.setRawHeader("Accept", "*/*");

    for (QMap<QString, QString>::const_iterator it = rawHeader.begin(); it != rawHeader.end(); it++)
        nr.setRawHeader(it.key().toLatin1(), it.value().toLatin1());
    post(nr, m_data);
}

void QNet::addData(const QString &name, const QString &data)
{
    m_data.append("Content-Disposition: form-data; name=\"" + name + "\"\r\n\r\n");
    m_data.append(data + "\r\n");
    m_data.append("--" + boundary + "\r\n");
}

void QNet::addData(const QByteArray &data)
{
    m_data.append(data);
}

void QNet::downloadRawData(const QString &url)
{
    connect(this, SIGNAL(finished(QNetworkReply*)), this, SLOT(rawFinished(QNetworkReply*)));
    QUrl u(url);
    QNetworkRequest r(u);
    QSslConfiguration sslConf = r.sslConfiguration();
    sslConf.setPeerVerifyMode(QSslSocket::VerifyNone);
    sslConf.setProtocol(QSsl::AnyProtocol);
    r.setSslConfiguration(sslConf);
    get(r);
}

void QNet::deleteLater(const QString &data, bool isError)
{
    Q_UNUSED(data);
    Q_UNUSED(isError);
    QObject::deleteLater();
}

void QNet::finished(QNetworkReply *reply)
{
    if (reply->error() == QNetworkReply::NoError) {
        emit getResponse(reply->readAll(), false);
    } else {
        emit getResponse(reply->errorString(), true);
    }
    reply->deleteLater();
}

void QNet::rawFinished(QNetworkReply *reply)
{
    if (reply->error() == QNetworkReply::NoError) {
        QVariant attribute = reply->attribute(QNetworkRequest::RedirectionTargetAttribute);
        if (attribute.isValid()) {
            QUrl url = attribute.toUrl();
            qDebug() << "must go to:" << url;
            return;
        }
        m_rawData = reply->readAll();
        emit getRawResponse(m_rawData, false);
    } else {
        m_rawData = reply->errorString().toLatin1();
        emit getRawResponse(m_rawData, true);
    }
    reply->deleteLater();
}
