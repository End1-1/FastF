#include "mptcpsocket.h"
#include <QNetworkProxy>

MPTcpSocket::MPTcpSocket(QObject *parent) :
    QObject(parent)
{

}

void MPTcpSocket::setServerIP(const QString &ip)
{
    fServerIP = ip;
}

void MPTcpSocket::setValue(const QString &key, const QVariant &value)
{
    fData[key] = value;
}

QJsonObject MPTcpSocket::sendData()
{
    QJsonObject jObj;
    foreach (QString s, fData.keys()) {
        jObj[s] = fData[s].toString();
    }
    fData.clear();
    QJsonDocument jDoc(jObj);
    QByteArray rawData = jDoc.toJson();
    QTcpSocket socket;
    socket.setProxy(QNetworkProxy::NoProxy);
    if (fServerIP.length() == 0) {
        socket.connectToHost(SETVALUE("ip").toString(), 888);
    } else {
        socket.connectToHost(fServerIP, 888);
    }
    if (!socket.waitForConnected()) {
        QJsonObject jErr;
        jErr["error"] = socket.errorString() + "\r\n" + SETVALUE("ip").toString() + ":" + "888";
        emit handleJson(jErr);
        return jErr;
    }
    qint32 dataSize = rawData.size();
    socket.write(reinterpret_cast<const char*>(&dataSize), sizeof(quint32));
    socket.write(rawData);
    socket.flush();
    socket.waitForBytesWritten();
    dataSize = 0;
    rawData.clear();
    socket.waitForReadyRead();
    if (socket.bytesAvailable()) {
        do {
            socket.waitForReadyRead();
            if (socket.bytesAvailable()) {
                if (dataSize == 0) {
                    socket.read(reinterpret_cast<char *>(&dataSize), sizeof(qint32));
                }
                const int buffSize = 1024;
                char tempBuff[buffSize];
                int bytesRead = socket.read(tempBuff, buffSize);
                if (bytesRead > 0) {
                    rawData.append(tempBuff, bytesRead);
                } else {
                    break;
                }
            }
        } while (rawData.size() < dataSize);
    }
    socket.close();
    jDoc = QJsonDocument::fromJson(rawData);
    emit handleJson(jDoc.object());
    return jDoc.object();
}
