#ifndef DLGPAYMENT_H
#define DLGPAYMENT_H

#include <QDialog>
#include "orderdrv/od_drv.h"

namespace Ui
{
class DlgPayment;
}

class FF_User;

class DlgPayment : public QDialog
{
    Q_OBJECT

public:
    explicit DlgPayment(FF_User *u, OD_Drv *drv, QWidget *parent = nullptr);
    ~DlgPayment();
    static bool payment(FF_User *u, OD_Drv *drv, QWidget *parent);

private slots:
    void on_btnPrepayment_clicked();
    void on_btnCash_clicked();
    void on_btnARCA_clicked();
    void on_btnMASTER_clicked();
    void on_btnVISA_clicked();
    void on_btnMAESTRO_clicked();
    void on_btnIDRAM_clicked();
    void on_btnCancel_clicked();
    void on_btnPrepaymentCash_clicked();
    void on_btnPrepaymentNoCash_clicked();
    void on_btnCard_clicked();
    void on_btnComplimentary_clicked();
    void on_btnChange_clicked();
    void on_btnFiscalCancel_clicked();

private:
    Ui::DlgPayment* ui;
    OD_Drv* fDrv;
    FF_User* fUser;
    void payment(quint8 mode);
    void receipt(int mode);
    void printPrecheck(const QString &qr);
};

#endif // DLGPAYMENT_H
