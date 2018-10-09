#include "gatwriter.h"
#include <QDebug>
#include <QAxObject>

GATWriter::GATWriter()
{
    //setControl("{0A530613-6024-11D5-A3AC-0050BF2CF639}");
    connect(&m_timer, SIGNAL(timeout()), this, SLOT(timeout()));
    m_seconds = 0;
    m_timer.start(1500);
}

void GATWriter::timeout()
{
    m_seconds++;
    QAxObject *o = new QAxObject();
    o->setControl("{0A530613-6024-11D5-A3AC-0050BF2CF639}");
    o->setProperty("FID", 9999);
    o->setProperty("CryptKey", "E6147DD79B589816B733BE7B4D52EE32");
    QString cr = "E6147DD79B589816B733BE7B4D52EE32", out = "9999";
    o->dynamicCall("MF_SetCrypt(QString&,QString&)", cr, out);
    o->dynamicCall("OpenDevice()");
    //int a = 0;
    //qDebug() << o->dynamicCall("ReadCash(int, int&)", "2", a);
    o->dynamicCall("LEDGreen(int)", 100);
    o->dynamicCall("Beep(int)", 100);
    QString code = o->dynamicCall("GetUniqueNumber()").toString();
    o->dynamicCall("CloseDevice()");
    o->deleteLater();
    if (code.length()) {
        m_timer.stop();
        emit readCode(code);
    }
    if (m_seconds > 5) {
        m_timer.stop();
        emit longTimeout();
    }
}
