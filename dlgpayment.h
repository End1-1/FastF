#ifndef DLGPAYMENT_H
#define DLGPAYMENT_H

#include <QDialog>
#include "orderdrv/od_drv.h"
#include "mptcpsocket.h"

namespace Ui {
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
    void on_btnJazzveGift_clicked();
    void on_btnCancel_clicked();
    void on_btnPrepaymentCash_clicked();
    void on_btnPrepaymentNoCash_clicked();
    void on_leCash_textChanged(const QString &arg1);
    void on_leCard_textChanged(const QString &arg1);
    void on_btnClearCash_clicked();
    void on_btnClearCard_clicked();
    void on_btnSetCash_clicked();
    void on_btnCard_clicked();
    void on_btnComplimentary_clicked();

    void on_btnPrepaymentCash_2_clicked();

    void on_btnChange_clicked();

private:
    Ui::DlgPayment *ui;
    OD_Drv *fDrv;
    MPTcpSocket fTcpSocket;
    double fGift;
    void payment(quint8 mode);
    void receipt(int mode);
};

#endif // DLGPAYMENT_H
