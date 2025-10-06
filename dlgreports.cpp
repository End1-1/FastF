#include "dlgreports.h"
#include "ui_dlgreports.h"
#include "ff_settingsdrv.h"
#include "dlgreportfilter.h"
#include "dlgmessage.h"
#include "dlgorder.h"
#include "c5printing.h"
#include "qsqldb.h"
#include "cnfapp.h"
#include "dlgselecttaxreport.h"
#include "mptcpsocket.h"
#include "cnfmaindb.h"
#include "utils.h"
#include "dlglist.h"
#include <QDir>
#include <QScrollBar>
#include <QLibrary>
#include <QTranslator>

#define report_page_width 60

typedef QString(*caption)();
typedef QString(*sql)();
typedef QStringList(*fields)();
typedef QStringList(*totalFields)();

dlgreports::dlgreports(FF_User *user, FF_HallDrv *hall, QWidget *parent) :
    FastfDialog(parent),
    ui(new Ui::dlgreports),
    m_user(user),
    m_hall(hall)
{
    ui->setupUi(this);
    m_sqlDrv = new QSqlDrv(m_user->fullName, "main");
    showFullScreen();
    getOrdersList();
    m_filter["date1"] = FF_SettingsDrv::cashDate().toString(DATE_FORMAT);
    m_filter["date2"] = FF_SettingsDrv::cashDate().toString(DATE_FORMAT);
    QDir d(QSystem::appPath() + "/reports/");
    QStringList files = d.entryList();

    for(QStringList::const_iterator i = files.begin(); i != files.end(); i++) {
        if((*i) == "." || (*i) == "..")
            continue;

        QLibrary l(QSystem::appPath() + "/reports/" + (*i));

        if(!l.load())
            continue;

        caption c = (caption) l.resolve("caption");
        sql s = (sql) l.resolve("sql");
        fields f = (fields) l.resolve("fields");
        totalFields tf = (totalFields) l.resolve("totalFields");

        if(!c || !s || !f || !tf) {
            l.unload();
            continue;
        }

        QString dllCaption = c();
        m_reports[dllCaption] = s();
        m_repFields[dllCaption] = f();
        m_totalFields[dllCaption] = tf();
        l.unload();
    }

    connect(ui->tblOrders->horizontalHeader(), SIGNAL(clicked(QModelIndex)), this, SLOT(ordersHeaderClicked(QModelIndex)));
    getOrdersList();
}

dlgreports::~dlgreports()
{
    delete ui;
}

void dlgreports::toError(const QString &msg)
{
    sender()->deleteLater();
    DlgMessage::Msg(msg);
}

void dlgreports::toTableLocked(const QString &orderId, int tableId)
{
    dlgorder *d = new dlgorder(this);
    d->setData(m_user, m_hall, tableId, orderId);
    d->setCashMode();
    d->showFullScreen();
    d->exec();
    delete d;
    sender()->deleteLater();
}

void dlgreports::ordersHeaderClicked(const QModelIndex &index)
{
    ui->tblOrders->sortByColumn(index.column(), Qt::AscendingOrder);
}

void dlgreports::on_btnClose_clicked()
{
    accept();
}

void dlgreports::getOrdersList()
{
    QList<int> colWidths;
    colWidths << 100 << 250 << 100 << 180 << 130;
    QStringList colCaptions;
    colCaptions << tr("ID") << tr("Staff") << tr("Table") << tr("Close date") << tr("Amount");
    m_sqlDrv->m_sql = "select oo.id, e.fname || ' ' || e.lname as staff_name, h.name as table_name, oo.date_close, oo.amount "
                      "from o_order oo, employes e, h_table h "
                      "where oo.table_id=h.id and oo.staff_id=e.id "
                      "and oo.date_cash between :date1 and :date2 and oo.state_id=:state_id "
                      "order by 2, 4";

    if(!m_sqlDrv->prepare())
        return;

    m_sqlDrv->m_query->bindValue(":date1", QDate::fromString(m_filter["date1"], DATE_FORMAT));
    m_sqlDrv->m_query->bindValue(":date2", QDate::fromString(m_filter["date2"], DATE_FORMAT));
    m_sqlDrv->m_query->bindValue(":state_id", ORDER_STATE_CLOSED);

    if(!m_sqlDrv->execSQL())
        return;

    m_sqlDrv->fillTableWidget(m_fields, ui->tblOrders);
    m_sqlDrv->close();
    ui->tblOrders->setHorizontalHeaderLabels(colCaptions);

    for(int i = 0; i < colWidths.count(); i++)
        ui->tblOrders->setColumnWidth(i, colWidths.at(i));

    ui->tblTotal->clear();
    ui->tblTotal->setColumnCount(ui->tblOrders->columnCount());
    ui->tblTotal->setRowCount(1);

    for(int i = 0; i < ui->tblTotal->columnCount(); i++)
        ui->tblTotal->setColumnWidth(i, ui->tblOrders->columnWidth(i));

    double amount = 0;

    for(int i = 0; i < ui->tblOrders->rowCount(); i++)
        amount += ui->tblOrders->item(i, 4)->data(Qt::DisplayRole).toDouble();

    ui->tblTotal->setItem(0, 4, new QTableWidgetItem(QString::number(amount, 'f', 0)));
    ui->tblTotal->setItem(0, 3, new QTableWidgetItem(QString::number(ui->tblOrders->rowCount())));
}

void dlgreports::on_btnOrderFilter_clicked()
{
    DlgReportFilter *d = new DlgReportFilter(m_filter, this);

    if(d->exec() == QDialog::Accepted) {
        getOrdersList();
    }

    delete d;
}

void dlgreports::on_pushButton_clicked()
{
    getOrdersList();
}

void dlgreports::on_btnUp_clicked()
{
    ui->tblOrders->verticalScrollBar()->setValue(ui->tblOrders->verticalScrollBar()->value() + 10);
}

void dlgreports::on_btnDown_clicked()
{
    ui->tblOrders->verticalScrollBar()->setValue(ui->tblOrders->verticalScrollBar()->value() - 10);
}

void dlgreports::on_btnPrint_clicked()
{
    QVariant rep;

    if(!DlgList::value(m_reports, rep, this))
        return;

    QString title = m_reports.key(rep);
    QString checkPrinterName = FF_SettingsDrv::value(SD_CHECK_PRINTER_NAME).toString();
    C5Printing pm;
    pm.setSceneParams(700, 3000, QPageLayout::Portrait);
    pm.setFont(qApp->font());
    pm.setFontSize(26);

    if(FF_SettingsDrv::value(SD_LOGO_FILENAME).toString().length())
        pm.image(FF_SettingsDrv::value(SD_LOGO_FILENAME).toString(), Qt::AlignCenter);

    pm.br();
    pm.ctext(FF_SettingsDrv::value(SD_OBJECT_NAME).toString());
    pm.br();
    pm.ctext(title);
    pm.br();
    pm.ltext(tr("Date range"), 1);
    pm.br();
    pm.ltext(m_filter["date1"] + " - " + m_filter["date2"], 1);
    pm.br();
    pm.line();
    pm.br();
    m_sqlDrv->prepare(rep.toString());
    m_sqlDrv->bind(":date1", QDate::fromString(m_filter["date1"], DATE_FORMAT));
    m_sqlDrv->bind(":date2", QDate::fromString(m_filter["date2"], DATE_FORMAT));
    m_sqlDrv->execSQL();
    QStringList &repFields = m_repFields[title];
    QStringList &repTotal = m_totalFields[title];
    QMap<QString, float> totals;

    for(QStringList::const_iterator i = repTotal.constBegin(); i != repTotal.constEnd(); i++)
        totals[*i] = 0;

    while(m_sqlDrv->next()) {
        for(QStringList::const_iterator i = repFields.constBegin(); i != repFields.constEnd(); i++) {
            QVariant v = m_sqlDrv->val();
            pm.ltext(*i, 1);

            if(v.type() == QVariant::Double)
                pm.rtext(QString::number(v.toDouble(), 'f', 2));
            else
                pm.rtext(v.toString());

            pm.br();

            if(totals.keys().contains(*i))
                totals[*i] += v.toFloat();
        }

        pm.line();
    }

    m_sqlDrv->close();
    pm.line(0);
    pm.setFontBold(true);
    pm.ctext(tr("Total"));
    pm.br();

    for(QMap<QString, float>::const_iterator i = totals.begin(); i != totals.end(); i++) {
        pm.ltext(i.key(), 1);
        pm.rtext(dts(i.value()));
        pm.br();
    }

    pm.line();
    pm.line();
    pm.setFontBold(false);
    pm.ltext(QString("%1: %2")
             .arg(tr("Printed"))
             .arg(QDateTime::currentDateTime().toString("dd.MM.yyyy HH:mm:ss")), 1);
    pm.br();
    pm.ltext(getHostName() + "/" + FF_SettingsDrv::value(SD_CHECK_PRINTER_NAME).toString(), 1);
    pm.br();
    pm.ltext(".", 1);
    pm.print(FF_SettingsDrv::value(SD_CHECK_PRINTER_NAME).toString(), QPageSize::Custom);
    QSqlLog::write(TABLE_HISTORY, tr("Print report"), "", m_user->fullName, 0);
}

void dlgreports::on_btnDailySale_clicked()
{
    QMap<QString, double> twoGo;
    QMap<QString, double> idram;
    QMap<QString, double> other;
    QMap<QString, double> twoGo_qty;
    QMap<QString, double> idram_qty;
    QMap<QString, double> other_qty;
    QMap<QString, double> cash;
    QMap<QString, double> cash_qty;
    QMap<QString, double> card;
    QMap<QString, double> card_qty;
    QMap<QString, double> complimentary;
    QMap<QString, double> complimentary_qty;
    QStringList staff;
    m_sqlDrv->prepare("select e.lname || ' ' || e.fname as ename, "
                      "sum(cast(right(f.data, char_length(f.data) - position(':', f.data)) as decimal(9,2))) as amount, "
                      " count(o.id) as qty "
                      "from o_order o, employes e , o_order_flags f "
                      "where o.staff_id=e.id and o.state_id=2 and o.id=f.order_id  "
                      "and o.date_cash between :date1 and :date2 "
                      "group by 1 "
                      "order by 1");
    m_sqlDrv->bind(":date1", QDate::fromString(m_filter["date1"], DATE_FORMAT));
    m_sqlDrv->bind(":date2", QDate::fromString(m_filter["date2"], DATE_FORMAT));
    m_sqlDrv->execSQL();

    while(m_sqlDrv->next()) {
        idram[m_sqlDrv->valStr("ename")] = m_sqlDrv->valFloat("amount");
        idram_qty[m_sqlDrv->valStr("ename")] = m_sqlDrv->valFloat("qty");
    }

    m_sqlDrv->prepare("select e.lname || ' ' || e.fname as ename, count(o.id) as qty, "
                      "sum(o.amount)+sum(COALESCE(oc.FAMOUNT, 0)) as amount, "
                      "sum(ot.fcash) as cash, "
                      "sum(ot.fcard) as card, "
                      "sum(ot.fidram) as idram, "
                      "sum(ot.fcomplimentary) as complimentary "
                      "from o_order o "
                      "left join o_tax ot on ot.fid=o.id "
                      "LEFT JOIN employes e on o.staff_id=e.id "
                      "LEFT JOIN O_CARD_TIPS oc ON o.id=oc.FORDER "
                      "where o.state_id=2 and o.date_cash between :date1 and :date2 "
                      "  "
                      "group by 1 ");
    m_sqlDrv->bind(":date1", QDate::fromString(m_filter["date1"], DATE_FORMAT));
    m_sqlDrv->bind(":date2", QDate::fromString(m_filter["date2"], DATE_FORMAT));
    m_sqlDrv->execSQL();

    while(m_sqlDrv->next()) {
        cash[m_sqlDrv->valStr("ename")] = m_sqlDrv->valFloat("cash");
        card[m_sqlDrv->valStr("ename")] = m_sqlDrv->valFloat("card");
        other[m_sqlDrv->valStr("ename")] = m_sqlDrv->valFloat("amount")
                                           - twoGo[m_sqlDrv->valStr("ename")]
                                           - idram[m_sqlDrv->valStr("ename")];
        other_qty[m_sqlDrv->valStr("ename")] = m_sqlDrv->valFloat("qty")
                                               - twoGo_qty[m_sqlDrv->valStr("ename")]
                                               - idram_qty[m_sqlDrv->valStr("ename")];
        idram[m_sqlDrv->valStr("ename")] = m_sqlDrv->valFloat("idram");
        complimentary[m_sqlDrv->valStr("ename")] = m_sqlDrv->valFloat("complimentary");
    }

    m_sqlDrv->close();

    for(QMap<QString, double>::const_iterator it = idram.begin(); it != idram.end(); it++)
        if(!staff.contains(it.key()))
            staff.append(it.key());

    for(QMap<QString, double>::const_iterator it = other.begin(); it != other.end(); it++)
        if(!staff.contains(it.key()))
            staff.append(it.key());

    std::sort(staff.begin(), staff.end());
    double gst = 0;
    double total2Go = 0;
    double totalIdram = 0;
    QString checkPrinterName = FF_SettingsDrv::value(SD_CHECK_PRINTER_NAME).toString();
    C5Printing pm;
    pm.setSceneParams(700, 3000, QPageLayout::Portrait);
    pm.setFont(qApp->font());
    pm.setFontSize(24);
    pm.image(__cnfapp.path() + "/logo_receipt.png", Qt::AlignHCenter);
    pm.br();
    pm.ctext(FF_SettingsDrv::value(SD_OBJECT_NAME).toString());
    pm.br();
    pm.setFontBold(true);
    pm.ctext(tr("Daily sale"));
    pm.setFontBold(false);
    pm.br();
    pm.ltext(tr("Date range"), 1);
    pm.br();
    pm.ltext(m_filter["date1"] + " - " + m_filter["date2"], 1);
    pm.br();
    pm.line(2);

    for(QStringList::const_iterator s = staff.constBegin(); s != staff.constEnd(); s++) {
        if(!twoGo.contains(*s))
            twoGo[*s] = 0;

        if(!twoGo_qty.contains(*s))
            twoGo_qty[*s]  = 0;

        if(!idram.contains(*s))
            idram[*s] = 0;

        if(!idram_qty.contains(*s))
            idram_qty[*s] = 0;

        if(!other.contains(*s))
            other[*s] = 0;

        if(!other_qty.contains(*s))
            other_qty[*s] = 0;

        if(!complimentary.contains(*s))
            complimentary[*s] = 0;

        double st = twoGo[*s] + idram[*s] + other[*s];
        double sq = twoGo_qty[*s] + idram_qty[*s] + other_qty[*s];
        gst += st;
        total2Go += twoGo[*s];
        totalIdram += idram[*s];
        pm.setFontBold(true);
        pm.ltext(*s, 2);
        pm.setFontBold(false);
        pm.br();
        pm.lrtext(tr("Total"), QString("%1 / %2")
                  .arg(QLocale().toString(st, 'f', 0))
                  .arg(QString::number(sq, 'f', 0)));
        pm.br();

        if(twoGo[*s] > 1) {
            pm.lrtext("2Go", QString("%1 / %2")
                      .arg(QString::number(twoGo[*s], 'f', 0))
                      .arg(QString::number(twoGo_qty[*s], 'f', 0)));
            pm.br();
        }

        if(cash[*s] > 1) {
            pm.lrtext(tr("Cash"),  QString("%1")
                      .arg(QLocale().toString(cash[*s], 'f', 0)));
            pm.br();
        }

        if(card[*s] > 1) {
            pm.lrtext(tr("Card"),  QString("%1")
                      .arg(QLocale().toString(card[*s], 'f', 0)));
            pm.br();
        }

        if(complimentary[*s] > 1) {
            pm.lrtext("Complimentary",  QString("%1")
                      .arg(QString::number(complimentary[*s], 'f', 0)));
            pm.br();
        }

        if(idram[*s] > 1) {
            pm.lrtext("IDram",  QString("%1")
                      .arg(QString::number(idram[*s], 'f', 0)));
            pm.br();
        }

        pm.line(2);
        pm.br();
    }

    pm.line(2);
    pm.setFontBold(true);
    pm.lrtext(tr("Total"), QString::number(gst, 'f', 0));
    pm.br();

    if(total2Go > 1 || totalIdram > 1) {
        pm.setFontBold(false);
        pm.ltext(tr("Included"), 2);

        if(total2Go > 1) {
            pm.br();
            pm.lrtext("2Go",  QString::number(total2Go, 'f', 0));
        }

        if(totalIdram > 1) {
            pm.br();
            pm.lrtext("IDram",  QString::number(totalIdram, 'f', 0));
        }

        pm.br();
    }

    pm.setFontBold(false);
    pm.line(2);
    pm.ltext("Coffee2Go", 2);
    pm.br();
    pm.br();
    pm.setFontBold(false);
    pm.setFontBold(false);
    pm.ltext(QString("%1: %2").arg(tr("Printed"), QDateTime::currentDateTime().toString(DATETIME_FORMAT)), 2);
    pm.br();
    pm.ltext(".", 1);
    pm.print(FF_SettingsDrv::value(SD_CHECK_PRINTER_NAME).toString(), QPageSize::Custom);
}

void dlgreports::on_btnPrintTaxReport_clicked()
{
    if(int t = DlgSelectTaxReport::getReportType()) {
        MPTcpSocket fTcpSocket;
        fTcpSocket.setServerIP(__cnfmaindb.fServerIP);
        fTcpSocket.setValue("session", SESSIONID);
        fTcpSocket.setValue("query", "taxreport");
        fTcpSocket.setValue("type", t);
        fTcpSocket.setValue("d1", m_filter["date1"] + " 00:00:00");
        fTcpSocket.setValue("d2", m_filter["date2"] + " 00:00:00");
        QJsonObject o = fTcpSocket.sendData();

        if(o["reply"].toString() == "ok") {
            msg(tr("Printed"));
        } else {
            msg(o["reply"].toString());
        }
    }
}

void dlgreports::on_btnTaxBack_clicked()
{
    QModelIndexList il = ui->tblOrders->selectionModel()->selectedRows();

    if(!il.count()) {
        DlgMessage::Msg(tr("No order is selected"));
        return;
    }

    QString ord = ui->tblOrders->item(il.at(0).row(), 0)->text();

    if(DlgMessage::Msg(QString("%1 %2").arg(tr("Confirm to cancel fiscal"), ord)) != QDialog::Accepted) {
        return;
    }

    MPTcpSocket fTcpSocket;
    fTcpSocket.setServerIP(__cnfmaindb.fServerIP);
    fTcpSocket.setValue("session", SESSIONID);
    fTcpSocket.setValue("query", "taxcancel");
    fTcpSocket.setValue("order", ord);
    QJsonObject o = fTcpSocket.sendData();

    if(o["reply"].toString() == "ok") {
        msg(tr("Fiscal canceled"));
    } else {
        msg(o["reply"].toString());
    }
}

void dlgreports::on_btnOrderDetails_clicked()
{
    QModelIndexList il = ui->tblOrders->selectionModel()->selectedRows();

    if(!il.count()) {
        DlgMessage::Msg(tr("No order is selected"));
        return;
    }

    dlgorder d(this);
    d.setData(m_user, m_hall, 0, ui->tblOrders->item(il.at(0).row(), 0)->text());
    d.setCashMode();
    d.showFullScreen();
    d.exec();
}
