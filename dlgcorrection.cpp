#include "dlgcorrection.h"
#include "c5printing.h"
#include "ui_dlgcorrection.h"
#include "od_dish.h"
#include "dlgmessage.h"
#include "od_drv.h"
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

    for(QMap<int, QStringList>::const_iterator it = fLetters.begin(); it != fLetters.end(); it++) {
        col = 0;
        ui->tblLetter->setRowCount(it.key() + 1);
        ui->tblLetter->setColumnCount(it.value().count() > ui->tblLetter->columnCount() ? it.value().count() : ui->tblLetter->columnCount());

        for(QStringList::const_iterator jt = it.value().constBegin(); jt != it.value().constEnd(); jt++)
            ui->tblLetter->setItem(it.key(), col++, new QTableWidgetItem(*jt));

        row++;
    }

    fDish = d;
    m_ord = drv;
    ui->lbDishName->setText(d->f_dishName);
    ui->lbQty->setText(double_str(d->f_totalQty));
    ui->lbl1->setEnabled(true);

    if(fDish->f_printedQty > 0.01) {
        ui->lbl2->setEnabled(true);
        ui->lbl3->setEnabled(true);
    }

    ui->ptReason->setPlainText(fDish->f_removeReason);
    ui->lbAproved->setVisible(fDish->f_cancelrequest == 2);
}

DlgCorrection::~DlgCorrection()
{
    delete ui;
}

void DlgCorrection::printRemoved(double qty)
{
    QStringList prn;

    if(!fDish->f_print1.isEmpty()) {
        prn << fDish->f_print1;
    }

    if(!fDish->f_print2.isEmpty()) {
        prn << fDish->f_print2;
    }

    for(QStringList::const_iterator it = prn.constBegin(); it != prn.constEnd(); it++) {
        QFont font(qApp->font());
        font.setPointSize(20);
        C5Printing xp;
        QSize paperSize(1950, 2800);
        xp.setFont(font);
        xp.ctext(tr("Removed from order"));
        xp.br();
        xp.ltext(tr("Order number"), 0);
        xp.rtext(m_ord->m_header.f_id);
        xp.br();
        xp.lrtext(tr("Table"), m_ord->m_header.f_tableName);
        xp.br();
        xp.lrtext(tr("Staff"), m_ord->m_header.f_currStaffName);
        xp.br();
        xp.lrtext(tr("Date"), QDateTime::currentDateTime().toString(DATETIME_FORMAT));
        xp.br();
        xp.line();
        xp.br();
        xp.ltext(fDish->f_dishName, 1);
        xp.br();
        xp.rtext(double_str(qty));
        xp.br();
        xp.ltext(ui->ptReason->toPlainText(), 0);
        xp.br();
        xp.line(1);
        xp.br();
        xp.ltext(".", 1);
        xp.print(*it, QPageSize::Custom);
    }
}

void DlgCorrection::requestForCorrection(double qty)
{
    setEnabledWidget(false);
    m_ord->prepare("update o_dishes set f_removereason=:f_removereason, cancelrequest=:cancelrequest where id=:id");
    m_ord->bindValue(":id", fDish->f_id);
    m_ord->bindValue(":cancelrequest", 1);
    m_ord->bindValue(":f_removereason", ui->ptReason->toPlainText());
    m_ord->execSQL();
    m_ord->closeDB();
    msg(tr("Your request was accepted"));
    accept();
}

void DlgCorrection::on_btnReject_clicked()
{
    reject();
}

void DlgCorrection::on_btnOk_clicked()
{
    if(fDish->f_cancelrequest == 1) {
        return;
    }

    if(!ui->btnLossYes->isChecked() && !ui->btnLossNo->isChecked()) {
        DlgMessage::Msg(tr("No one of loss option was seLlected"));
        return;
    }

    if(ui->ptReason->toPlainText().trimmed().isEmpty()) {
        DlgMessage::Msg(tr("Reason cannot be empty"));
        return;
    }

    if(fDish->f_cancelrequest == 0) {
        requestForCorrection(fDish->f_totalQty);
        return;
    }

    fDish->f_stateId = DISH_STATE_REMOVED_PRINTED;
    LogThread::logOrderThread(m_ord->m_header.f_currStaffName, m_ord->m_header.f_id,
                              QString::number(fDish->f_id), tr("Removed with print "),
                              fDish->f_dishName + ": " + double_str(fDish->f_totalQty)
                              + ", " + ui->ptReason->toPlainText());
    printRemoved(fDish->f_totalQty);
    m_ord->prepare("update o_dishes set state_id=:state_id, qty=:qty, "
                   "f_removereason=:f_removereason, f_storestate=:f_storestate, "
                   "printed_qty=:printed_qty, emarks=null where id=:id");
    m_ord->bindValue(":state_id", fDish->f_stateId);
    m_ord->bindValue(":qty", fDish->f_totalQty);
    m_ord->bindValue(":printed_qty", fDish->f_printedQty);
    m_ord->bindValue(":f_storestate", (fDish->f_stateId != DISH_STATE_NORMAL && ui->btnLossYes->isChecked()) ? 1 : 0);
    m_ord->bindValue(":f_removereason", ui->ptReason->toPlainText());
    m_ord->bindValue(":id", fDish->f_id);
    m_ord->execSQL();
    accept();
}

void DlgCorrection::on_btnRemoveReason_clicked()
{
    if(!m_ord->openDB())
        return;

    m_ord->prepare("select id, name from o_remove_reason order by name");
    m_ord->execSQL();
    QMap<int, QString> rv;

    while(m_ord->next()) {
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
    if(!item) {
        return;
    }

    if(item->text() == "<") {
        ui->ptReason->setPlainText(ui->ptReason->toPlainText().remove(ui->ptReason->toPlainText().length() - 1, 1));
    } else {
        ui->ptReason->setPlainText(ui->ptReason->toPlainText() + item->text());
    }
}

void DlgCorrection::setWaitMode()
{
    ui->btnRemoveReason->setEnabled(false);
    ui->btnKeyboard->setEnabled(false);
    ui->btnClearWindow->setEnabled(false);
    ui->btnOk->setEnabled(false);
    ui->btnCheckResponse->setEnabled(true);
}

void DlgCorrection::on_btnCheckResponse_clicked()
{
    m_ord->prepare("select cancelrequest from o_dishes where id=:id");
    m_ord->bindValue(":id", fDish->f_id);
    m_ord->execSQL();

    if(m_ord->next()) {
        int cancelrequest = m_ord->v_int(0);

        if(cancelrequest != 2) {
            msg(tr("Your request wasnt approved"));
            return;
        }
    }

    ui->wButtons->setEnabled(true);
    ui->btnClearWindow->setEnabled(false);
    ui->btnCheckResponse->setEnabled(false);
    ui->btnOk->setEnabled(true);
    ui->btnReject->setEnabled(true);
    on_btnOk_clicked();
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
