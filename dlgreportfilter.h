#ifndef DLGREPORTFILTER_H
#define DLGREPORTFILTER_H

#include <QDialog>

#ifndef DATE_FORMAT
#define DATE_FORMAT "dd.MM.yyyy"
#endif

namespace Ui {
class DlgReportFilter;
}

class DlgReportFilter : public QDialog
{
    Q_OBJECT

public:
    explicit DlgReportFilter(QMap<QString, QString> &filter, QWidget *parent = 0);
    ~DlgReportFilter();

private slots:
    void on_pushButton_2_clicked();

    void on_pushButton_clicked();

private:
    Ui::DlgReportFilter *ui;
    QMap<QString, QString> &m_filter;
};

#endif // DLGREPORTFILTER_H
