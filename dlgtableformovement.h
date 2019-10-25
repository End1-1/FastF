#ifndef DLGTABLEFORMOVEMENT_H
#define DLGTABLEFORMOVEMENT_H

#include <QDialog>
#include "ff_halldrv.h"

namespace Ui {
class DlgTableForMovement;
}

class DlgTableForMovement : public QDialog
{
    Q_OBJECT

public:
    explicit DlgTableForMovement(QWidget *parent = 0);
    ~DlgTableForMovement();
    static int getTable(int &tableId, QWidget *parent, FF_HallDrv *hallDrv);

private slots:
    void on_btnCancel_clicked();
    void on_tblHall_currentItemChanged(QTableWidgetItem *current, QTableWidgetItem *previous);
    void on_btnOK_clicked();
    void on_btnFilter_clicked();

private:
    Ui::DlgTableForMovement *ui;
    FF_HallDrv *m_hallDrv;
    int m_currentTableId;
    int m_newTableId;
    int m_currentHallId;
};

#endif // DLGTABLEFORMOVEMENT_H
