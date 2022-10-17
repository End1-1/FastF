#include "dlgselecttaxreport.h"
#include "ui_dlgselecttaxreport.h"
#include "printtaxn.h"

DlgSelectTaxReport::DlgSelectTaxReport(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DlgSelectTaxReport)
{
    ui->setupUi(this);
    fResult = 0;
}

DlgSelectTaxReport::~DlgSelectTaxReport()
{
    delete ui;
}

int DlgSelectTaxReport::getReportType()
{
    DlgSelectTaxReport d;
    d.exec();
    return d.fResult;
}

void DlgSelectTaxReport::on_btnReportZ_clicked()
{
    fResult = report_z;
    accept();
}

void DlgSelectTaxReport::on_btnReportX_clicked()
{
    fResult = report_x;
    accept();
}

void DlgSelectTaxReport::on_btnCancel_clicked()
{
    reject();
}
