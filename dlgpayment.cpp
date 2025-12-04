#include "dlgpayment.h"
#include "MobilePointServer/databaseresult.h"
#include "msqldatabase.h"
#include "mtfilelog.h"
#include "qprinterinfo.h"
#include "ui_dlgpayment.h"
#include "logthread.h"
#include "mdefined.h"
#include "dlgmessage.h"
#include "mtprintkitchen.h"
#include "idram.h"
#include "ff_settingsdrv.h"
#include "dlggetpassword.h"
#include "dlgcalcchange.h"
#include "mjsonhandler.h"
#include "ff_user.h"
#include "cnfapp.h"
#include <QJsonObject>

DlgPayment::DlgPayment(FF_User *u, OD_Drv *drv, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DlgPayment),
    fUser(u)
{
    ui->setupUi(this);
    ui->btnComplimentary->setEnabled(u->m_groupId.toInt() <= 2);
    ui->btnFiscalCancel->setEnabled(u->m_groupId.toInt() <= 2);
    fDrv = drv;
    ui->leTable->setText(fDrv->m_header.f_tableName);
    ui->leStaff->setText(fDrv->m_header.f_currStaffName);
    ui->leAmount->setText(fDrv->m_header.f_amount);
    fDrv->m_dbDrv.prepare("select fcash, fcard from o_tax where fid=:fid");
    fDrv->m_dbDrv.bindValue(":fid", fDrv->m_header.f_id);
    fDrv->m_dbDrv.execSQL();
    fDrv->prepare("select f_code from o_gift_card where f_order=:f_order");
    fDrv->bindValue(":f_order", fDrv->m_header.f_id);
    fDrv->execSQL();

    if(fDrv->next()) {
        DbDriver db;
        db.configureDb("10.1.0.2", "maindb", "SYSDBA", "masterkey");

        if(!db.openDB()) {
            DlgMessage::Msg("Cannot connect to main server!");
            return;
        }

        db.prepare("select sum(f_amount) from o_gift_card where f_code=:f_code");
        db.bindValue(":f_code", fDrv->v_str(0));
        db.execSQL();

        if(!db.next()) {
            DlgMessage::Msg(tr("Invalid gift card"));
            return;
        }

        ui->btnCash->setEnabled(false);
        ui->btnIDRAM->setEnabled(false);
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
    QJsonObject jo;
    jo["session"] = fDrv->m_header.f_currStaffId;
    jo["query"] = "closeorder";
    jo["staff"] = fDrv->m_header.f_currStaffId;
    jo["mode"] = mode;
    jo["order"] = fDrv->m_header.f_id;
    jo["printer"] = FF_SettingsDrv::value(SD_CHECK_PRINTER_NAME).toString();
    MJsonHandler jh;
    jo = jh.handleCloseOrder(jo);

    if(jo["reply"].toString() != "ok") {
        msg(jo["reply"].toString());
        return;
    }

    if(!jo.contains("reply")) {
        msg(tr("Unknown error"));
        return;
    }

    if(jo["reply"] == "ok") {
        fDrv->m_header.f_stateId = ORDER_STATE_CLOSED;
        LogThread::logOrderThread(fDrv->m_header.f_currStaffName, fDrv->m_header.f_id, "", "Order closed", "");
        accept();
        return;
    }

    msg(jo["reply"].toString());
}

void DlgPayment::receipt(int mode)
{
    ui->btnCash->setEnabled(false);
    ui->btnCard->setEnabled(false);
    ui->btnPrepaymentCash->setEnabled(false);
    QJsonObject jo;
    jo["session"] = fDrv->m_header.f_currStaffId;
    jo["query"] = "printreceipt";
    jo["staff"] = fDrv->m_header.f_currStaffId;
    jo["order"] = fDrv->m_header.f_id;
    jo["printer"] = FF_SettingsDrv::value(SD_CHECK_PRINTER_NAME).toString();
    jo["mode"] = mode;
    jo["tax"] = ui->leTax->text();
    MJsonHandler jh;
    QJsonObject o = jh.handleReceipt(jo);
    ui->btnCash->setEnabled(true);
    ui->btnCard->setEnabled(true);
    ui->btnPrepaymentCash->setEnabled(true);

    if(!o.contains("reply")) {
        msg(tr("Unknown error"));
        return;
    }

    if(o["reply"] == "ok") {
        QString text = tr("Receipt printed");

        if(o.contains("warm")) {
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

void DlgPayment::printPrecheck(const QString &qr)
{
    QString order = fDrv->m_header.f_id;

    if(order.isEmpty()) {
        msg(tr("Order cannot be empty"));
        return;
    }

    QMap<QString, QVariant> v;
    DatabaseResult dr;
    v[":id"] = order;
    MSqlDatabase fDb;
    fDb.select("select print_qty from o_order where id=:id", v, dr, false);

    if(dr.value("PRINT_QTY").toInt() > 0) {
        /* Check for time , after 30 minutes deny any print */
        DatabaseResult dct;
        v[":f_order"] = order;
        MTFileLog::createLog(__LOG_RECEIPT, order + ", mark 2");
        fDb.select("select f_date, f_time from o_log where f_order=:f_order and f_action='Print prepayment check'", v, dct, false);

        if(dct.rowCount() > 0 && fUser->id != 1) {
            if(QDate::fromString(dct.toString(0, "F_DATE"), "yyyy-MM-dd") != QDate::currentDate()) {
                msg(tr("Cannot print receipt, becouse date is not equal current date"));
                return;
            }

            qDebug() << QTime::fromString(dct.toString(0, "F_TIME"), "HH:mm:ss").secsTo(QTime::currentTime());

            if(QTime::fromString(dct.toString(0, "F_TIME"), "HH:mm:ss").secsTo(QTime::currentTime()) > 60 * 30)  {
                msg(tr("Cannot print receipt, becouse time up"));
                return;
            }
        }

        v[":id"] = fUser->id;
        DatabaseResult dp;
        fDb.select("select group_id from employes where id=:id", v, dp, false);

        if(dp.rowCount() == 0) {
            msg(tr("Cannot print multiple receipt\r for this order by you"));
            return;
        }

        if(dp.value("GROUP_ID").toInt() != 2 && dp.value("GROUP_ID").toInt() != 1) {
            msg(tr("Cannot print multiple receipt\r for this order by you"));
            return;
        }
    }

    if(dr.rowCount() == 0) {
        msg(tr("Incorrect order number"));
        return;
    }

    v[":id"] = order;
    fDb.select("select t.name as tname, h.name as hname, e.fname || ' ' || e.lname as ename, "
               "o.date_cash, o.amount, o.amount_inc, o.amount_dec, o.amount_inc_value, o.amount_dec_value "
               "from o_order o "
               "left join h_table t on t.id=o.table_id "
               "left join employes e on e.id=o.staff_id "
               "left join h_hall h on h.id=t.hall_id "
               "where o.id=:id ", v, dr, false);

    if(dr.rowCount() == 0) {
        msg(tr("Unable to select order with id: ") + order);
        return;
    }

    double total;
    QMap<QString, QString> data;
    data["table"] = dr.value("TNAME").toString();
    data["hall"] = dr.value("HNAME").toString();
    data["staff"] = dr.value("ENAME").toString();
    data["date"] = dr.value("DATE_CASH").toDate().toString(def_date_format);
    data["printer"] = FF_SettingsDrv::value(SD_CHECK_PRINTER_NAME).toString();
    data["order"] = order;
    total = dr.value("AMOUNT").toDouble();
    data["amount"] = float_str(total, 2);
    data["amount_inc"] = float_str(dr.value("AMOUNT_INC").toDouble(), 2);
    data["amount_dec"] = float_str(dr.value("AMOUNT_DEC").toDouble(), 2);
    data["amount_inc_value"] = float_str(dr.value("AMOUNT_INC_VALUE").toDouble() * 100, 2);
    data["amount_dec_value"] = float_str(dr.value("AMOUNT_DEC_VALUE").toDouble() * 100, 2);
    QList<QMap<QString, QString> > dishes;
    v[":order_id"] = order;
    v[":state_id"] = DISH_STATE_NORMAL;
    fDb.select("select d.name, od.qty, od.price, od.qty*od.price as total, "
               "od.qty-od.printed_qty as printed,mt.adgcode, d.id as dishid, "
               "od.payment_mod as pm, od.emarks "
               "from o_dishes od "
               "left join me_dishes d on d.id=od.dish_id "
               "left join me_types mt on mt.id=d.type_id "
               "where od.order_id=:order_id and od.state_id=:state_id ",
               v, dr, false);

    for(int i = 0; i < dr.rowCount(); i++) {
        if(dr.value(i, "PRINTED").toDouble() > 0.1) {
            msg(tr("Incomplete order"));
            return;
        }

        QMap<QString, QString> dish;
        dish["dish"] = dr.value(i, "NAME").toString();
        dish["qty"] = float_str(dr.value(i, "QTY").toDouble(), 1);
        dish["price"] = float_str(dr.value(i, "PRICE").toDouble(), 1);
        dish["total"] = float_str(dr.value(i, "TOTAL").toDouble(), 1);
        dish["adgcode"] = dr.value(i, "ADGCODE").toString();
        dish["id"] = dr.value(i, "DISHID").toString();
        dish["pm"] = dr.value(i, "PM").toString();
        dish["emarks"] = dr.value(i, "EMARKS").toString();
        dishes.append(dish);
    }

    v[":id"] = order;
    DatabaseResult doh;
    fDb.select("select state_id from o_order where id=:id", v, doh, false);

    if(doh.rowCount() == 0) {
        msg("Error. no order id");
        return;
    }

    v[":id"] = order;
    fDb.select("update o_order set print_qty=abs(print_qty)+1 where id=:id", v, dr, false);
    fDrv->m_header.f_printQty = abs(fDrv->m_header.f_printQty) + 1;
    MTPrintKitchen m(data, dishes, false, qr, nullptr);
    m.run();
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
    fDrv->openDB();
    fDrv->prepare("select * from o_idram where fid=:fid");
    fDrv->bindValue(":fid", fDrv->m_header.f_id);
    fDrv->execSQL();

    if(fDrv->next()) {
        auto idramOrder = fDrv->v_str(1);
        auto *idram = new IDram(this);
        idram->requestToken([this, idram, idramOrder]() {
            idram->requestCheckPayment(idramOrder, [this, idram](const QJsonObject & jo) {
                QJsonObject jp = jo["value"].toObject();

                if(jp["paymentStatusId"].toInt() == 8) {
                    payment(PaymentIdram);
                } else {
                    msg(tr("Payment not complited"));
                }

                idram->deleteLater();
            });
        }, [](const QString & e) {
            msg(e);
        });
    }
}

void DlgPayment::on_btnCancel_clicked()
{
    reject();
}

void DlgPayment::on_btnPrepaymentCash_clicked()
{
    fDrv->openDB();
    fDrv->prepare("select * from o_idram where fid=:fid");
    fDrv->bindValue(":fid", fDrv->m_header.f_id);
    fDrv->execSQL();

    if(fDrv->next()) {
        printPrecheck(fDrv->v_str(2));
    } else {
        auto *idram = new IDram(this);
        idram->requestToken([this, idram]() {
            idram->requestQr(ui->leAmount->text().toDouble(), [this, idram](const QString & idramOrder, const QString & qrContent) {
                fDrv->prepare("insert into  o_idram(fid, fidram, fqr) values (:fid, :fidram, :fqr)");
                fDrv->bindValue(":fid", fDrv->m_header.f_id);
                fDrv->bindValue(":fidram", idramOrder);
                fDrv->bindValue(":fqr", qrContent);
                fDrv->execSQL();
                printPrecheck(qrContent);
                idram->deleteLater();
            }, [this](const QString & e) {
                msg(e);
                printPrecheck("");
            });
        }, [this](const QString & e) {
            msg(e);
            printPrecheck("");
        });
    }
}

void DlgPayment::on_btnPrepaymentNoCash_clicked()
{
    receipt(2);
}

void DlgPayment::on_btnComplimentary_clicked()
{
    fDrv->openDB();
    fDrv->prepare("delete from o_tax where fid=:fid");
    fDrv->bindValue(":fid", fDrv->m_header.f_id);
    fDrv->execSQL();
    fDrv->prepare(QString("insert into o_tax (fid, ffiscal, fnumber, fhvhh, fcash, fcard, fidram, fcomplimentary, jsonin, json) "
                          "values (:fid, '%1', '%1', '%1', 0, 0, 0, :fcomplimentary, '%1', '%1')").arg(tr("Comp.")));
    fDrv->bindValue(":fid", fDrv->m_header.f_id);
    fDrv->bindValue(":fcomplimentary", fDrv->m_header.f_amount);
    fDrv->execSQL();
    fDrv->closeDB();
    payment(PaymentComplimentary);
}

void DlgPayment::on_btnChange_clicked()
{
    DlgCalcChange *d = new DlgCalcChange(fDrv->m_header.f_amount, this);
    d->exec();
    delete d;
}

void DlgPayment::on_btnCard_clicked()
{
    payment(PaymentArca);
}

void DlgPayment::on_btnFiscalCancel_clicked()
{
    if(DlgMessage::Msg(QString("%1").arg(tr("Confirm to cancel fiscal"))) != QDialog::Accepted) {
        return;
    }

    QJsonObject jo;
    jo["session"] = fDrv->m_header.f_currStaffId;
    jo["query"] = "taxcancel";
    jo["order"] = fDrv->m_header.f_id;
    MJsonHandler jh;
    QJsonObject o = jh.handleTaxCancel(jo);

    if(o["reply"].toString() == "ok") {
        msg(tr("Fiscal canceled"));
    } else {
        msg(o["reply"].toString());
    }
}
