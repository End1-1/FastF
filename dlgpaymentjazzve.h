#ifndef DLGPAYMENTJAZZVE_H
#define DLGPAYMENTJAZZVE_H

#include <QDialog>

namespace Ui {
class DlgPaymentJazzve;
}

class DlgPaymentJazzve : public QDialog
{
    Q_OBJECT

public:
    explicit DlgPaymentJazzve(QWidget *parent = 0);
    ~DlgPaymentJazzve();

private:
    Ui::DlgPaymentJazzve *ui;
};

#endif // DLGPAYMENTJAZZVE_H
