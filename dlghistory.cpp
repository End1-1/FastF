#include "dlghistory.h"
#include "ui_dlghistory.h"
#include "qsqldrv.h"

DlgHistory::DlgHistory(QString orderId, const QString &username, QWidget *parent) :
    QDialog(parent, Qt::FramelessWindowHint),
    ui(new Ui::DlgHistory)
{
    ui->setupUi(this);

    QStringList captions;
    captions << tr("Date") << tr("Host") << tr("Staff") << tr("Action")
             << tr("Additional");
    QList<int> widths;
    widths << 100 << 150 << 250 << 200 << 500;
    ui->tblHistory->setColumnCount(widths.count());
    ui->tblHistory->setHorizontalHeaderLabels(captions);
    for (int i = 0; i < widths.count(); i++)
        ui->tblHistory->setColumnWidth(i, widths.at(i));

    QSqlDrv d(username, "thread");
    d.prepare("select log_date, host_name, user_name, query, data "
              "from sql_log where table_name='SALE_HISTORY' and note=:sid");
    d.bind(":sid", orderId);
    d.execSQL();
    int colCount = d.m_query->record().count();
    while (d.m_query->next()) {
        int row = ui->tblHistory->rowCount();
        ui->tblHistory->setRowCount(row + 1);
        for (int i = 0; i < colCount; i++)
            ui->tblHistory->setItem(row, i, new QTableWidgetItem(d.m_query->value(i).toString()));
    }
    d.close();

    showFullScreen();
}

DlgHistory::~DlgHistory()
{
    delete ui;
}

void DlgHistory::on_btnOK_clicked()
{
    accept();
}
