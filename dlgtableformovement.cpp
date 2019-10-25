#include "dlgtableformovement.h"
#include "ui_dlgtableformovement.h"
#include "dlgmessage.h"
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
    m_newTableId = -1;
    ui->tblHall->setButtons(ui->btnUp, ui->btnDown);
}

DlgTableForMovement::~DlgTableForMovement()
{
    delete ui;
}

int DlgTableForMovement::getTable(int &tableId, QWidget *parent, FF_HallDrv *hallDrv)
{
    DlgTableForMovement *d = new DlgTableForMovement(parent);
    d->m_currentTableId = tableId;
    d->m_hallDrv = hallDrv;
    d->m_hallDrv->configGrid(d->ui->tblHall, 0);
    d->ui->lbOldTable->setText(d->m_hallDrv->table(tableId)->name + " >> ");
    int result = d->exec();
    if (result == QDialog::Accepted)
        tableId = d->m_newTableId;
    delete d;
    return result;
}

void DlgTableForMovement::on_btnCancel_clicked()
{
    if (m_newTableId > -1)
        m_hallDrv->unlockTable(m_newTableId);
    reject();
}

void DlgTableForMovement::on_tblHall_currentItemChanged(QTableWidgetItem *current, QTableWidgetItem *previous)
{
    Q_UNUSED(previous)

    if (m_newTableId > -1)
        m_hallDrv->unlockTable(m_newTableId);

    ui->lbTable->clear();
    ui->btnOK->setEnabled(false);
    if (!current)
        return;

    m_newTableId = current->data(Qt::UserRole).toInt();
    if (!m_newTableId)
        return;

    m_newTableId = m_hallDrv->table(m_newTableId)->id;
    if (m_newTableId != m_currentTableId) {
        switch(m_hallDrv->lockTable(m_newTableId)) {
        case LOCK_SUCCESS:
            ui->btnOK->setEnabled(true);
            ui->lbTable->setText(m_hallDrv->table(m_newTableId)->name);
            break;
        case LOCK_LOCKED:
            m_newTableId = -1;
            DlgMessage::Msg(tr("Table is locked"));
            break;
        case LOCK_ERROR:
            m_newTableId = -1;
            DlgMessage::Msg(tr("SQL error"));
            break;
        }
    }

}

void DlgTableForMovement::on_btnOK_clicked()
{
    accept();
}

void DlgTableForMovement::on_btnFilter_clicked()
{
    QVariant out;
    if (!DlgList::value(m_hallDrv->getHallMap(), out, this)) {
        return;
    }
    m_currentHallId = out.toInt();
    m_hallDrv->filter(m_currentHallId, false);
    m_hallDrv->configGrid(ui->tblHall, 0);
}
