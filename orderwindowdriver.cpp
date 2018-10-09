#include "orderwindowdriver.h"
#include "qnet.h"
#include "ff_settingsdrv.h"
#include "dlgmessage.h"
#include "logthread.h"
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QDebug>


OrderWindowDriver::OrderWindowDriver(QObject *parent) :
    QObject(parent)
{
}

void OrderWindowDriver::checkOnlinePayment(OD_Drv *o)
{
    m_drv = o;
    QNet *n = new QNet(this);
    connect(n, SIGNAL(getResponse(QString,bool)), SLOT(parseOnlinePaymentResponse(QString,bool)));
    n->URL = "https://money.idram.am/api/History/Search";
    n->rawHeader.insert("_SessionId_", FF_SettingsDrv::value(SD_IDRAM_SESSION_ID).toString()); //"3497ae22-8623-45be-84d9-f9f9671b0628");
    n->rawHeader.insert("_EncMethod_", "NONE");
    n->ContentType = "application/json";
   // m_drv->m_header.f_id = "N98960";
    QString request = QString("{\"Detail\":\"" + m_drv->m_header.f_id + "\"}");
    n->addData(request.toUtf8());
    LogThread::logOrderThread(o->m_header.f_currStaffId, o->m_header.f_id, "IDram request", request + " Session:" + FF_SettingsDrv::value(SD_IDRAM_SESSION_ID).toString());
    n->go();
}

void OrderWindowDriver::checkDiscountApp(OD_Drv *o, const QString &query)
{
    /* remove block */
    emit discountChecked();
    return;
    /* end remove block &*/
    m_drv = o;
    QNet *n = new QNet(this);
    connect(n, SIGNAL(getResponse(QString,bool)), this, SLOT(parseDiscountAppResponse(QString,bool)));
    n->URL = "http://jazzve.am/zMobileApp/control/app_response.php?t=" + query;
    n->go();
}

void OrderWindowDriver::removeDiscountFromApp(const QString &query)
{
    /* remove block */
    emit discountRemoved();
    return;
    /* end remove block */
    QNet *n = new QNet(this);
    connect(n, SIGNAL(getResponse(QString,bool)), this, SLOT(parseRemoveDiscountFromApp(QString,bool)));
    n->URL = "http://jazzve.am/zMobileApp/control/app_remove.php?t=" + query;
    n->go();
}

void OrderWindowDriver::parseOnlinePaymentResponse(const QString &str, bool isError)
{
    qobject_cast<QNet*>(sender())->deleteLater();
    if (isError) {
        DlgMessage::Msg(tr("Network error. Try again") + "\r\n" + str);
        LogThread::logOrderThread(m_drv->m_header.f_currStaffId, m_drv->m_header.f_id, "IDram response", str);
        return;
    }
    QJsonDocument jDoc = QJsonDocument::fromJson(str.toUtf8());
    QJsonObject jObj = jDoc.object();
    QJsonArray jArr = jObj["Result"].toArray();
    if (!jArr.count()) {
        qDebug() << str;
        LogThread::logOrderThread(m_drv->m_header.f_currStaffId, m_drv->m_header.f_id, "IDram response", str);
        DlgMessage::Msg(tr("Payment not confirmed"));

        return;
    }
    QJsonObject jObjArr = jArr[0].toObject();
    QString result = jObjArr["SRC_STATUS"].toString();
    double debit = jObjArr["DEBIT"].toString().replace(",", "").toDouble();

    if (!(result == "0")) {
        LogThread::logOrderThread(m_drv->m_header.f_currStaffId, m_drv->m_header.f_id, "IDram response", str);
        DlgMessage::Msg(tr("Payment not confirmed"));
        return;
    }

    if (debit < m_drv->m_header.f_amount) {
        LogThread::logOrderThread(m_drv->m_header.f_currStaffId, m_drv->m_header.f_id, "IDram response", str);
        DlgMessage::Msg(tr("Not enough money to continue") + "\r\n" + QString("%1 < %2").arg(debit).arg(m_drv->m_header.f_amount));
        return;
    }

    switch(m_drv->m_flags.setFlag(OFLAG_IDRAM, QString("%1:%2").arg(m_drv->m_header.f_amount).arg(debit), m_drv->m_dbDrv)) {
    case -1:
        LogThread::logOrderThread(m_drv->m_header.f_currStaffId, m_drv->m_header.f_id, "IDram response", str);
        DlgMessage::Msg(tr("Program error. Payment rejected."));
        return;
    case 0:
        break;
    case 1:
        LogThread::logOrderThread(m_drv->m_header.f_currStaffId, m_drv->m_header.f_id, "IDram response", str);
        DlgMessage::Msg(tr("Alreary paid."));
        return;
    }

    if (debit > m_drv->m_header.f_amount) {
        DlgMessage::Msg(QString("%1 : %2").arg(tr("Tip")).arg(debit - m_drv->m_header.f_amount));
    }
    LogThread::logOrderThread(m_drv->m_header.f_currStaffId, m_drv->m_header.f_id, "IDram response", str + "\r\n" + "Confirmed");
    DlgMessage::Msg(tr("Payment confirmed"));
}

void OrderWindowDriver::parseDiscountAppResponse(const QString &str, bool isError)
{
    qobject_cast<QNet*>(sender())->deleteLater();
    if (isError) {
        DlgMessage::Msg(tr("Network error. Try again") + "\r\n" + str);
        emit discountCheckError();
        return;
    }

    if (str == "0") {
        DlgMessage::Msg(tr("No online discount"));
        emit discountChecked();
        return;
    }
    QRegExp r("user=(.*);value=(.*)");
    if (r.indexIn(str) < 0){
        DlgMessage::Msg(tr("Network error. Try again") + "\r\n" + str);
        emit discountCheckError();
        return;
    }
    m_drv->m_header.f_amount_dec_value = r.cap(2).toFloat() / 100;
    m_drv->mfDefaultPriceDec = m_drv->m_header.f_amount_dec_value;
    for (int i = 0; i < m_drv->m_dishes.count(); i++)
        m_drv->m_dishes[i]->setPriceMod(m_drv->mfDefaultPriceInc, m_drv->mfDefaultPriceDec);
    m_drv->countAmounts();
    emit discountChecked();
}

void OrderWindowDriver::parseRemoveDiscountFromApp(const QString &str, bool isError)
{
    qobject_cast<QNet*>(sender())->deleteLater();
    qDebug() << str;
    qDebug() << isError;
    emit discountRemoved();
}
