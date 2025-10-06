#include "qqrengine.h"
#include <QDebug>

QQrEngine::QQrEngine()
{
}


void QQrEngine::encodeBytes(const QString &src)
{
    QByteArray temp = src.toUtf8();
    m_outBuffer.clear();
    m_outBuffer.append(E_BYTE);
    for (int i = 0; i < temp.length(); i++) {
        QByteArray c;
        for (int j = 0; j < 8; j++)
            c.append(((unsigned char)temp.at(i) >> j) & 1);
        std::reverse(c.begin(), c.end());
        m_outBuffer.append(c);
        qDebug() << c;
    }
}

void QQrEngine::encodeData(const QString &src, QQrEngine::EncodeType et)
{
    switch (et) {
    case etDigits:
        break;
    case etDigitsAndSymbols:
        break;
    case etByte:
        encodeBytes(src);
        break;
    }
}
