#ifndef QQRENGINE_H
#define QQRENGINE_H

#include <QByteArray>

#define E_DIGITS QString"0001"
#define E_DIGITS_SYMBOLS "0010"
#define E_BYTE "0100"

class QQrEngine {

private:
    QByteArray m_outBuffer;
    void encodeBytes(const QString &src);

public:
    enum EncodeType {etDigits = 0, etDigitsAndSymbols, etByte};
    QQrEngine();
    void encodeData(const QString &src, EncodeType et);
};

#endif // QQRENGINE_H
