#include "dlgreports.h"
#include "ui_dlgreports.h"
#include "ff_settingsdrv.h"
#include "ff_orderdrv.h"
#include "dlgreportfilter.h"
#include "dlgmessage.h"
#include "dlgorder.h"
#include "printing.h"
#include "qsqldb.h"
#include "dlglist.h"
#include <QDir>
#include <QScrollBar>
#include <QLibrary>
#include <QTranslator>

#define report_page_width 60

typedef QString (*caption)();
typedef QString (*sql)();
typedef QStringList (*fields)();
typedef QStringList (*totalFields)();

dlgreports::dlgreports(FF_User *user, FF_HallDrv *hall, QWidget *parent) :
    QDialog(parent, Qt::FramelessWindowHint),
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
    for (QStringList::const_iterator i = files.begin(); i != files.end(); i++) {
        if ((*i) == "." || (*i) == ".." )
            continue;

        QLibrary l(QSystem::appPath() + "/reports/" + (*i));
        if (!l.load())
            continue;

        caption c = (caption) l.resolve("caption");
        sql s = (sql) l.resolve("sql");
        fields f = (fields) l.resolve("fields");
        totalFields tf = (totalFields) l.resolve("totalFields");
        if (!c || !s || !f || !tf) {
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

void dlgreports::ordersHeaderClicked(const QModelIndex &index)
{
    ui->tblOrders->sortByColumn(index.column());
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
    if (!m_sqlDrv->prepare())
        return;
    m_sqlDrv->m_query->bindValue(":date1", QDate::fromString(m_filter["date1"], DATE_FORMAT));
    m_sqlDrv->m_query->bindValue(":date2", QDate::fromString(m_filter["date2"], DATE_FORMAT));
    m_sqlDrv->m_query->bindValue(":state_id", ORDER_STATE_CLOSED);
    if (!m_sqlDrv->execSQL())
        return;
    m_sqlDrv->fillTableWidget(m_fields, ui->tblOrders);
    m_sqlDrv->close();

    ui->tblOrders->setHorizontalHeaderLabels(colCaptions);
    for (int i = 0; i < colWidths.count(); i++)
        ui->tblOrders->setColumnWidth(i, colWidths.at(i));

    ui->tblTotal->clear();
    ui->tblTotal->setColumnCount(ui->tblOrders->columnCount());
    ui->tblTotal->setRowCount(1);
    for (int i = 0; i < ui->tblTotal->columnCount(); i++)
        ui->tblTotal->setColumnWidth(i, ui->tblOrders->columnWidth(i));

    double amount = 0;
    for (int i = 0; i < ui->tblOrders->rowCount(); i++)
        amount += ui->tblOrders->item(i, 4)->data(Qt::DisplayRole).toDouble();
    ui->tblTotal->setItem(0, 4, new QTableWidgetItem(QString::number(amount, 'f', 0)));
    ui->tblTotal->setItem(0, 3, new QTableWidgetItem(QString::number(ui->tblOrders->rowCount())));
}

void dlgreports::on_btnOrderFilter_clicked()
{
    DlgReportFilter *d = new DlgReportFilter(m_filter, this);
    if (d->exec() == QDialog::Accepted) {
        getOrdersList();
    }
    delete d;
}

void dlgreports::on_pushButton_clicked()
{
    getOrdersList();
}

void dlgreports::on_pushButton_2_clicked()
{
    QModelIndexList il = ui->tblOrders->selectionModel()->selectedRows();
    if (!il.count()) {
        DlgMessage::Msg(tr("No order is selected"));
        return;
    }

    QSqlLog::write(TABLE_HISTORY, tr("Open order from cash"), "", m_user->fullName, ui->tblOrders->item(il.at(0).row(), 0)->text().toInt());

    dlgorder *d = new dlgorder(this);
    d->setData(m_user, m_hall, 0, ui->tblOrders->item(il.at(0).row(), 0)->text());
    d->setCashMode();
    d->showFullScreen();
    d->exec();
    delete d;
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
    if (!DlgList::value(m_reports, rep, this))
        return;

    QString title = m_reports.key(rep);

    QString checkPrinterName = FF_SettingsDrv::value(SD_CHECK_PRINTER_NAME).toString();
    if(!___printerInfo->printerExists(checkPrinterName)) {
        QString prns;
        for (QList<QPrinterInfo>::const_iterator it = ___printerInfo->m_printers.begin(); it != ___printerInfo->m_printers.end(); it++)
            prns += it->printerName() + "\r\n";
        DlgMessage::Msg(tr("Printer not exists") + " :" + checkPrinterName + "\r\n"
                        + tr("Available printers are: ") + "\r\n"
                        + prns);
        return;
    }

    SizeMetrics sm(___printerInfo->resolution(checkPrinterName));
    XmlPrintMaker pm(&sm);

    pm.setFontName(qApp->font().family());
    pm.setFontSize(10);
    int top = 5;

    if (FF_SettingsDrv::value(SD_LOGO_FILENAME).toString().length())
        top += pm.imageCenter(FF_SettingsDrv::value(SD_LOGO_FILENAME).toString(), top, report_page_width) + 1;
    pm.textCenterAlign(FF_SettingsDrv::value(SD_OBJECT_NAME).toString(), report_page_width, top);
    top += pm.lastTextHeight() + 2;
    pm.textCenterAlign(title, report_page_width, top);
    top += pm.lastTextHeight() + 2;
    pm.text(tr("Date range"), 1, top);
    top += pm.lastTextHeight() + 2;
    pm.text(m_filter["date1"] + " - " + m_filter["date2"], 1, top);
    top += pm.lastTextHeight() + 2;
    pm.line(1, top, report_page_width, top);
    top++;

    m_sqlDrv->prepare(rep.toString());
    m_sqlDrv->bind(":date1", QDate::fromString(m_filter["date1"], DATE_FORMAT));
    m_sqlDrv->bind(":date2", QDate::fromString(m_filter["date2"], DATE_FORMAT));
    m_sqlDrv->execSQL();

    QStringList &repFields = m_repFields[title];
    QStringList &repTotal = m_totalFields[title];
    QMap<QString, float> totals;
    for (QStringList::const_iterator i = repTotal.begin(); i != repTotal.end(); i++)
        totals[*i] = 0;

    while (m_sqlDrv->next()) {
        for (QStringList::const_iterator i = repFields.begin(); i != repFields.end(); i++) {
            QVariant v = m_sqlDrv->val();
            pm.text(*i, 1, top);
            if (v.type() == QVariant::Double)
                pm.textRightAlign(QString::number(v.toDouble(), 'f', 2), report_page_width, top);
            else
                pm.textRightAlign(v.toString(), report_page_width, top);
            top += pm.lastTextHeight() + 2;
            if (totals.keys().contains(*i))
                totals[*i] += v.toFloat();
        }
        top += 3;
        pm.line(0, top, report_page_width, top);
        top++;
        pm.checkForNewPage(top);
    }

    m_sqlDrv->close();

    pm.checkForNewPage(top);
    pm.line(0, top, report_page_width, top);
    top++;
    pm.setFontSize(12);
    pm.setFontBold(true);
    pm.textCenterAlign(tr("Total"), report_page_width, top);
    top += pm.lastTextHeight() + 2;
    pm.checkForNewPage(top);
    for (QMap<QString, float>::const_iterator i = totals.begin(); i != totals.end(); i++) {
        pm.text(i.key(), 1, top);
        pm.textRightAlign(dts(i.value()), report_page_width, top);
        top += pm.lastTextHeight() + 2;
        pm.checkForNewPage(top);
    }

    pm.line(1, top, report_page_width, top);
    top++;
    pm.line(1, top, report_page_width, top);
    top += 5;

    pm.checkForNewPage(top);
    pm.setFontSize(8);
    pm.setFontBold(false);
    pm.text(QString("%1: %2")
            .arg(tr("Printed"))
            .arg(QDateTime::currentDateTime().toString("dd.MM.yyyy HH:mm:ss")), 1, top);
    top += pm.lastTextHeight() + 1;
    pm.checkForNewPage(top);
    pm.text(getHostName() + "/" + FF_SettingsDrv::value(SD_CHECK_PRINTER_NAME).toString(), 1, top);

    top += 5;
    pm.text(".", 1, top);
    pm.finishPage();

    ThreadPrinter *tp = new ThreadPrinter(FF_SettingsDrv::value(SD_CHECK_PRINTER_NAME).toString(), sm, pm);
    tp->start();

    QSqlLog::write(TABLE_HISTORY, tr("Print report"), "", m_user->fullName, 0);
}

void dlgreports::on_btnDailySale_clicked()
{
    QMap<QString, double> twoGo;
    QMap<QString, double> idram;
    QMap<QString, double> other;
    QMap<QString, double> icev;
    QMap<QString, double> twoGo_qty;
    QMap<QString, double> idram_qty;
    QMap<QString, double> other_qty;
    QMap<QString, double> icev_qty;
    QMap<QString, double> icev_qtyDish;
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
    while (m_sqlDrv->next()) {
        idram[m_sqlDrv->valStr("ename")] = m_sqlDrv->valFloat("amount");
        idram_qty[m_sqlDrv->valStr("ename")] = m_sqlDrv->valFloat("qty");
    }

    m_sqlDrv->prepare("select e.lname || ' ' || e.fname as ename, count(o.id) as qty, sum(o.amount) as amount "
                      "from o_order o, employes e "
                      "where o.staff_id=e.id and o.state_id=2 and o.date_cash between :date1 and :date2 "
                      "  "
                      "group by 1 ");
    m_sqlDrv->bind(":date1", QDate::fromString(m_filter["date1"], DATE_FORMAT));
    m_sqlDrv->bind(":date2", QDate::fromString(m_filter["date2"], DATE_FORMAT));
    m_sqlDrv->execSQL();
    while (m_sqlDrv->next()) {
        other[m_sqlDrv->valStr("ename")] = m_sqlDrv->valFloat("amount")
                - twoGo[m_sqlDrv->valStr("ename")]
                - idram[m_sqlDrv->valStr("ename")]
                - icev[m_sqlDrv->valStr("ename")];
        other_qty[m_sqlDrv->valStr("ename")] = m_sqlDrv->valFloat("qty")
                - twoGo_qty[m_sqlDrv->valStr("ename")]
                - idram_qty[m_sqlDrv->valStr("ename")]
                - icev_qty[m_sqlDrv->valStr("ename")];
    }
    m_sqlDrv->close();

    for (QMap<QString, double>::const_iterator it = idram.begin(); it != idram.end(); it++)
        if (!staff.contains(it.key()))
                staff.append(it.key());
    for (QMap<QString, double>::const_iterator it = other.begin(); it != other.end(); it++)
        if (!staff.contains(it.key()))
                staff.append(it.key());
    qSort(staff);

    double gst = 0;
    double gsq = 0;
    double total2Go = 0;
    double totalIdram = 0;
    double totalIcev = 0;

    QString checkPrinterName = FF_SettingsDrv::value(SD_CHECK_PRINTER_NAME).toString();
    SizeMetrics sm(___printerInfo->resolution(checkPrinterName));
    XmlPrintMaker pm(&sm);

    pm.setFontName(qApp->font().family());
    pm.setFontSize(10);
    int top = 5;

    if (FF_SettingsDrv::value(SD_LOGO_FILENAME).toString().length())
        top += pm.imageCenter(FF_SettingsDrv::value(SD_LOGO_FILENAME).toString(), top, report_page_width) + 1;
    pm.textCenterAlign(FF_SettingsDrv::value(SD_OBJECT_NAME).toString(), report_page_width, top);
    top += pm.lastTextHeight() + 2;
    pm.setFontBold(true);
    pm.textCenterAlign(tr("Daily sale"), report_page_width, top);
    pm.setFontBold(false);
    top += pm.lastTextHeight() + 2;
    pm.text(tr("Date range"), 1, top);
    top += pm.lastTextHeight() + 2;
    pm.text(m_filter["date1"] + " - " + m_filter["date2"], 1, top);
    top += pm.lastTextHeight() + 2;
    pm.line(1, top, report_page_width, top);
    top++;

    for (QStringList::const_iterator s = staff.begin(); s != staff.end(); s++) {
        if (!twoGo.contains(*s))
            twoGo[*s] = 0;
        if (!twoGo_qty.contains(*s))
            twoGo_qty[*s]  = 0;
        if (!idram.contains(*s))
            idram[*s] = 0;
        if (!idram_qty.contains(*s))
            idram_qty[*s] = 0;
        if (!icev.contains(*s))
            icev[*s] = 0;
        if (!icev_qty[*s])
            icev_qty[*s] = 0;
        if (!icev_qtyDish.contains(*s))
            icev_qtyDish[*s] = 0;
        if (!other.contains(*s))
            other[*s] = 0;
        if (!other_qty.contains(*s))
            other_qty[*s] = 0;
        double st = twoGo[*s] + idram[*s] + icev[*s] + other[*s];
        double sq = twoGo_qty[*s] + idram_qty[*s] + icev_qty[*s] + other_qty[*s];
        gst += st;
        gsq += sq;
        total2Go += twoGo[*s];
        totalIdram += idram[*s];
        totalIcev += icev[*s];
        pm.setFontBold(true);
        pm.text(*s, 2, top);
        pm.setFontBold(false);
        top += pm.lastTextHeight();
        pm.text(tr("Total"), 2, top);
        pm.textRightAlign(QString("%1 / %2")
                          .arg(QString::number(st, 'f', 0))
                          .arg(QString::number(sq, 'f', 0)),
                           report_page_width, top);
        top += pm.lastTextHeight();
        pm.checkForNewPage(top);
        if (twoGo[*s] > 1) {
            pm.text("2Go", 2, top);
            pm.textRightAlign(QString("%1 / %2")
                              .arg(QString::number(twoGo[*s], 'f', 0))
                              .arg(QString::number(twoGo_qty[*s], 'f', 0)),
                              report_page_width, top);
            top += pm.lastTextHeight();
            pm.checkForNewPage(top);
        }
        if (idram[*s] > 1) {
            pm.text("IDram", 2, top);
            pm.textRightAlign(QString("%1 / %2")
                              .arg(QString::number(idram[*s], 'f', 0))
                              .arg(QString::number(idram_qty[*s], 'f', 0)),
                              report_page_width, top);
            top += pm.lastTextHeight();
            pm.checkForNewPage(top);
        }
        if (icev[*s] > 1) {
            pm.text("ICE-V", 2, top);
            pm.textRightAlign(QString("%1 / %2 (%3)")
                              .arg(QString::number(icev[*s], 'f', 0))
                              .arg(QString::number(icev_qty[*s], 'f', 0))
                                .arg(QString::number(icev_qtyDish[*s], 'f', 0)),
                              report_page_width, top);
            top += pm.lastTextHeight();
            pm.checkForNewPage(top);
        }
        top ++;
        pm.line(0, top, report_page_width, top);
        top ++;
    }



//    /* Total sales */
//    m_sqlDrv->prepare("select e.lname || ' ' || e.fname as fname, count(o.id) as qnt, sum(o.amount) as amount "
//                      "from o_order o, employes e "
//                      "where o.staff_id=e.id and o.state_id=2 and o.date_cash between :date1 and :date2 "
//                      "group by 1 ");
//    m_sqlDrv->bind(":date1", QDate::fromString(m_filter["date1"], DATE_FORMAT));
//    m_sqlDrv->bind(":date2", QDate::fromString(m_filter["date2"], DATE_FORMAT));
//    m_sqlDrv->execSQL();
//    double total = 0;
//    while (m_sqlDrv->next()) {
//        pm.text(m_sqlDrv->val().toString(), 2, top);
//        top += pm.lastTextHeight() + 2;
//        pm.text(QString::number(m_sqlDrv->valInt("QNT")), 2, top);
//        pm.textRightAlign(QString::number(m_sqlDrv->valFloat("amount"), 'f', 2), report_page_width, top);
//        top += pm.lastTextHeight() + 2;
//        pm.line(2, top, report_page_width, top);
//        top ++;
//        total += m_sqlDrv->valFloat("amount");
//        pm.checkForNewPage(top);
//    }
    top ++;
    pm.checkForNewPage(top);
    pm.line(0, top, report_page_width, top);
    top ++;
    pm.line(0, top, report_page_width, top);
    top += 2;
    pm.setFontBold(true);
    pm.setFontSize(12);
    pm.text(tr("Total"), 2, top);
    pm.textRightAlign(QString::number(gst, 'f', 0), report_page_width, top);
    top += pm.lastTextHeight() + 2;

    if (total2Go > 1 || totalIdram > 1 || totalIcev > 1) {
        pm.setFontBold(false);
        pm.setFontSize(10);
        pm.text(tr("Included"), 2, top);
        if (total2Go > 1) {
            top += pm.lastTextHeight();
            pm.text("2Go", 2, top);
            pm.textRightAlign(QString::number(total2Go, 'f', 0), report_page_width, top);
        }
        if (totalIdram > 1) {
            top += pm.lastTextHeight();
            pm.text("IDram", 2, top);
            pm.textRightAlign(QString::number(totalIdram, 'f', 0), report_page_width, top);
        }
        if (totalIcev > 1) {
            top += pm.lastTextHeight();
            pm.text("ICE-V", 2, top);
            pm.textRightAlign(QString::number(totalIcev, 'f', 0), report_page_width, top);
        }
        top += pm.lastTextHeight() + 2;
    }

    pm.setFontBold(false);
    pm.setFontSize(10);
    top += 40;
    pm.checkForNewPage(top);
    pm.line(2, top, report_page_width, top);
    top++;
    pm.setFontSize(20);
    pm.text("Coffee2Go", 2, top);
    top += pm.lastTextHeight();
    top += pm.lastTextHeight() + 2;
    pm.setFontBold(false);
    pm.setFontSize(10);
    pm.text("ICE-V", 2, top);
    top += pm.lastTextHeight();
    top += pm.lastTextHeight() + 2;
    pm.setFontBold(false);
    pm.setFontSize(10);
    /*
    m_sqlDrv->prepare("select * from IDRAM (:date1, :date2)");
    m_sqlDrv->bind(":date1", QDate::fromString(m_filter["date1"], DATE_FORMAT));
    m_sqlDrv->bind(":date2", QDate::fromString(m_filter["date2"], DATE_FORMAT));
    m_sqlDrv->execSQL();
    bool another = false;
    bool firstIdram = true;
    float totalIdram = 0.0;
    while (m_sqlDrv->next()) {
        if (!another) {
            pm.checkForNewPage(top);
            top += 20;
            pm.setFontBold(true);
            pm.text(tr("Other information"), 2, top);
            pm.setFontBold(false);
            top += pm.lastTextHeight() + 1;
            pm.line(2, top, report_page_width, top);
            top ++;
            pm.line(2, top, report_page_width, top);
            top ++;
            another = true;
        }
        if (firstIdram) {
            pm.checkForNewPage(top);
            pm.text(tr("IDram"), 2, top);
            top += pm.lastTextHeight() + 2;
            pm.line(2, top, report_page_width, top);
            top ++;
            firstIdram = false;
        }
        pm.checkForNewPage(top);
        pm.text(m_sqlDrv->val().toString(), 2, top);
        top += pm.lastTextHeight() + 2;
        pm.textRightAlign(QString("%1 - %2")
                          .arg(m_sqlDrv->val().toInt())
                          .arg(QString::number(m_sqlDrv->val().toFloat(), 'f', 2)),
                          report_page_width, top);
        top += pm.lastTextHeight() + 2;
        pm.line(2, top, report_page_width, top);
        totalIdram += m_sqlDrv->valFloat("AMOUNT");
    }
    if (totalIdram > 1) {
        pm.checkForNewPage(top);
        pm.text(tr("IDram total"), 2, top);
        pm.textRightAlign(QString::number(totalIdram, 'f', 2), report_page_width, top);
    }

     Jazzve
    m_sqlDrv->prepare("select md.name, sum(od.qty) as qty, sum(od.qty*od.price) as amount "
                      "from o_order o, o_dishes od, me_dishes md "
                      "where o.id=od.order_id and od.dish_id=md.id and o.state_id=2 and od.state_id=1 "
                      "and o.date_cash between :date1 and :date2 "
                      "and od.dish_id in (86049, 86050, 83822, 90460, 83823, 90461) "
                      "group by 1 ");
    m_sqlDrv->bind(":date1", QDate::fromString(m_filter["date1"], DATE_FORMAT));
    m_sqlDrv->bind(":date2", QDate::fromString(m_filter["date2"], DATE_FORMAT));
    m_sqlDrv->execSQL();
    bool firstJazzve = true;
    float totalJazzve = 0.0;
    while (m_sqlDrv->next()) {
        if (!another) {
            pm.checkForNewPage(top);
            top += 20;
            pm.setFontBold(true);
            pm.text(tr("Other information"), 2, top);
            pm.setFontBold(false);
            top += pm.lastTextHeight() + 1;
            pm.line(2, top, report_page_width, top);
            top ++;
            pm.line(2, top, report_page_width, top);
            top ++;
            another = true;
        }
        if (firstJazzve) {
            pm.checkForNewPage(top);
            top += pm.lastTextHeight() + 20;
            pm.text(tr("Jazzve"), 2, top);
            top += pm.lastTextHeight() + 2;
            pm.line(2, top, report_page_width, top);
            top ++;
            firstJazzve = false;
        }
        pm.checkForNewPage(top);
        pm.text(m_sqlDrv->valStr("NAME"), 2, top);
        top += pm.lastTextHeight() + 2;
        pm.text(QString::number(m_sqlDrv->valFloat("QTY"), 'f', 2), 2, top);
        pm.textRightAlign(QString::number(m_sqlDrv->valFloat("AMOUNT"), 'f', 2), report_page_width, top);
        top += pm.lastTextHeight() + 2;
        pm.line(2, top, report_page_width, top);
        totalJazzve += m_sqlDrv->valFloat("AMOUNT");
    }
    if (totalJazzve > 1) {
        pm.checkForNewPage(top);
        pm.text(tr("Jazzve total"), 2, top);
        pm.textRightAlign(QString::number(totalJazzve, 'f', 2), report_page_width, top);
    }
    */
    top += 20;
    pm.checkForNewPage(top);
    pm.setFontSize(8);
    pm.text(QString("%1: %2").arg(tr("Printed")).arg(QDateTime::currentDateTime().toString(DATETIME_FORMAT)), 2, top);
    top += pm.lastTextHeight() + 2;
    pm.text(".", 1, top);
    pm.finishPage();

    ThreadPrinter *tp = new ThreadPrinter(FF_SettingsDrv::value(SD_CHECK_PRINTER_NAME).toString(), sm, pm);
    tp->start();
}
