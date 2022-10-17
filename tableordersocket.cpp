#include "tableordersocket.h"
#include "cnfmaindb.h"
#include "fastfprotocol.h"
#include <QApplication>
#include <QFile>
#include <QMessageBox>

static QSslCertificate fCert;

TableOrderSocket::TableOrderSocket(int tableId, QObject *parent) :
    TableOrderSocket(parent)
{
    fTable = tableId;
}

TableOrderSocket::TableOrderSocket(const QString &orderId, QObject *parent) :
    TableOrderSocket(parent)
{
    fOrderId = orderId;
}

void TableOrderSocket::begin()
{
    if (fCert.isNull()) {
        QString certFileName = qApp->applicationDirPath() + "/" + "cert.pem";
        QFile file(certFileName);
        if (!file.open(QIODevice::ReadOnly)) {
            emit err("Certificate file missing");
            return;
        }
        fCert = QSslCertificate(file.readAll());
    }
    qDebug() << QSslSocket::supportsSsl() << QSslSocket::sslLibraryBuildVersionString() << QSslSocket::sslLibraryVersionString();
    reset();    
    fSocket->addCaCertificate(fCert);
    fSocket->setPeerVerifyMode(QSslSocket::VerifyNone);
    fSocket->connectToHostEncrypted(__cnfmaindb.fServerIP, 8788);
    if (!fSocket->waitForEncrypted(3000)) {
        qDebug() << fSocket->errorString();
        emit err(tr("Could not secure socket.") + "<br>" + fSocket->errorString() + "<br>" + QString("%1:%2").arg(__cnfmaindb.fServerIP).arg(8788));
        return;
    }
    if (fOrderId.isEmpty()) {
        QByteArray data = QString::number(fTable).toUtf8();
        quint32 dataSize = data.size();
        quint32 dataType = p_locktable;
        fSocket->write(reinterpret_cast<const char*>(&dataSize), sizeof(dataSize));
        fSocket->write(reinterpret_cast<const char*>(&dataType), sizeof(dataType));
        fSocket->write(data, data.length());
        fSocket->waitForBytesWritten();
    } else {
        QByteArray data = fOrderId.toUtf8();
        quint32 dataSize = data.size();
        quint32 dataType = p_locktable_order;
        fSocket->write(reinterpret_cast<const char*>(&dataSize), sizeof(dataSize));
        fSocket->write(reinterpret_cast<const char*>(&dataType), sizeof(dataType));
        fSocket->write(data, data.length());
        fSocket->waitForBytesWritten();
    }
}

TableOrderSocket::TableOrderSocket(QObject *parent) :
    QObject(parent)
{
    fTable = 0;
    fSocket = new QSslSocket(this);
    connect(fSocket, SIGNAL(readyRead()), this, SLOT(readyRead()));
    connect(fSocket, SIGNAL(disconnected()), this, SLOT(disconnected()));
}

void TableOrderSocket::reset()
{
    fDataSize = 0;
    fDataType = 0;
    fDataRead = 0;
    fData.clear();
}

void TableOrderSocket::lockTable()
{
    quint32 result;
    memcpy(&result, fData.data(), sizeof(result));
    if (result == 1) {
        emit tableLocked(fTable);
    } else {
        emit err(tr("Could not lock table"));
    }
}

void TableOrderSocket::readyRead()
{
    if (fDataSize == 0) {
        fSocket->read(reinterpret_cast<char *>(&fDataSize), sizeof(fDataSize));
    }
    if (fDataType == 0) {
        fSocket->read(reinterpret_cast<char *>(&fDataType), sizeof(fDataType));
    }
    fData.append(fSocket->read(fDataSize - fDataRead));
    fDataRead = fData.size();
    if (fDataRead == fDataSize) {
        switch (fDataType) {
        case p_locktable:
            lockTable();
            break;
        case p_locktable_order:
            lockTable();
            break;
        }
        reset();
    }
}

void TableOrderSocket::disconnected()
{
    emit socketDisconnected();
}
