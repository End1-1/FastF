#ifndef DLGCHANGEPASS_H
#define DLGCHANGEPASS_H

#include <QDialog>
#include "ff_user.h"

namespace Ui {
class DlgChangePass;
}

class DlgChangePass : public QDialog
{
    Q_OBJECT

public:
    explicit DlgChangePass(FF_User *user, QWidget *parent = 0);
    ~DlgChangePass();

private slots:
    void on_btnClose_clicked();
    void on_tblEmployes_clicked(const QModelIndex &index);

private:
    Ui::DlgChangePass *ui;
    FF_User *m_user;
    QSqlDrv *m_sqlDrv;
    void getEmployesList();
};

#endif // DLGCHANGEPASS_H
