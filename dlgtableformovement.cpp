#include "dlgtableformovement.h"
#include "ui_dlgtableformovement.h"
#include "dlgmessage.h"
#include "htable.h"
#include "dlglist.h"

DlgTableForMovement::DlgTableForMovement(QWidget *parent) :
    QDialog(parent, Qt::FramelessWindowHint),
    ui(new Ui::DlgTableForMovement)
{
    ui->setupUi(this);
    showFullScreen();
    ui->lbTable->clear();
    QFont f(qApp->font());
    f.setPointSize(f.pointSize() + 10);
    ui->lbTable->setFont(f);
    ui->lbOldTable->setFont(f);
    fNewTableId = -1;
    ui->tblHall->setButtons(ui->btnUp, ui->btnDown);
}

DlgTableForMovement::~DlgTableForMovement()
{
    delete ui;
}

int DlgTableForMovement::getTable(int& tableId, QWidget *parent, FF_HallDrv *hallDrv)
{
    DlgTableForMovement *d = new DlgTableForMovement(parent);
    d->m_currentTableId = tableId;
    d->m_hallDrv = hallDrv;
    d->m_hallDrv->configGrid(d->ui->tblHall, 0);
    d->ui->lbOldTable->setText(d->m_hallDrv->table(tableId)->name + " >> ");
    int result = d->exec();

    if(result == QDialog::Accepted) {
        tableId = d->fNewTableId;
    }

    delete d;
    return result;
}

void DlgTableForMovement::err(const QString &msg)
{
    sender()->deleteLater();
    ui->btnOK->setEnabled(false);
    ui->lbTable->clear();
    fNewTableId = -1;
    DlgMessage::Msg(msg);
}

void DlgTableForMovement::tableLocked(int tableId)
{
    ui->btnOK->setEnabled(true);
    ui->lbTable->setText(m_hallDrv->table(tableId)->name);
    fNewTableId = tableId;
}

void DlgTableForMovement::on_btnCancel_clicked()
{
    reject();
}

void DlgTableForMovement::on_tblHall_currentItemChanged(QTableWidgetItem *current, QTableWidgetItem *previous)
{
    Q_UNUSED(previous);
    fNewTableId = current->data(Qt::UserRole).toInt();

    if(!fNewTableId) {
        return;
    }

    HTable t;

    if(!t.tryLock(fNewTableId)) {
        err(tr("Table locked"));
        return;
    }

    tableLocked(fNewTableId);
}

void DlgTableForMovement::on_btnOK_clicked()
{
    accept();
}

void DlgTableForMovement::on_btnFilter_clicked()
{
    QVariant out;

    if(!DlgList::value(m_hallDrv->getHallMap(), out, this)) {
        return;
    }

    m_currentHallId = out.toInt();
    m_hallDrv->filter(m_currentHallId, false);
    m_hallDrv->configGrid(ui->tblHall, 0);
}
