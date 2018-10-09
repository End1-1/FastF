#include "dlgreportfilter.h"
#include "ui_dlgreportfilter.h"

DlgReportFilter::DlgReportFilter(QMap<QString, QString> &filter, QWidget *parent) :
    QDialog(parent, Qt::FramelessWindowHint),
    ui(new Ui::DlgReportFilter),
    m_filter(filter)
{
    ui->setupUi(this);
    if (!m_filter.contains("date1")) {
        m_filter["date1"] = QDate::currentDate().toString(DATE_FORMAT);
        m_filter["date2"] = m_filter["date1"];
    }
    ui->date1->setDate(QDate::fromString(m_filter["date1"], DATE_FORMAT));
    ui->date2->setDate(QDate::fromString(m_filter["date2"], DATE_FORMAT));
}

DlgReportFilter::~DlgReportFilter()
{
    delete ui;
}

void DlgReportFilter::on_pushButton_2_clicked()
{
    reject();
}

void DlgReportFilter::on_pushButton_clicked()
{
    m_filter["date1"] = ui->date1->date().toString(DATE_FORMAT);
    m_filter["date2"] = ui->date2->date().toString(DATE_FORMAT);
    accept();
}
