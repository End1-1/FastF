#ifndef DLGSELECTTAXREPORT_H
#define DLGSELECTTAXREPORT_H

#include <QDialog>

namespace Ui {
class DlgSelectTaxReport;
}

class DlgSelectTaxReport : public QDialog
{
    Q_OBJECT

public:
    explicit DlgSelectTaxReport(QWidget *parent = nullptr);
    ~DlgSelectTaxReport();
    static int getReportType();

private slots:
    void on_btnReportZ_clicked();

    void on_btnReportX_clicked();

    void on_btnCancel_clicked();

private:
    Ui::DlgSelectTaxReport *ui;
    int fResult;
};

#endif // DLGSELECTTAXREPORT_H
