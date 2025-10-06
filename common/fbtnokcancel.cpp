#include "fbtnokcancel.h"
#include "ui_fbtnokcancel.h"
#include "qdlgbase.h"

FBtnOkCancel::FBtnOkCancel(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FBtnOkCancel)
{
    ui->setupUi(this);
}

FBtnOkCancel::~FBtnOkCancel()
{
    delete ui;
}

void FBtnOkCancel::on_btnCancel_clicked()
{
    (qobject_cast<QDlgBase*>(parent()))->reject();
}

void FBtnOkCancel::on_btnOK_clicked()
{
    static_cast<QDlgBase*>(parent())->btnOK();
}
