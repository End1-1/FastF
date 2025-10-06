#include "od_print.h"
#include <QProcess>
#include "qsystem.h"
#include <math.h>
#include <QHostInfo>
#include "od_drv.h"
#include "ff_settingsdrv.h"
#include <QMessageBox>
#include "c5printing.h"
#include <QFile>
#include <QPrinterInfo>
#include <QApplication>
#include <QJsonObject>
#include <QJsonDocument>

QFont OD_Print::mfFont;
QMap<int, QStringList> OD_Print::m_printSchema;

OD_Print::OD_Print()
{
}

OD_Print::~OD_Print()
{
}

void OD_Print::getPrinterSchema(OD_Base *db)
{
    m_printSchema.clear();

    /* Database must be opened */
    if(db->prepare("select schema_id, name from me_printers")) {
        if(db->execSQL()) {
            while(db->next()) {
                if(!m_printSchema.contains(db->v_int(0))) {
                    m_printSchema.insert(db->v_int(0), QStringList());
                }

                m_printSchema[db->v_int(0)].append(db->v_str(1));
            }
        }
    }
}

bool OD_Print::printService(int remind, const QString &objName, QList<OD_Dish*>& dishes, OD_Header &header, DbDriver &db)
{
    QMap<int, float> list;
    QMap<QString, QList<int> > printSchema;

    if(!db.openDB())
        return false;

    for(int i = 0; i < dishes.count(); i++) {
        if(dishes.at(i)->f_stateId != DISH_STATE_NORMAL)
            continue;

        if(dishes.at(i)->f_totalQty > dishes.at(i)->f_printedQty) {
            list[i] = dishes.at(i)->f_totalQty - dishes.at(i)->f_printedQty;
            dishes.at(i)->f_printedQty = dishes.at(i)->f_totalQty;
            dishes.at(i)->m_saved = false;
        }
    }

    for(int i = 0; i < dishes.count(); i++)
        if(!dishes.at(i)->saveToDB(db))
            return false;

    if(remind) {
        if(!db.prepare("insert into o_dishes_reminder "
                       "(state_id, date_register, record_id, staff_id, table_id, dish_id, qty, reminder_id) "
                       "values "
                       "(:state_id, :date_register, :record_id, :staff_id, :table_id, :dish_id, :qty, :reminder_id)"))
            return false;

        for(QMap<int, float>::const_iterator i = list.begin(); i != list.end(); i++) {
            if(dishes[i.key()]->f_remind > 0) {
                OD_Dish *d = dishes[i.key()];
                db.bindValue(":state_id", 0);
                db.bindValue(":date_register", QDateTime::currentDateTime());
                db.bindValue(":record_id", d->f_id);
                db.bindValue(":staff_id", header.f_currStaffId);
                db.bindValue(":table_id", header.f_tableId);
                db.bindValue(":dish_id", d->f_dishId);
                db.bindValue(":qty", i.value());
                db.bindValue(":reminder_id", dishes[i.key()]->f_remind);

                if(!db.execSQL()) {
                    return false;
                }
            }
        }
    }

    db.closeDB();

    for(QMap<int, float>::const_iterator it = list.begin(); it != list.end(); it++) {
        QStringList prn;

        if(!dishes[it.key()]->f_print1.isEmpty()) {
            prn << dishes[it.key()]->f_print1;
        }

        if(!dishes[it.key()]->f_print2.isEmpty()) {
            prn << dishes[it.key()]->f_print2;
        }

        for(QStringList::const_iterator p = prn.begin(); p != prn.end(); p++) {
            if(!printSchema.contains(*p))
                printSchema.insert(*p, QList<int>());

            printSchema[*p].append(it.key());
        }
    }

    for(QMap<QString, QList<int> >::const_iterator it = printSchema.begin(); it != printSchema.end(); it++) {
        QFont font(qApp->font());
        font.setPointSize(30);
        C5Printing p;
        p.setSceneParams(650, 2700, QPageLayout::Portrait);
        p.setFont(font);
        p.ctext(tr("New order"));
        p.br();
        p.ctext(tr("Order number") + " " + header.f_id);
        p.br();
        p.ltext(tr("Table"), 0);
        p.rtext(objName + " / " + header.f_tableName);
        p.br();
        p.setFontSize(22);
        p.ltext(tr("Staff"), 0);
        p.rtext(header.f_currStaffName);
        p.br();
        p.ltext(tr("Date"), 0);
        p.rtext(QDateTime::currentDateTime().toString("MM.yyyy HH:mm:ss"));
        p.br();
        p.line(5);
        p.br(2);
        p.setFontSize(28);

        for(QList<int>::const_iterator dish = it.value().begin(); dish != it.value().end(); dish++) {
            p.ltext(dishes[*dish]->f_dishName, 0);
            p.br();

            if(dishes[*dish]->f_comments.length()) {
                p.ltext(dishes[*dish]->f_comments, 0);
                p.br();
            }

            p.setFontBold(true);
            p.ltext(QString("%1").arg(list[*dish]), 0);
            p.setFontBold(false);
            p.br();
            p.line(3);
            p.br(2);
        }

        p.line(3);
        p.br();
        p.br();
        p.br();
        p.br();
        p.br();
        p.ltext("_", 0);
        p.print(it.key(), QPageSize::Custom);
    }

    return true;
}

void OD_Print::printCheckout(const QString &prnName, OD_Drv *d)
{
    /*
     * if (d->m_header.f_printQty < 0)
        d->m_header.f_printQty *= -1;
    d->m_header.f_printQty++;

    SizeMetrics sm(___printerInfo->resolution(prnName));
    XmlPrintMaker pm(&sm);

    pm.setFontName(mfFont.family());
    pm.setFontSize(10);
    int top = 5;

    QString logoFile = QSystem::appPath() + "\\logo.png";
    if (QFile::exists(logoFile))
        top += pm.imageCenter(logoFile, page_width) + 1;
    pm.ltext(tr("Order number"), 1);
    pm.ltextRightAlign(d->m_header.f_id, page_width);
    top += pm.lastTextHeight() + 1;
    pm.ltext(tr("Table"), 1);
    pm.ltextRightAlign(d->mfObjectName + " / " + d->m_header.f_tableName, page_width);
    top += pm.lastTextHeight() + 1;
    pm.ltext(tr("Staff"), 1);
    pm.ltextRightAlign(d->m_header.f_currStaffName, page_width);
    top += pm.lastTextHeight() + 1;
    pm.ltext(tr("Date"), 1);
    pm.ltextRightAlign(datets(d->m_header.f_dateCash), page_width);
    pm.br()
    pm.line(0, page_width);
    top++;
    pm.line(0, page_width);
    float otherDisc = 0;

    for (int i = 0; i <d->m_dishes.count(); i++) {
        if (d->m_dishes[i]->f_stateId != DISH_STATE_NORMAL)
            continue;
        if (d->m_dishes[i]->f_paymentMod != DISH_MOD_NORMAL)
            continue;
        top++;
        QString dishName = d->m_dishes[i]->f_dishName;
        if (d->m_dishes[i]->f_amountDec > 0)
            if (d->m_dishes[i]->f_priceDec != d->m_header.f_amount_dec_value) {
                dishName += "*";
                float cnt = d->m_dishes[i]->f_totalQty * d->m_dishes[i]->f_price;
                cnt += (cnt * d->m_dishes[i]->f_priceInc);
                cnt *= d->m_dishes[i]->f_priceDec;
                otherDisc += cnt;
            }
        pm.ltext(dishName, 1, page_width);
        top += pm.lastTextHeight() + 1;
        pm.ltext(dts(d->m_dishes[i]->f_totalQty), 10);
        pm.ltext(dts(d->m_dishes[i]->f_price), 20);
        pm.ltextRightAlign(dts(d->m_dishes[i]->f_amount), page_width);
        pm.br()
        pm.line(0, page_width);

    }

    for (int i = 0; i <d->m_dishes.count(); i++) {
        if (d->m_dishes[i]->f_stateId != DISH_STATE_NORMAL)
            continue;
        if (d->m_dishes[i]->f_paymentMod != DISH_MOD_NOINCDEC)
            continue;
        top++;
        pm.ltext(d->m_dishes[i]->f_dishName, 1, page_width);
        top += pm.lastTextHeight() + 1;
        pm.ltext(dts(d->m_dishes[i]->f_totalQty), 10);
        pm.ltext(dts(d->m_dishes[i]->f_price), 20);
        pm.ltextRightAlign(dts(d->m_dishes[i]->f_amount), page_width);
        pm.br()
        pm.line(0, page_width);

    }

    top += 2;
    pm.line(0, page_width);
    top++;

    pm.ltext(tr("Counted"), 1);
    pm.ltextRightAlign(dts(d->m_header.f_amount + d->m_header.f_amount_dec - d->m_header.f_amount_inc), page_width);
    top += pm.lastTextHeight() + 1;

    if (d->m_header.f_amount_inc > 0) {
        pm.ltext(QString("%1 %2%").arg(tr("Service")).arg(d->m_header.f_amount_inc_value * 100), 1);
        pm.ltextRightAlign(dts(d->m_header.f_amount_inc), page_width);
        top += pm.lastTextHeight() + 1;

    }

    if (d->m_header.f_amount_dec_value > 0) {
        pm.ltext(QString("%1 %2%").arg(tr("Discount")).arg(d->m_header.f_amount_dec_value * 100), 1);
        pm.ltextRightAlign(dts(d->m_header.f_amount_dec - otherDisc), page_width);
        top += pm.lastTextHeight() + 1;

    }

    if (otherDisc > 0.01) {
        pm.ltext(tr("* Partial discount"), 1);
        pm.ltextRightAlign(dts(otherDisc), page_width);
        top += pm.lastTextHeight() + 1;

    }

    top++;
    pm.line(0, page_width);
    top++;
    pm.line(0, page_width);

    pm.setFontBold(true);
    pm.setFontSize(12);
    top += 2;
    pm.ltext(tr("Total"), 1);
    pm.ltextRightAlign(dts(d->m_header.f_amount), page_width);

    top += pm.lastTextHeight() + 1;

    pm.setFontBold(false);
    pm.setFontItalic(true);
    pm.setFontSize(6);
    pm.ltext(tr("Thank you for visit"), 1);

    top += pm.lastTextHeight() + 10;

    pm.setFontItalic(false);
    pm.ltext(QString("%1: %2, %3: %4").arg(tr("Printed"))
            .arg(QDateTime::currentDateTime().toString("dd.MM.yyyy HH:mm:ss"))
            .arg(tr("Smpl."))
            .arg(d->m_header.f_printQty), 1);
    top += pm.lastTextHeight() + 1;

    QHostInfo h;
    pm.ltext(h.hostName() + "/" + prnName, 1);




    top += pm.lastTextHeight() + 3;
    pm.ltext(tr("Payment avaiable with IDram"), 10);
    top += pm.lastTextHeight() + 1;
    QString qrCodeFile = QString("%1%2%3.bmp").arg(QSystem::homePath()).arg(d->m_header.f_id).arg(d->m_header.f_printQty);
    qrcodeToFile(qrCodeFile.toLatin1().data(),
                 QString("%1;%2;%3;%4|%5;%6;")
               .arg("Jazzve")
               .arg(FF_SettingsDrv::value(SD_IDRAM_ID).toString()) //IDram ID
               .arg(d->m_header.f_amount)
               .arg(d->m_header.f_id)
               .arg(FF_SettingsDrv::value(SD_IDRAM_PHONE).toString())
               .arg("1").toLatin1().data());

    top += pm.imageCenter(qrCodeFile + 5, page_width, true);


    top += 5;
    pm.ltext(".", 1);

    pm.finishPage();

    ThreadPrinter *tp = new ThreadPrinter(prnName, sm, pm);
    tp->start();
    */
    QMessageBox::information(0, "Info", "Old version of print");
}

void OD_Print::printTax(const QString &ip, const QString &port, const QString &pass, OD_Drv *d, bool print)
{
    if(!port.toInt()) {
        if(!d->m_header.f_taxPrint)
            d->m_header.f_taxPrint = FF_SettingsDrv::value(SD_DEFAULT_HALL_ID).toInt();

        d->m_saved = false;
        d->saveAll();
        return;
    }

    QString fileName = QString("tax_%1.json")
                       .arg(d->m_header.f_id);
    QFile f(QSystem::appPath() + "\\" + fileName);

    if(f.open(QIODevice::WriteOnly)) {
        float amount = d->m_header.f_amount - d->m_header.f_amountCard ;
        float amountCard = d->m_header.f_amountCard;
        f.write((QString("{\"seq\":1,\"paidAmount\":%1, \"paidAmountCard\":%2, \"useExtPOS\":true, \"mode\":2,\"items\":[")
                 .arg(QString::number(amount, 'f', 2))
                 .arg(QString::number(amountCard, 'f', 2))).toUtf8());
        bool first = true;

        for(int i = 0; i < d->m_dishes.count(); i++) {
            if(d->m_dishes[i]->f_stateId != DISH_STATE_NORMAL)
                continue;

            if(d->m_dishes[i]->f_price < 0.01)
                continue;

            if(!first)
                f.write(",");
            else
                first = false;

            float price = d->m_dishes[i]->f_price;
            price += (price * d->m_dishes[i]->f_priceInc);
            price -= (price * d->m_dishes[i]->f_priceDec);
            f.write(QString("{\"dep\":%1,\"qty\":%2,\"price\":%3,\"productCode\":\"%4\",\"productName\":\"%5\",\"adgCode\":\"%6\", \"unit\":\"%7\"}")
                    .arg(FF_SettingsDrv::value(SD_TAX_PRINT_TERMINAL_DEP).toString())
                    .arg(d->m_dishes[i]->f_totalQty)
                    .arg(QString::number(price, 'f', 2))
                    .arg(d->m_dishes[i]->f_dishId)
                    .arg(d->m_dishes[i]->f_dishName)
                    .arg(d->m_dishes[i]->f_adgCode)
                    .arg(QString::fromUtf8("հատ"))
                    .toUtf8());
        }

        f.write("]}");
        f.close();

        if(print) {
            QStringList args;
            args << ip << port << pass << fileName;
            QProcess *p = new QProcess();
            p->start(QSystem::appPath() + "\\printtax.exe", args);
        }
    }
}

void OD_Print::printTax(const QString &orderId, DbDriver &db)
{
    if(!db.openDB())
        return;

    if(!db.prepare("select payment from o_order where id=:id"))
        return;

    db.bindValue(":id", orderId);

    if(!db.execSQL())
        return;

    db.next();
    int payMethod = db.v_int(0);

    if(!db.prepare("select od.dish_id, d.name, od.qty, od.price, od.price_inc, od.price_dec, mt.adgcode "
                   "from o_dishes od, me_dishes d, me_types mt "
                   "where od.dish_id=d.id and d.type_id=mt.id and od.order_id=:order_id and od.state_id=1 and price > 0.01"))
        return;

    db.bindValue(":order_id", orderId);

    if(!db.execSQL())
        return;

    float amount = 0, amountCard = 0;
    QStringList codeList;
    QStringList nameList;
    QStringList qtyList;
    QStringList priceList;
    QStringList adgCode;

    while(db.next()) {
        float qty = db.v_dbl(2);
        float price = db.v_dbl(3);
        float priceInc = db.v_dbl(4);
        float priceDec = db.v_dbl(5);
        price += (price * priceInc);
        price -= (price * priceDec);
        amount += price * qty;
        codeList.append(db.v_str(0));
        nameList.append(db.v_str(1));
        adgCode.append(db.v_str(6));
        qtyList.append(QString::number(db.v_dbl(2), 'f', 2));
        priceList.append(QString::number(price, 'f', 2));
    }

    if(!db.prepare("update o_order set taxprint = 0 where id=:id"))
        return;

    db.bindValue(":id", orderId);

    if(!db.execSQL())
        return;

    db.closeDB();

    if(payMethod == 2) {
        amountCard = amount;
        amount = 0;
    }

    QString fileName = QString("tax_%1.json").arg(orderId);
    QFile f(QSystem::appPath() + "\\" + fileName);

    if(f.open(QIODevice::WriteOnly)) {
        f.write((QString("{\"seq\":1,\"paidAmount\":%1, \"paidAmountCard\":%2, \"useExtPOS\":true, \"mode\":2,\"items\":[")
                 .arg(QString::number(amount, 'f', 2))
                 .arg(QString::number(amountCard, 'f', 2))).toUtf8());
        bool first = true;

        for(int i = 0; i < codeList.count(); i++) {
            if(!first)
                f.write(",");
            else
                first = false;

            f.write(QString("{\"dep\":%1,\"qty\":%2,\"price\":%3,\"productCode\":\"%4\",\"productName\":\"%5\",\"adgCode\":\"%6\", \"unit\":\"%7\"}")
                    .arg(FF_SettingsDrv::value(SD_TAX_PRINT_TERMINAL_DEP).toString())
                    .arg(qtyList.at(i))
                    .arg(priceList.at(i))
                    .arg(codeList.at(i))
                    .arg(nameList.at(i))
                    .arg(adgCode.at(i))
                    .arg(QString::fromUtf8("հատ"))
                    .toUtf8());
        }

        f.write("]}");
        f.close();
        QStringList args;
        args << FF_SettingsDrv::value(SD_TAX_PRINT_IP).toString() << FF_SettingsDrv::value(SD_TAX_PRINT_PORT).toString() << FF_SettingsDrv::value(SD_TAX_PRINT_PASS).toString() << fileName;
        QProcess *p = new QProcess();
        p->start(QSystem::appPath() + "\\printtax.exe", args);
    }
}
