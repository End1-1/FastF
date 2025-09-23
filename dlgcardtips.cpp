#include "dlgcardtips.h"
#include "ui_dlgcardtips.h"
#include "wdtnumpad.h"
#include "printtaxn.h"
#include "qsqldrv.h"
#include "msqldatabase.h"
#include <QJsonObject>
#include <QJsonDocument>
#include <QMessageBox>

DlgCardTips::DlgCardTips(const QString &order, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::DlgCardTips),
    fOrder(order)
{
    ui->setupUi(this);
    ui->wdt->configDouble();
    connect(ui->wdt, &WdtNumpad::keyEnter, this, &DlgCardTips::enter);
    connect(ui->wdt, &WdtNumpad::cancel, this, &DlgCardTips::cancel);
    QSqlDrv qd("admin", "main");
    if (qd.prepare(QString("select sum(famount) as a from o_card_tips where forder='%1'").arg(fOrder))) {
        if (qd.execSQL()) {
            if (qd.next()) {
                ui->lbtea->setText(qd.valStr("a"));
            }
        }
    }


    adjustSize();
}

DlgCardTips::~DlgCardTips()
{
    delete ui;
}

void DlgCardTips::cancel()
{
    reject();
}

void DlgCardTips::enter()
{
    double d = ui->wdt->password().toDouble();
    if (d < 0.01) {
        return;
    }
    QSqlDrv qd("admin", "main");
    if (!qd.prepare("select * from s_nconfig where fid='fiscal'")) {
        return;
    }
    if (!qd.execSQL()) {
        return;
    }
    if (qd.next() == false) {
        return;
    }
    QJsonObject jtax = QJsonDocument::fromJson(qd.m_query->value("fdata").toString().toUtf8()).object();
    jtax = jtax["fiscal"].toObject();
    PrintTaxN pt(jtax["ip"].toString(), jtax["port"].toInt(), jtax["password"].toString(), "true", jtax["opcode"].toString(), jtax["oppin"].toString(), this);
    pt.addGoods(1, "56.10", "0001", "Սպասարկման վարձ", d, 1, 0 );
    QString jsonIn, jsonOut, err;
    int result = 0;
    result = pt.makeJsonAndPrint(d, 0, jsonIn, jsonOut, err);
    QMap<QString, QVariant> v;
    DatabaseResult dr;
    v[":forder"] = fOrder;
    v[":fdate"] = QDateTime::currentDateTime();
    v[":fin"] = jsonIn;
    v[":fout"] = jsonOut;
    v[":ferr"] = err;
    v[":fresult"] = result;
    MSqlDatabase fDb;
    fDb.setConnectionParams(qd.m_db.hostName(), qd.m_db.databaseName(), qd.m_db.userName(), qd.m_db.password());
    fDb.configure();
    fDb.insert("o_tax_log", v);
    if (result == pt_err_ok) {
        v[":forder"] = fOrder;
        v[":famount"] = d;
        v[":ffiscal"] = jsonOut;
        fDb.insert("o_card_tips", v);
        accept();
    } else {
        QMessageBox::critical(this, "", err);
    }
}
