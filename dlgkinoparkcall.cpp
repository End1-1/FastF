#include "dlgkinoparkcall.h"
#include "ui_dlgkinoparkcall.h"

dlgKinoParkCall::dlgKinoParkCall(bool newOrd, bool callStaff, const QString &tableName, QWidget *parent) :
    QDialog(parent, Qt::FramelessWindowHint),
    ui(new Ui::dlgKinoParkCall)
{
    ui->setupUi(this);
    ui->lbTable->setText(tableName);
    ui->lbNew->setVisible(newOrd);
    ui->lbCall->setVisible(callStaff);
    showFullScreen();
}

dlgKinoParkCall::~dlgKinoParkCall()
{
    delete ui;
}

void dlgKinoParkCall::on_btnClose_clicked()
{
    close();
}
