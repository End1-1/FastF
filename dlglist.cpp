#include "dlglist.h"
#include "ui_dlglist.h"
#include "dlgmessage.h"

DlgList::DlgList(QWidget *parent) :
    QDialog(parent, Qt::FramelessWindowHint),
    ui(new Ui::DlgList)
{
    ui->setupUi(this);
    QFont font(qApp->font());
    font.setPointSize(18);
    font.setBold(true);
    ui->lstData->setFont(font);
    showFullScreen();
}

DlgList::~DlgList()
{
    delete ui;
}

bool DlgList::value(const QMap<QString, QVariant> data, QVariant &out, QWidget *parent)
{
    DlgList *d = new DlgList(parent);
    for (QMap<QString, QVariant>::const_iterator it = data.begin(); it != data.end(); it++) {
        QListWidgetItem *item = new QListWidgetItem(d->ui->lstData);
        item->setText(it.key());
        item->setData(Qt::UserRole, it.value());
        item->setSizeHint(QSize(200, 50));
        item->setTextAlignment(Qt::AlignCenter);
        d->ui->lstData->addItem(item);
    }
    if (d->exec() == QDialog::Accepted) {
        out = d->m_result;
        delete d;
        return true;
    }
    delete d;
    return false;
}

void DlgList::on_btnCancel_clicked()
{
    reject();
}

void DlgList::on_btnOK_clicked()
{
    QList<QListWidgetItem*> l = ui->lstData->selectedItems();
    if (!l.count()) {
        DlgMessage::Msg(tr("Nothing is selected"));
        return;
    }
    m_result = l.at(0)->data(Qt::UserRole);
    accept();
}
