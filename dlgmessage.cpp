#include "dlgmessage.h"
#include "ui_dlgmessage.h"

DlgMessage::DlgMessage(QWidget *parent) :
    QDialog(parent, Qt::FramelessWindowHint),
    ui(new Ui::DlgMessage)
{
    ui->setupUi(this);
    QFont f(qApp->font());
    f.setPointSize(f.pointSize() + 4);
    ui->lbText->setFont(f);
}

DlgMessage::~DlgMessage()
{
    delete ui;
}

int DlgMessage::Msg(const QString &message, bool merci)
{
    DlgMessage *d = new DlgMessage();
    d->ui->lbText->setText(message);
    if (merci) {
        d->ui->btnClose->setText(QObject::tr("Merci"));
        d->ui->btnCancel->setText(QObject::tr("Merci"));
    }
    int result = d->exec();
    delete d;
    return result;
}

void DlgMessage::on_btnClose_clicked()
{
    accept();
}

void DlgMessage::on_btnCancel_clicked()
{
    reject();
}
