#include "dlgapprovecorrectionmessage.h"
#include "ui_dlgapprovecorrectionmessage.h"
#include <QJsonArray>
#include <QJsonObject>

DlgApproveCorrectionMessage::DlgApproveCorrectionMessage(const QJsonArray &ar, bool approve, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DlgApproveCorrectionMessage)
{
    ui->setupUi(this);
    ui->tbl->setColumnWidth(0, 200);
    ui->tbl->setColumnWidth(1, 300);
    ui->tbl->setColumnWidth(2, 30);
    if (approve) {
        ui->lbApprove->setText(tr("Approved"));
    } else {
        ui->lbApprove->setText(tr("Not approved"));
    }
    ui->tbl->setRowCount(ar.count());
    for (int i = 0; i < ar.count(); i++) {
        QJsonObject jo = ar.at(i).toObject();
        ui->tbl->setItem(i, 0, new QTableWidgetItem(jo["user"].toString()));
        ui->tbl->setItem(i, 1, new QTableWidgetItem(jo["msg"].toString()));
        QTableWidgetItem *item = new QTableWidgetItem();
        item->setIcon(QIcon(jo["ok"].toInt() == 0 ? ":/res/cancel.png" : ":/res/ok.png"));
        ui->tbl->setItem(i, 2, item);
    }
}

DlgApproveCorrectionMessage::~DlgApproveCorrectionMessage()
{
    delete ui;
}

void DlgApproveCorrectionMessage::on_btnClose_clicked()
{
    accept();
}
