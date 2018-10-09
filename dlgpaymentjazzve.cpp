#include "dlgpaymentjazzve.h"
#include "ui_dlgpaymentjazzve.h"

DlgPaymentJazzve::DlgPaymentJazzve(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DlgPaymentJazzve)
{
    ui->setupUi(this);
}

DlgPaymentJazzve::~DlgPaymentJazzve()
{
    delete ui;
}
