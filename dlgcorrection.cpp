#include "dlgcorrection.h"
#include "ui_dlgcorrection.h"
#include "od_dish.h"
#include "dlgmessage.h"
#include "od_drv.h"
#include "printing.h"
#include "dlgremovereason.h"
#include "dlgdishcomment.h"
#include "ff_settingsdrv.h"
#include "cnfapp.h"
#include "logthread.h"
#include "qnet.h"
#include "dlgapprovecorrectionmessage.h"
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>

DlgCorrection::DlgCorrection(OD_Drv *drv, OD_Dish *d, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DlgCorrection)
{
    ui->setupUi(this);

    ui->tblLetter->setVisible(false);
    adjustSize();
    ui->tblLetter->setItemDelegate(new DlgDishComment::QLetterDelegate());
    QStringList row1, row2, row3, row4, row5;
    row1 << U("1") << U("2") << U("3") << U("4") << U("5") << U("6") << U("7") << U("8") << U("9") << U("0") << U("=") << U(".") << U("<");
    row2 << U("Է") << U("Թ") << U("Փ") << U("Ձ") << U("Ջ") << U("Ւ") << U("և") << U("Ր") << U("Չ") << U("Ճ") << U("-") << U("Ժ") << U(" ");
    row3 << U("Ք") << U("Ո") << U("Ե") << U("Ռ") << U("Տ") << U("Տ") << U("Ը") << U("Ւ") << U("Ի") << U("Օ") << U("Պ") << U("Խ") << U("Ծ");
    row4 << U("Ա") << U("Ս") << U("Դ") << U("Ֆ") << U("Գ") << U("Հ") << U("Յ") << U("Կ") << U("Լ") << U(":") << U("Շ") << U(" ") << U(" ");
    row5 << U("Զ") << U("Ղ") << U("Ց") << U("Վ") << U("Բ") << U("Ն") << U("Մ") << U("")  << U("")  << U("?") << U(" ") << U(" ") << U(" ");
    fLetters.insert(0, row1);
    fLetters.insert(1, row2);
    fLetters.insert(2, row3);
    fLetters.insert(3, row4);
    fLetters.insert(4, row5);

    int col = 0, row = 0;
    for (QMap<int,QStringList>::const_iterator it = fLetters.begin(); it != fLetters.end(); it++) {
        col = 0;
        ui->tblLetter->setRowCount(it.key() + 1);
        ui->tblLetter->setColumnCount(it.value().count() > ui->tblLetter->columnCount() ? it.value().count() : ui->tblLetter->columnCount());
        for (QStringList::const_iterator jt = it.value().begin(); jt != it.value().end(); jt++)
            ui->tblLetter->setItem(it.key(), col++, new QTableWidgetItem(*jt));
        row++;
    }

    fDish = d;
    m_ord = drv;
    ui->lbDishName->setText(d->f_dishName);
    ui->lbQty->setText(double_str(d->f_totalQty));
    fQty = fDish->f_totalQty;
    setQtyRemove();
    move(pos().x(), 0);
}

DlgCorrection::~DlgCorrection()
{
    delete ui;
}

void DlgCorrection::on_btnMinus_clicked()
{
    fQty -= 1;
    if (fQty < 0) {
        fQty = 0;
    }
    setQtyRemove();
}

void DlgCorrection::setQtyRemove()
{
    ui->lbQtyRemove->setText(double_str(fQty));
    ui->lbl1->setEnabled(true);
    if (m_ord->m_header.f_printQty > 0) {
        ui->lbl1->setEnabled(true);
        ui->lbl2->setEnabled(true);
        ui->lbl3->setEnabled(true);
    } else {
        m_ord->openDB();
        m_ord->prepare("select state_id, date_start from o_dishes_reminder where record_id=:record_id");
        m_ord->bindValue(":record_id", fDish->f_id);
        m_ord->execSQL();
        if (m_ord->next()) {
            if (m_ord->v_int(0) > 1 && m_ord->v_int(0) < 5) {
                ui->lbl2->setEnabled(true);
                QDateTime dstarted = m_ord->v_dateTime(1);
                int ms = (dstarted.msecsTo(QDateTime::currentDateTime()) / 1000) / 60;
                if (ms > FF_SettingsDrv::value(SD_DISH_CORRECTION_RED_TIMEOUT).toInt()) {
                    ui->lbl3->setEnabled(true);
                }
                if (m_ord->v_int(0) > 2 && m_ord->v_int(0) < 5) {
                    ui->lbl3->setEnabled(true);
                }
            }
        } else {

        }
    }
    m_ord->prepare("select frequest, freason, fqty from o_request_correction where frecord=:frecord and fstate=0");
    m_ord->bindValue(":frecord", fDish->f_id);
    m_ord->execSQL();
    if (m_ord->next()) {
        fRequestId = m_ord->v_int(0);
        ui->lbQtyRemove->setText(double_str(m_ord->v_dbl(2)));
        fQty = m_ord->v_dbl(2);
        ui->ptReason->setPlainText(m_ord->v_str(1));
        if (fRequestId > 0) {
            setWaitMode();
            checkResponse(fRequestId);
        }
    }
    m_ord->closeDB();
}

void DlgCorrection::printRemoved(double qty)
{
    QStringList prn;
    if (!fDish->f_print1.isEmpty()) {
        prn << fDish->f_print1;
    }
    if (!fDish->f_print2.isEmpty()) {
        prn << fDish->f_print2;
    }
    for (QStringList::const_iterator it = prn.begin(); it != prn.end(); it++) {
        if (!___printerInfo->printerExists(*it))
            continue;
        SizeMetrics sm(___printerInfo->resolution(*it));
        XmlPrintMaker xp(&sm);
        int top = 3;

        xp.setFontName(qApp->font().family());
        xp.setFontSize(8);

        xp.text(tr("Removed from order"), 1, top);
        top += xp.lastTextHeight() + 1;
        xp.text(tr("Order number"), 1, top);
        xp.textRightAlign(m_ord->m_header.f_id, page_width, top);
        top += xp.lastTextHeight() + 1;
        xp.text(tr("Table"), 1, top);
        xp.textRightAlign(m_ord->m_header.f_tableName, page_width, top);
        top += xp.lastTextHeight() + 1;
        xp.text(tr("Staff"), 1, top);
        xp.textRightAlign(m_ord->m_header.f_currStaffName, page_width, top);
        top += xp.lastTextHeight() + 1;
        xp.text(tr("Date"), 1, top);
        xp.textRightAlign(QDateTime::currentDateTime().toString(DATETIME_FORMAT), page_width, top);
        top += xp.lastTextHeight() + 1;
        top ++;
        xp.line(1, top, page_width, top);
        top ++;
        xp.text(fDish->f_dishName, 1, top);
        top += xp.lastTextHeight() + 1;
        xp.textRightAlign(double_str(qty), page_width, top);
        top += xp.lastTextHeight() + 2;
        xp.text(ui->ptReason->toPlainText(), 1, top);
        top += xp.lastTextHeight() + 1;
        top ++;
        xp.line(1, top, page_width, top);
        top ++;
        top ++;
        xp.text(".", 1, top);
        xp.finishPage();

        ThreadPrinter *tp = new ThreadPrinter(*it, sm, xp);
        tp->start();
    }
}

void DlgCorrection::requestForCorrection(double qty)
{
    setEnabledWidget(false);
    m_ord->prepare("insert into o_request_correction (fstate, frecord, frequest, freason, fqty) values (0, :frecord, 0, :freason, :fqty)");
    m_ord->bindValue(":frecord", fDish->f_id);
    m_ord->bindValue(":freason", ui->ptReason->toPlainText());
    m_ord->bindValue(":fqty", qty);
    m_ord->execSQL();
    m_ord->closeDB();
    QNet *n = new QNet(this);
    connect(n, SIGNAL(getResponse(QString,bool)), SLOT(parseRequestForCorrection(QString,bool)));
    n->URL = __cnfapp.exchangeServer() + "/request_for_correction.php";
    n->addData("reqpass", __cnfapp.exchangePassword());
    n->addData("reqpass", __cnfapp.exchangePassword());
    n->addData("cafe", __cnfapp.exchangeIdentifyBy());
    n->addData("order", m_ord->m_header.f_id);
    n->addData("record", QString::number(fDish->f_id));
    n->addData("user", m_ord->m_header.f_currStaffName);
    n->addData("reason", ui->ptReason->toPlainText());
    n->addData("item", fDish->f_dishName);
    n->addData("qty", double_str(qty));
    n->go();
}

void DlgCorrection::checkResponse(int id)
{
    setEnabledWidget(false);
    QNet *n = new QNet(this);
    connect(n, SIGNAL(getResponse(QString,bool)), SLOT(parseRequestResponse(QString,bool)));
    n->URL = "http://www.jazzve.am/cafe4/check_request_response.php";
    n->addData("reqpass", "glibalda");
    n->addData("reqpass", "glibalda");
    n->addData("id", QString::number(id));
    n->go();
}

void DlgCorrection::on_btnPlus_clicked()
{
    fQty += 1;
    if (fQty > fDish->f_totalQty) {
        fQty = fDish->f_totalQty;
    }
    setQtyRemove();
}

void DlgCorrection::on_btnReject_clicked()
{
    reject();
}

void DlgCorrection::on_btnOk_clicked()
{
    if (fQty < 0.001) {
        DlgMessage::Msg(tr("Incorrect qty to remove"));
        return;
    }
    if (!ui->btnLossYes->isChecked() && !ui->btnLossNo->isChecked()) {
        DlgMessage::Msg(tr("No one of loss option was selected"));
        return;
    }
    if (ui->ptReason->toPlainText().trimmed().isEmpty()) {
        DlgMessage::Msg(tr("Reason cannot be empty"));
        return;
    }
    double qtyNoPrint = fDish->f_totalQty - fDish->f_printedQty;
    if (qtyNoPrint > 0.001) {
        if (qtyNoPrint >= fQty) {
            qtyNoPrint = fQty;
            fDish->f_totalQty -= fQty;
            fQty -= qtyNoPrint;
            if (fDish->f_totalQty < 0.001) {
                fDish->f_stateId = 0;
            }
        } else {
            fDish->f_totalQty -= qtyNoPrint;
            fQty -= qtyNoPrint;
        }
        LogThread::logOrderThread(m_ord->m_header.f_currStaffName, m_ord->m_header.f_id, QString::number(fDish->f_id), tr("Removed without print"), fDish->f_dishName + ": " + double_str(qtyNoPrint) + ", " + ui->ptReason->toPlainText());
    }
    if (fQty > 0.001) {
        if (ui->lbl3->isEnabled()) {
            requestForCorrection(fQty);
            return;
        }
        if (fQty == fDish->f_totalQty) {
            fDish->f_stateId = DISH_STATE_REMOVED_PRINTED;
        } else {
            fDish->f_totalQty -= fQty;
            if (fDish->f_printedQty > fDish->f_totalQty) {
                fDish->f_printedQty = fDish->f_totalQty;
            }
            OD_Dish *dc = fDish->copy();
            dc->f_totalQty = fQty;
            dc->f_printedQty = fQty;
            dc->f_storestate =ui->btnLossYes->isChecked() ? 1 : 0;
            dc->f_removeReason = ui->ptReason->toPlainText();
            dc->f_stateId = DISH_STATE_REMOVED_PRINTED;
            m_ord->appendDish(dc);
        }
        LogThread::logOrderThread(m_ord->m_header.f_currStaffName, m_ord->m_header.f_id, QString::number(fDish->f_id), tr("Removed with print "), fDish->f_dishName + ": " + double_str(fQty) + ", " + ui->ptReason->toPlainText());
        printRemoved(fQty);
    }
    m_ord->prepare("update o_dishes set state_id=:state_id, qty=:qty, f_removereason=:f_removereason, f_storestate=:f_storestate, printed_qty=:printed_qty where id=:id");
    m_ord->bindValue(":state_id", fDish->f_stateId);
    m_ord->bindValue(":qty", fDish->f_totalQty);
    m_ord->bindValue(":printed_qty", fDish->f_printedQty);
    m_ord->bindValue(":f_storestate", (fDish->f_stateId != DISH_STATE_NORMAL && ui->btnLossYes->isChecked()) ? 1 : 0);
    m_ord->bindValue(":f_removereason", ui->ptReason->toPlainText());
    m_ord->bindValue(":id", fDish->f_id);
    m_ord->execSQL();

    if (fDish->f_stateId != DISH_STATE_NORMAL) {
        m_ord->prepare("update o_dishes_reminder set state_id=5, date_removed=:date_removed where record_id=:record_id");
        m_ord->bindValue(":record_id", fDish->f_id);
        m_ord->bindValue(":date_removed", QDateTime::currentDateTime());
        m_ord->execSQL();
    }
    accept();
}

void DlgCorrection::on_btnRemoveReason_clicked()
{
    if (!m_ord->openDB())
        return;
    m_ord->prepare("select id, name from o_remove_reason order by name");
    m_ord->execSQL();
    QMap<int, QString> rv;
    while (m_ord->next()) {
        rv[m_ord->v_int(0)] = m_ord->v_str(1);
    }
    DlgRemoveReason *dlg = new DlgRemoveReason(rv, this);
    dlg->exec();
    QString reasonName = dlg->m_reasonName;
    delete dlg;
    ui->ptReason->setPlainText(reasonName);
}

void DlgCorrection::on_btnClearWindow_clicked()
{
    ui->ptReason->clear();
}

void DlgCorrection::on_btnKeyboard_clicked()
{
    ui->tblLetter->setVisible(ui->btnKeyboard->isChecked());
    adjustSize();
}

void DlgCorrection::on_tblLetter_itemClicked(QTableWidgetItem *item)
{
    if (!item) {
        return;
    }
    if (item->text() == "<") {
        ui->ptReason->setPlainText(ui->ptReason->toPlainText().remove(ui->ptReason->toPlainText().length() - 1, 1));
    } else {
        ui->ptReason->setPlainText(ui->ptReason->toPlainText() + item->text());
    }
}

void DlgCorrection::parseRequestForCorrection(const QString &response, bool result)
{
   setEnabledWidget(true);
   int newID = response.toInt();
   if (newID == 0) {
       DlgMessage::Msg(tr("Request error") + "\r\n" + response);
       return;
   }
   fRequestId = newID;
   m_ord->prepare("update o_request_correction set frequest=:frequest where frecord=:frecord and fstate=0");
   m_ord->bindValue(":frequest", newID);
   m_ord->bindValue(":frecord", fDish->f_id);
   m_ord->execSQL();
   m_ord->closeDB();
   setWaitMode();
   LogThread::logOrderThread(m_ord->m_header.f_currStaffName, m_ord->m_header.f_id, QString::number(fDish->f_id), tr("Request for correction"), fDish->f_dishName + ": " + ui->lbQtyRemove->text());
   DlgMessage::Msg(tr("Request created. Wait for response."));
}

void DlgCorrection::parseRequestResponse(const QString &response, bool result)
{
    QJsonDocument jd = QJsonDocument::fromJson(response.toUtf8());
    QJsonArray ja = jd.array();
    if (ja.count() == 0) {
        DlgMessage::Msg(tr("Response error.") + "\r\n" + response);
        return;
    }
    bool approve = false;
    //Check for super role
    for (int i = 0; i < ja.count(); i++) {
        QJsonObject jo = ja.at(i).toObject();
        if (jo["role"].toInt() == 1) {
            if (jo["ok"].toInt() == 1) {
                approve = true;
                break;
            }
        }
    }
    //Check for other role
    if (!approve) {
        bool approve2 = true;
        bool checked = false;
        for (int i = 0; i < ja.count(); i++) {
            QJsonObject jo = ja.at(i).toObject();
            if (jo["role"].toInt() == 1) {
                continue;
            } else {
                checked = true;
                if (jo["ok"].toInt() != 1) {
                    approve2 = false;
                    break;
                }
            }
        }
        if (checked) {
            approve = approve2;
        }
    }
    DlgApproveCorrectionMessage *d = new DlgApproveCorrectionMessage(ja, approve, this);
    d->exec();
    delete d;
    if (approve) {
        ui->wButtons->setEnabled(true);
        ui->btnClearWindow->setEnabled(false);
        ui->btnCheckResponse->setEnabled(false);
        ui->btnOk->setEnabled(true);
        ui->btnReject->setEnabled(true);
        on_btnOk_clicked();
    } else {
        setEnabledWidget(true);
        setWaitMode();
    }
}

void DlgCorrection::setWaitMode()
{
    ui->btnRemoveReason->setEnabled(false);
    ui->btnKeyboard->setEnabled(false);
    ui->btnMinus->setEnabled(false);
    ui->btnPlus->setEnabled(false);
    ui->btnClearWindow->setEnabled(false);
    ui->btnOk->setEnabled(false);
    ui->btnCheckResponse->setEnabled(true);
}

void DlgCorrection::on_btnCheckResponse_clicked()
{
    checkResponse(fRequestId);
}
void DlgCorrection::on_btnLossYes_clicked()
{
    ui->btnLossNo->setChecked(false);
}

void DlgCorrection::on_btnLossNo_clicked()
{
    ui->btnLossYes->setChecked(false);
}

void DlgCorrection::setEnabledWidget(bool v)
{
    ui->wButtons->setEnabled(v);
    ui->wName->setEnabled(v);
    ui->wReason->setEnabled(v);
}
