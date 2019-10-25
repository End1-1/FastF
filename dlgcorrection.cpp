#include "dlgcorrection.h"
#include "ui_dlgcorrection.h"
#include "od_dish.h"
#include "dlgmessage.h"
#include "od_drv.h"
#include "printing.h"
#include "dlgremovereason.h"
#include "dlgdishcomment.h"
#include "ff_settingsdrv.h"
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
    m_ord->openDB();
    m_ord->prepare("select state_id, date_start, request_correction, request_correction_qty from o_dishes_reminder where record_id=:record_id");
    m_ord->bindValue(":record_id", fDish->f_id);
    m_ord->execSQL();
    if (m_ord->next()) {
        if (m_ord->v_int(0) > 1) {
            ui->lbl2->setEnabled(true);
            QDateTime dstarted = m_ord->v_dateTime(1);
            int ms = (dstarted.msecsTo(QDateTime::currentDateTime()) / 1000) / 60;
            if (ms > FF_SettingsDrv::value(SD_DISH_CORRECTION_RED_TIMEOUT).toInt()) {
                ui->lbl3->setEnabled(true);
            }
            if (m_ord->v_int(0) > 2) {
                ui->lbl3->setEnabled(true);
            }
        }
        fRequestId = m_ord->v_int(2);
        if (fRequestId > 0) {
            ui->lbQtyRemove->setText(double_str(m_ord->v_dbl(3)));
            fQty = m_ord->v_dbl(3);
            m_ord->prepare("select remove_reason from o_dishes where id=:id");
            m_ord->bindValue(":id", fDish->f_id);
            m_ord->execSQL();
            if (m_ord->next()) {
                ui->ptReason->setPlainText(m_ord->v_str(0));
            }
            setWaitMode();
            checkResponse(fRequestId);
        }
    } else {

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
    setEnabled(false);
    m_ord->prepare("update o_dishes set remove_reason=:remove_reason where id=:id");
    m_ord->bindValue(":remove_reason", ui->ptReason->toPlainText());
    m_ord->bindValue(":id", fDish->f_id);
    m_ord->execSQL();
    m_ord->closeDB();
    QNet *n = new QNet(this);
    connect(n, SIGNAL(getResponse(QString,bool)), SLOT(parseRequestForCorrection(QString,bool)));
    n->URL = "http://www.jazzve.am/cafe4/request_for_correction.php";
    n->addData("reqpass", "glibalda");
    n->addData("reqpass", "glibalda");
    n->addData("cafe", m_ord->mfOrderIdPrefix);
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
    setEnabled(false);
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
        LogThread::logOrderThread(m_ord->m_header.f_currStaffId, m_ord->m_header.f_id, tr("Removed without print"), fDish->f_dishName + ": " + double_str(qtyNoPrint) + ", " + ui->ptReason->toPlainText());
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
            dc->f_stateId = DISH_STATE_REMOVED_PRINTED;
            m_ord->appendDish(dc);
        }
        LogThread::logOrderThread(m_ord->m_header.f_currStaffId, m_ord->m_header.f_id, tr("Removed with print "), fDish->f_dishName + ": " + double_str(fQty) + ", " + ui->ptReason->toPlainText());
        printRemoved(fQty);
    }
    m_ord->prepare("update o_dishes set state_id=:state_id, qty=:qty, printed_qty=:printed_qty, remove_reason=:remove_reason where id=:id");
    m_ord->bindValue(":state_id", fDish->f_stateId);
    m_ord->bindValue(":qty", fDish->f_totalQty);
    m_ord->bindValue(":printed_qty", fDish->f_printedQty);
    m_ord->bindValue(":remove_reason", ui->ptReason->toPlainText());
    m_ord->bindValue(":id", fDish->f_id);
    m_ord->execSQL();
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
   setEnabled(true);
   int newID = response.toInt();
   if (newID == 0) {
       DlgMessage::Msg(tr("Request error") + "\r\n" + response);
       return;
   }
   fRequestId = newID;
   m_ord->prepare("update o_dishes_reminder set request_correction=:request_correction, request_correction_qty=:request_correction_qty where record_id=:record_id");
   m_ord->bindValue(":request_correction", newID);
   m_ord->bindValue(":request_correction_qty", ui->lbQtyRemove->text().toDouble());
   m_ord->bindValue(":record_id", fDish->f_id);
   m_ord->execSQL();
   m_ord->closeDB();
   setWaitMode();
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
        on_btnOk_clicked();
    } else {
        setEnabled(true);
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
