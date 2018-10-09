#include "dlgremovereason.h"
#include "ui_dlgremovereason.h"
#include "dlgmessage.h"
#include <QScrollBar>

DlgRemoveReason::DlgRemoveReason(QMap<int, QString> values, QWidget *parent) :
    QDialog(parent, Qt::FramelessWindowHint),
    ui(new Ui::DlgRemoveReason)
{
    ui->setupUi(this);
    for (QMap<int, QString>::const_iterator it = values.begin(); it != values.end(); it++) {
        QListWidgetItem *item = new QListWidgetItem(ui->lstData);
        item->setText(it.value());
        item->setData(Qt::UserRole, it.key());
        item->setSizeHint(QSize(300, 50));
        ui->lstData->addItem(item);
        QFont f(qApp->font());
        f.setPixelSize(16);
        f.setBold(true);
        ui->lstData->setFont(f);
    }
}

DlgRemoveReason::~DlgRemoveReason()
{
    delete ui;
}

void DlgRemoveReason::on_btnOK_clicked()
{
    QList<QListWidgetItem*> l = ui->lstData->selectedItems();
    if (!l.count()) {
        DlgMessage::Msg(tr("Nothing is selected"));
        return;
    }
    m_reasonId = l.at(0)->data(Qt::UserRole).toInt();
    m_reasonName = l.at(0)->text();
    accept();
}

void DlgRemoveReason::on_btnUp_clicked()
{
    ui->lstData->verticalScrollBar()->setValue(ui->lstData->verticalScrollBar()->value() - 10);
}

void DlgRemoveReason::on_btnDown_clicked()
{
    ui->lstData->verticalScrollBar()->setValue(ui->lstData->verticalScrollBar()->value() + 10);
}
