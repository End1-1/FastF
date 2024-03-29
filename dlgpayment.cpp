#include "dlgpayment.h"
#include "ui_dlgpayment.h"
#include "logthread.h"
#include "dlgmessage.h"
#include "utils.h"
#include "cnfmaindb.h"
#include "ff_settingsdrv.h"
#include "dlggetpassword.h"
#include "dlgcalcchange.h"
#include "ff_user.h"
#include "cnfapp.h"

DlgPayment::DlgPayment(FF_User *u, OD_Drv *drv, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DlgPayment)
{
    ui->setupUi(this);
    ui->btnComplimentary->setVisible(u->m_groupId.toInt() <= 2);
    ui->btnPrepaymentCash_2->setVisible(u->m_groupId.toInt() <= 2);
    fDrv = drv;
    fGift = 0;
    fTcpSocket.setServerIP(__cnfmaindb.fServerIP);
    ui->leTable->setText(fDrv->m_header.f_tableName);
    ui->leStaff->setText(fDrv->m_header.f_currStaffName);
    ui->leAmount->setText(fDrv->m_header.f_amount);
    fDrv->m_dbDrv.prepare("select fcash, fcard from o_tax where fid=:fid");
    fDrv->m_dbDrv.bindValue(":fid", fDrv->m_header.f_id);
    fDrv->m_dbDrv.execSQL();
    if (fDrv->m_dbDrv.next()) {
        ui->leCash->setText(fDrv->m_dbDrv.v_str(0));
        ui->leCard->setText(fDrv->m_dbDrv.v_str(1));
    } else {
        on_leCash_textChanged(ui->leAmount->text());
    }

    fDrv->prepare("select f_code from o_gift_card where f_order=:f_order");
    fDrv->bindValue(":f_order", fDrv->m_header.f_id);
    fDrv->execSQL();
    if (fDrv->next()) {
        DbDriver db;
        db.configureDb("10.1.0.2", "maindb", "SYSDBA", "masterkey");
        if (!db.openDB()) {
            DlgMessage::Msg("Cannot connect to main server!");
            return;
        }
        db.prepare("select sum(f_amount) from o_gift_card where f_code=:f_code");
        db.bindValue(":f_code", fDrv->v_str(0));
        db.execSQL();
        if (!db.next()) {
            DlgMessage::Msg(tr("Invalid gift card"));
            return;
        }
        fGift = db.v_dbl(0);
        ui->leGift->setText(db.v_str(0));
        if (fGift > ui->leAmount->text().toDouble()) {
            ui->leToPay->setText("0");
        } else {
            ui->leToPay->setText(QString::number(ui->leAmount->text().toDouble() - fGift));
        }
        ui->btnCash->setEnabled(false);
        ui->btnIDRAM->setEnabled(false);
    } else {
        ui->wGift->setVisible(false);
    }
}

DlgPayment::~DlgPayment()
{
    delete ui;
}

bool DlgPayment::payment(FF_User *u, OD_Drv *drv, QWidget *parent)
{
    DlgPayment *d = new DlgPayment(u, drv, parent);
    bool result = d->exec() == QDialog::Accepted;
    delete d;
    return result;
}

void DlgPayment::on_btnPrepayment_clicked()
{

}

void DlgPayment::on_btnCash_clicked()
{
    payment(PaymentCash);
}

void DlgPayment::payment(quint8 mode)
{
    fTcpSocket.setValue("session", SESSIONID);
    fTcpSocket.setValue("query", "closeorder");
    fTcpSocket.setValue("staff", fDrv->m_header.f_currStaffId);
    fTcpSocket.setValue("mode", mode);
    fTcpSocket.setValue("order", fDrv->m_header.f_id);
    fTcpSocket.setValue("printer", FF_SettingsDrv::value(SD_CHECK_PRINTER_NAME).toString());
    QJsonObject o = fTcpSocket.sendData();
    if (!o.contains("reply")) {
        msg(tr("Unknown error"));
        return;
    }
    if (o["reply"] == "ok") {
        fDrv->m_header.f_stateId = ORDER_STATE_CLOSED;
        LogThread::logOrderThread(fDrv->m_header.f_currStaffName, fDrv->m_header.f_id, "", "Order closed", "");
        accept();
        return;
    }
    msg(o["reply"].toString());
}

void DlgPayment::receipt(int mode)
{
    ui->btnCash->setEnabled(false);
    ui->btnCard->setEnabled(false);
    ui->btnPrepaymentCash->setEnabled(false);
    fTcpSocket.setValue("session", SESSIONID);
    fTcpSocket.setValue("query", "printreceipt");
    fTcpSocket.setValue("staff", fDrv->m_header.f_currStaffId);
    fTcpSocket.setValue("order", fDrv->m_header.f_id);
    fTcpSocket.setValue("printer", FF_SettingsDrv::value(SD_CHECK_PRINTER_NAME).toString());
    fTcpSocket.setValue("mode", mode);
    fTcpSocket.setValue("cash", ui->leCash->text().toDouble());
    fTcpSocket.setValue("card", ui->leCard->text().toDouble());
    fTcpSocket.setValue("tax", ui->leTax->text());
    QJsonObject o = fTcpSocket.sendData();
    ui->btnCash->setEnabled(true);
    ui->btnCard->setEnabled(true);
    ui->btnPrepaymentCash->setEnabled(true);
    if (!o.contains("reply")) {
        msg(tr("Unknown error"));
        return;
    }
    if (o["reply"] == "ok") {
        QString text = tr("Receipt printed");
        if (o.contains("warm")) {
            text += "<h1><font color=\"red\">";
            text += "\r\n";
            text += "\r\n";
            text += "!" + o["warm"].toString();
            text += "</font></h1>";
        }
        msg(text);
        fDrv->m_header.f_printQty = abs(fDrv->m_header.f_printQty) + 1;
        LogThread::logOrderThread(fDrv->m_header.f_currStaffName, fDrv->m_header.f_id, "", "Print prepayment check", "");
        return;
    }
    msg(o["reply"].toString());
}

void DlgPayment::on_btnARCA_clicked()
{
    payment(PaymentArca);
}

void DlgPayment::on_btnMASTER_clicked()
{
    payment(PaymentMaster);
}

void DlgPayment::on_btnVISA_clicked()
{
    payment(PaymentVisa);
}

void DlgPayment::on_btnMAESTRO_clicked()
{
    payment(PaymentMaestro);
}

void DlgPayment::on_btnIDRAM_clicked()
{
    payment(PaymentIdram);
}

void DlgPayment::on_btnJazzveGift_clicked()
{
    payment(PaymentJazzveGift);
}

void DlgPayment::on_btnCancel_clicked()
{
    reject();
}

void DlgPayment::on_btnPrepaymentCash_clicked()
{
    receipt(1);
}

void DlgPayment::on_btnPrepaymentNoCash_clicked()
{
    receipt(2);
}

void DlgPayment::on_leCash_textChanged(const QString &arg1)
{
    QString num = arg1;
    if (num.toDouble() > ui->leAmount->text().toDouble()) {
        num = ui->leAmount->text();
    }
    ui->leCard->setText(ui->leAmount->text().toDouble() - num.toDouble());
}

void DlgPayment::on_leCard_textChanged(const QString &arg1)
{
    QString num = arg1;
    if (num.toDouble() > ui->leAmount->text().toDouble()) {
        num = ui->leAmount->text();
    }
    ui->leCash->setText(ui->leAmount->text().toDouble() - num.toDouble());
}

void DlgPayment::on_btnClearCash_clicked()
{
    ui->leCash->clear();
}

void DlgPayment::on_btnClearCard_clicked()
{
    ui->leCard->clear();
}

void DlgPayment::on_btnSetCash_clicked()
{
    QString num;
    if (!DlgGetPassword::password(tr("Cash"), num, false, this)) {
        return;
    }
    on_leCash_textChanged(num);
}

void DlgPayment::on_btnCard_clicked()
{
    QString num;
    if (!DlgGetPassword::password(tr("Cash"), num, false, this)) {
        return;
    }
    on_leCard_textChanged(num);
}

void DlgPayment::on_btnComplimentary_clicked()
{
    fDrv->openDB();
    fDrv->prepare("delete from o_tax where fid=:fid");
    fDrv->bindValue(":fid", fDrv->m_header.f_id);
    fDrv->execSQL();
    fDrv->prepare(QString("insert into o_tax (fid, ffiscal, fnumber, fhvhh, fcash, fcard, json) "
               "values (:fid, '%1', '%1', '%1', 0, 0, '%1')").arg(tr("Comp.")));
    fDrv->bindValue(":fid", fDrv->m_header.f_id);
    fDrv->execSQL();
    fDrv->closeDB();
}

void DlgPayment::on_btnPrepaymentCash_2_clicked()
{
    if (DlgMessage::Msg(QString("%1").arg(tr("Confirm to cancel fiscal"))) != QDialog::Accepted) {
        return;
    }

    MPTcpSocket fTcpSocket;
    fTcpSocket.setServerIP(__cnfmaindb.fServerIP);
    fTcpSocket.setValue("session", SESSIONID);
    fTcpSocket.setValue("query", "taxcancel");
    fTcpSocket.setValue("order", fDrv->m_header.f_id);
    QJsonObject o = fTcpSocket.sendData();
    if (o["reply"].toString() == "ok") {
        msg(tr("Fiscal canceled"));
    } else {
        msg(o["reply"].toString());
    }
}

void DlgPayment::on_btnChange_clicked()
{
    DlgCalcChange *d = new DlgCalcChange(fDrv->m_header.f_amount, this);
    d->exec();
    delete d;
}
