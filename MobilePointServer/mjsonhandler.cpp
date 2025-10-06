#include "mjsonhandler.h"
#include "msqldatabase.h"
#include "cnfmaindb.h"
#include "mdefined.h"
#include "mtprintkitchen.h"
#include "mtfilelog.h"
#include "cnfapp.h"
#include "dbdriver.h"
#include "printtaxn.h"
#include <QUuid>
#include <QNetworkInterface>
#include <QCryptographicHash>
#include <QPrinterInfo>
#include <QFile>
#include <QMutex>

MJsonHandler::MJsonHandler(QObject *parent) : QObject(parent)
{
    if(!fDb.configure()) {
        fError = fDb.lastError();
    }
}

QJsonObject MJsonHandler::handleDish(const QJsonObject &o)
{
    Q_UNUSED(o)
    DatabaseResult dr;
    QMap<QString, QVariant> bind;
    bind[":menuid"] = o["menuid"].toString().toInt();
    fDb.select("select md.id, md.type_id, md.name, mm.price, mm.print1, mm.print2, mm.store_id, md.remind "
               "from me_dishes_menu mm "
               "inner join me_dishes md on md.id=mm.dish_id "
               "where mm.menu_id=:menuid and mm.state_id=1 ", bind, dr);
    QJsonArray jArr;

    for(int i = 0; i < dr.rowCount(); i++) {
        QJsonObject jt;
        jt["id"] = dr.toString(i, "ID");
        jt["type"] = dr.toString(i, "TYPE_ID");
        jt["name"] = dr.toString(i, "NAME");
        jt["price"] = dr.toString(i, "PRICE");
        jt["print1"] = dr.toString(i, "PRINT1");
        jt["print2"] = dr.toString(i, "PRINT2");
        jt["store"] = dr.toString(i, "STORE_ID");
        jt["remind"] = dr.toString(i, "REMIND");
        jArr.push_back(jt);
    }

    QJsonObject jObj;
    jObj["reply"] = "ok";
    jObj["list"] = jArr;
    jObj["type"] = "dish";
    return jObj;
}

QJsonObject MJsonHandler::handleDishType(const QJsonObject &o)
{
    Q_UNUSED(o)
    DatabaseResult dr;
    QMap<QString, QVariant> bind;
    bind[":menuid"] = o["menuid"].toString().toInt();
    fDb.select("select id, part_id, name from me_types where id in "
               "(select type_id from me_dishes where id in "
               "(select dish_id from me_dishes_menu where menu_id=:menuid and state_id=1))", bind, dr);
    QJsonArray jArr;

    for(int i = 0; i < dr.rowCount(); i++) {
        QJsonObject jt;
        jt["id"] = dr.value(i, "ID").toString();
        jt["part"] = dr.value(i, "PART_ID").toString();
        jt["name"] = dr.value(i, "NAME").toString();
        jArr.push_back(jt);
    }

    QJsonObject jObj;
    jObj["reply"] = "ok";
    jObj["list"] = jArr;
    jObj["type"] = "dishtype";
    return jObj;
}

QJsonObject MJsonHandler::handleHall(const QJsonObject &o)
{
    Q_UNUSED(o)
    MSqlDatabase db;
    DatabaseResult dr;
    QMap<QString, QVariant> bind;
    fDb.select("select t.id, t.hall_id, t.name, o.staff_id, e.fname || ' ' || e.lname as staff_name, "
               "o.amount, o.print_qty "
               "from h_table t "
               "left join o_order o on o.id=t.order_id "
               "left join employes e on e.id=o.staff_id "
               "order by queue", bind, dr);
    QJsonArray jArr;

    for(int i = 0; i < dr.rowCount(); i++) {
        QJsonObject jt;
        jt["id"] = dr.value(i, "ID").toString();
        jt["hall"] = dr.value(i, "HALL_ID").toString();
        jt["name"] = dr.value(i, "NAME").toString();
        jt["staffid"] = dr.value(i, "STAFF_ID").toString();
        jt["staffname"] = dr.value(i, "STAFF_NAME").toString();
        jt["amount"] = float_str(dr.value(i, "AMOUNT").toDouble(), 0);
        jt["printed"] = dr.value(i, "PRINT_QTY").toString();
        jArr.push_back(jt);
    }

    QJsonObject jObj;
    jObj["reply"] = "ok";
    jObj["list"] = jArr;
    jObj["type"] = "hall";
    return jObj;
}

QJsonObject MJsonHandler::handleDishToOrder(const QJsonObject &o)
{
    int table = o["table"].toString().toInt();
    QString order = o["order"].toString();
    QMap<QString, QVariant> v;

    if(order.isEmpty()) {
        DatabaseResult dorder;
        v[":table_id"] = table;
        v[":state_id"] = 1;

        if(!fDb.select("select id from o_order where table_id=:table_id and state_id=:state_id", v, dorder, false)) {
            return jsonError(fDb.lastError());
        }

        if(dorder.rowCount() == 0) {
            order = QString("%1-%2").arg("J3").arg(fDb.genId("gen_o_order_id"));
            v[":order_id"] = order;

            if(!fDb.update("h_table", v, table)) {
                return jsonError(fDb.lastError());
            }

            v[":id"] = order;
            v[":state_id"] = 1;
            v[":table_id"] = table;
            v[":date_open"] = QDateTime::currentDateTime();
            v[":date_close"] = QDateTime::currentDateTime();
            v[":date_cash"] = QDate::currentDate();
            v[":staff_id"] = o["session"];
            v[":print_qty"] = 0;
            v[":amount_inc"] = 0;
            v[":amount_dec"] = 0;
            v[":amount_inc_value"] = o["priceinc"].toString().toDouble();
            v[":amount_dec_value"] = 0;
            v[":payment"] = 1;
            v[":taxprint"] = 0;
            v[":comment"] = "";
            v[":cash_id"] = 3;

            if(!fDb.insert("o_order", v)) {
                return jsonError(fDb.lastError());
            }
        } else {
            order = dorder.value("ID").toString();
        }
    }

    v[":id"] = order;
    DatabaseResult dr;
    fDb.select("select print_qty from o_order where id=:id", v, dr, false);

    if(dr.rowCount() > 0) {
        if(dr.value("PRINT_QTY").toInt() > 0) {
            return jsonError(tr("Only close order is available"));
        }
    }

    int rec = fDb.genId("gen_o_dish_id");
    v[":id"] = rec;
    v[":order_id"] = order;
    v[":state_id"] = 1;
    v[":dish_id"] = o["dish"].toString().toInt();
    v[":qty"] = o["qty"].toString().toDouble();
    v[":printed_qty"] = 0;
    v[":price"] = o["price"].toString().toDouble();
    v[":price_inc"] = o["priceinc"].toString().toDouble();
    v[":price_dec"] = 0;
    v[":last_user"] = o["session"];
    v[":store_id"] = o["store"].toString().toInt();
    v[":payment_mod"] = 1;
    v[":comments"] = "";
    v[":remind"] = o["remind"].toString().toInt();
    v[":print1"] = o["print1"].toString();
    v[":print2"] = o["print2"].toString();
    fDb.insert("o_dishes", v);
    QString total = updateOrderAmount(order);
    QJsonObject jObj;
    jObj["order"] = order;
    jObj["rec"] = rec;
    jObj["reply"] = "ok";
    jObj["total"] = total;
    return jObj;
}

QJsonObject MJsonHandler::handleOpenTable(const QJsonObject &o)
{
    int table = o["table"].toString().toInt();
    QMap<QString, QVariant> v;
    v[":table_id"] = table;
    v[":state_id"] = 1;
    DatabaseResult dr;
    fDb.select("select id, amount from o_order where table_id=:table_id and state_id=:state_id", v, dr);
    QJsonObject obj;

    if(dr.rowCount() > 0) {
        obj["reply"] = "ok";
        obj["order"] = dr.value("ORDER_ID").toString();
        obj["amount"] = float_str(dr.value("AMOUNT").toDouble(), 0);
    } else {
        obj["reply"] = "ok";
        obj["order"] = "";
    }

    return obj;
}

QJsonObject MJsonHandler::handleLoadOrder(const QJsonObject &o)
{
    QString order = o["order"].toString();

    if(order.isEmpty()) {
        return jsonError("Incorrect order number");
    }

    QJsonObject obj;
    QMap<QString, QVariant> v;
    DatabaseResult dr;
    v[":id"] = order;
    fDb.select("select o.state_id, e.fname || ' ' || e.lname as staff, o.amount from o_order o "
               "left join employes e on e.id=o.staff_id "
               "where o.id=:id ", v, dr);

    if(dr.rowCount() == 0) {
        return jsonError("Incorrect order number");
    }

    if(dr.toInt(0, "STATE_ID") != 1) {
        return jsonError("Order state is not opened");
    }

    obj["reply"] = "ok";
    obj["staff"] = dr.toString(0, "STAFF");
    obj["amount"] = float_str(dr.value(0, "AMOUNT").toDouble(), 0);
    v[":order_id"] = order;
    fDb.select("select od.id, od.dish_id, md.name, od.qty, od.printed_qty, od.comments, "
               "od.print1, od.print2, od.price "
               "from  o_dishes od "
               "left join me_dishes md on md.id=od.dish_id "
               "where od.order_id=:order_id and od.state_id=1 ", v, dr);
    QJsonArray ja;

    for(int i = 0; i < dr.rowCount(); i++) {
        QJsonObject od;
        od["recid"] = dr.toString(i, "ID");
        od["dishid"] = dr.toString(i, "DISH_ID");
        od["dishname"] = dr.toString(i, "NAME");
        od["qty"] = dr.toString(i, "QTY");
        od["printedqty"] = dr.toString(i, "PRINTED_QTY");
        od["comments"] = dr.toString(i, "COMMENTS");
        od["price"] = dr.toString(i, "PRICE");
        od["print1"] = dr.toString(i, "PRINT1");
        od["print2"] = dr.toString(i, "PRINT2");
        ja.append(od);
    }

    obj["dishes"] = ja;
    return obj;
}

QJsonObject MJsonHandler::handleDishChange(const QJsonObject &o)
{
    QMap<QString, QVariant> v;
    v[":id"] = o["recid"].toString().toInt();
    DatabaseResult dr;
    fDb.select("select order_id from o_dishes where id=:id", v, dr, false);
    QString order;

    if(dr.rowCount() > 0) {
        order = dr.value("ORDER_ID").toString();
    }

    v[":id"] = order;
    fDb.select("select print_qty from o_order where id=:id", v, dr, false);

    if(dr.rowCount() > 0) {
        if(dr.value("PRINT_QTY").toInt() > 0) {
            return jsonError(tr("Only close order is available"))       ;
        }
    }

    v[":qty"] = o["qty"].toString().toDouble();
    v[":comments"] = o["comments"].toString();

    if(!fDb.update("o_dishes", v, o["recid"].toString().toInt())) {
        return jsonError(fDb.lastError());
    }

    v[":id"] = o["recid"].toString().toInt();
    fDb.select("select order_id from o_dishes where id=:id", v, dr);
    QString total = "0";

    if(dr.rowCount() > 0) {
        total = updateOrderAmount(dr.value("ORDER_ID").toString());
    }

    QJsonObject obj;
    obj["reply"] = "ok";
    obj["total"] = total;
    return obj;
}

QJsonObject MJsonHandler::handleDishRemove(const QJsonObject &o)
{
    QMap<QString, QVariant> v;
    v[":state_id"] = DISH_STATE_REMOVED_NORMAL;

    if(!fDb.update("o_dishes", v, o["recid"].toString().toInt())) {
        return jsonError(fDb.lastError());
    }

    DatabaseResult dr;
    v[":id"] = o["recid"].toString().toInt();
    fDb.select("select order_id from o_dishes where id=:id", v, dr);
    QString total = "0";

    if(dr.rowCount() > 0) {
        total = updateOrderAmount(dr.value("ORDER_ID").toString());
    }

    QJsonObject obj;
    obj["reply"] = "ok";
    obj["total"] = total;
    return obj;
}

QJsonObject MJsonHandler::handlePrintKitchen(const QJsonObject &o)
{
    QString order = o["order"].toString();

    if(order.isEmpty()) {
        return jsonError("Incorrect order number");
    }

    QMap<QString, QVariant> v;
    DatabaseResult dr;
    v[":order_id"] = order;
    v[":state_id"] = DISH_STATE_NORMAL;
    fDb.select("select od.id, md.name, od.qty-od.printed_qty as qty, od.comments, od.print1, od.print2, "
               "od.remind, od.dish_id "
               "from o_dishes od "
               "left join me_dishes md on md.id=od.dish_id "
               "where od.order_id=:order_id and od.state_id=:state_id and od.printed_qty<od.qty "
               "and (char_length(od.print1)>0 or char_length(od.print2)>0) ", v, dr);
    DatabaseResult dh;
    v[":id"] = order;
    fDb.select("select h.name as hname, t.name tname, e.fname || ' ' || e.lname ename, "
               "o.staff_id, o.table_id "
               "from o_order o "
               "left join h_table t on t.id=o.table_id "
               "left join h_hall h on h.id=t.hall_id "
               "left join employes e on e.id=o.staff_id "
               "where o.id=:id", v, dh);
    QMap<QString, QString> data;
    data["order"] = order;
    data["hall"] = dh.toString(0, "HNAME");
    data["table"] = dh.toString(0, "TNAME");
    data["staff"] = dh.toString(0, "ENAME");
    QSet<QString> prn1;
    QSet<QString> prn2;

    for(int i = 0; i < dr.rowCount(); i++) {
        prn1.insert(dr.value(i, "PRINT1").toString());

        if(dr.value(i, "PRINT1").toString() != dr.value(i, "PRINT2").toString()) {
            prn2.insert(dr.value(i, "PRINT2").toString());
        }
    }

    foreach(QString prn, prn1) {
        data["printer"] = prn;
        QList<QMap<QString, QString> > dishes;

        for(int i = 0; i < dr.rowCount(); i++) {
            if(dr.toString(i, "PRINT1") == prn) {
                QMap<QString, QString> d;
                d["qty"] = float_str(dr.toDouble(i, "QTY"), 1);
                d["dish"] = dr.toString(i, "NAME");
                d["comments"] = dr.toString(i, "COMMENTS");
                dishes.append(d);
            }
        }

        if(dishes.count() > 0) {
            MTPrintKitchen *p = new MTPrintKitchen(data, dishes, true);
            p->run();
        }
    }

    foreach(QString prn, prn2) {
        data["printer"] = prn;
        QList<QMap<QString, QString> > dishes;

        for(int i = 0; i < dr.rowCount(); i++) {
            if(dr.toString(i, "PRINT2") == dr.toString(i, "PRINT1")) {
                continue;
            }

            if(dr.toString(i, "PRINT2") == prn) {
                QMap<QString, QString> d;
                d["qty"] = float_str(dr.toDouble(i, "QTY"), 1);
                d["dish"] = dr.toString(i, "NAME");
                d["comments"] = dr.toString(i, "COMMENTS");
                dishes.append(d);
            }
        }

        if(dishes.count() > 0) {
            MTPrintKitchen *p = new MTPrintKitchen(data, dishes, true, nullptr);
            p->run();
        }
    }

    fDb.open();

    for(int i = 0; i < dr.rowCount(); i++) {
        if(dr.value(i, "REMIND").toInt() > 0) {
            v[":date_start"] = QDateTime::currentDateTime();
            v[":staff_id"] = dh.value("STAFF_ID");
            v[":table_id"] = dh.value("TABLE_ID");
            v[":dish_id"] = dr.value(i, "DISH_ID");
            v[":qty"] = dr.value(i, "QTY");
            fDb.insert("o_dishes_reminder", v);
        }
    }

    v[":order_id"] = order;
    v[":state_id"] = DISH_STATE_NORMAL;
    fDb.select("update o_dishes set printed_qty=qty where order_id=:order_id and state_id=:state_id", v, dr);
    QJsonObject obj;
    obj["reply"] = "ok";
    return obj;
}

QJsonObject MJsonHandler::handleReceipt(const QJsonObject &o)
{
    QString order = o["order"].toString();

    if(order.isEmpty()) {
        return jsonError(tr("Order cannot be empty"));
    }

    QPrinterInfo pi;
#ifdef QT_DEBUG

    if(!pi.availablePrinterNames().contains(o["printer"].toString())) {
        return jsonError(tr("Incorrect printer name") + "\r\n" + o["printer"].toString());
    }

#endif
    QMap<QString, QVariant> v;
    DatabaseResult dr;
    v[":id"] = order;
    fDb.open();
    fDb.select("select print_qty from o_order where id=:id", v, dr, false);

    if(dr.rowCount() == 0) {
        return jsonError(tr("Incorrect order number"));
    }

    v[":id"] = order;
    QString sql = R"(
    select t.name as tname, h.name as hname,
    e.fname || ' ' || e.lname as ename,
    o.date_cash, o.amount, o.amount_inc,
    o.amount_dec, o.amount_inc_value, o.amount_dec_value,
    o.print_qty
    from o_order o
    left join h_table t on t.id=o.table_id
    left join employes e on e.id=o.staff_id
    left join h_hall h on h.id=t.hall_id
    where o.id=:id
    )";
    fDb.select(sql, v, dr, false);

    if(dr.rowCount() == 0) {
        return jsonError(tr("Unable to select order with id: ") + order);
    }

    if(dr.value("PRINT_QTY").toInt() == 0) {
        return jsonError(tr("Please print the precheck before continuing"));
    }

    double total;
    QMap<QString, QString> data;
    data["table"] = dr.value("TNAME").toString();
    data["hall"] = dr.value("HNAME").toString();
    data["staff"] = dr.value("ENAME").toString();
    data["date"] = dr.value("DATE_CASH").toDate().toString(def_date_format);
    data["printer"] = o["printer"].toString();
    data["order"] = order;
    total = dr.value("AMOUNT").toDouble();
    data["amount"] = float_str(total, 2);
    data["amount_inc"] = float_str(dr.value("AMOUNT_INC").toDouble(), 2);
    data["amount_dec"] = float_str(dr.value("AMOUNT_DEC").toDouble(), 2);
    data["amount_inc_value"] = float_str(dr.value("AMOUNT_INC_VALUE").toDouble() * 100, 2);
    data["amount_dec_value"] = float_str(dr.value("AMOUNT_DEC_VALUE").toDouble() * 100, 2);
    QList<QMap<QString, QString> > dishes;
    v[":order_id"] = order;
    v[":state_id"] = DISH_STATE_NORMAL;
    sql = R"(
    select d.name, od.qty, od.price, od.qty*od.price as total,
    od.qty-od.printed_qty as printed,mt.adgcode, d.id as dishid,
    od.payment_mod as pm, od.emarks
    from o_dishes od
    left join me_dishes d on d.id=od.dish_id
    left join me_types mt on mt.id=d.type_id
    where od.order_id=:order_id and od.state_id=:state_id
    )";
    fDb.select(sql, v, dr, false);

    for(int i = 0; i < dr.rowCount(); i++) {
        if(dr.value(i, "PRINTED").toDouble() > 0.1) {
            return jsonError(tr("Incomplete order"));
        }

        QMap<QString, QString> dish;
        dish["dish"] = dr.value(i, "NAME").toString();
        dish["qty"] = float_str(dr.value(i, "QTY").toDouble(), 1);
        dish["price"] = float_str(dr.value(i, "PRICE").toDouble(), 1);
        dish["total"] = float_str(dr.value(i, "TOTAL").toDouble(), 1);
        dish["adgcode"] = dr.value(i, "ADGCODE").toString();
        dish["id"] = dr.value(i, "DISHID").toString();
        dish["pm"] = dr.value(i, "PM").toString();
        dish["emarks"] = dr.value(i, "EMARKS").toString();
        dishes.append(dish);
    }

    bool nh = false;
    v[":id"] = order;
    DatabaseResult doh;
    fDb.select("select state_id from o_order where id=:id", v, doh, false);

    if(doh.rowCount() == 0) {
        return jsonError("Error. no order id");
    }

    double card = 0;

    switch(o["mode"].toInt()) {
    case PaymentIdram:
    case PaymentArca:
        card = total;
        break;
    }

    nh = doh.value("STATE_ID").toInt() == 1;
    QJsonObject obj;
    DatabaseResult dpay;
    v[":fid"] = order;
    fDb.select("select * from o_tax where fid=:fid", v, dpay, false);

    if(dpay.rowCount() == 0) {
        nh = true;
    } else {
        data["fiscal"] = dpay.toString(0, "json");
        nh = false;
    }

    if(o["mode"].toInt() == PaymentComplimentary) {
        nh = false;
    }

    if(nh) {
        double serv = data["amount_inc_value"].toDouble();
        double disc = data["amount_dec_value"].toDouble();
        //int mode = o["mode"].toString().toInt();
        PrintTaxN pt(__cnfapp.taxIP(), __cnfapp.taxPort(), __cnfapp.taxPassword(), "true", "3", "3", this);

        for(int i = 0; i < dr.rowCount(); i++) {
            double price = dishes.at(i)["price"].toDouble();

            if(dishes.at(i)["emarks"].isEmpty() == false) {
                pt.fEmarks.append(dishes.at(i)["emarks"]);
            }

            if(price < 1) {
                continue;
            }

            double tempDisc = 0.00;

            if(dishes.at(i)["pm"].toInt() == PaymentServiceDiscount) {
                tempDisc = disc;
            }

            pt.addGoods(__cnfmaindb.fTaxDepartment.toInt(),
                        dishes.at(i)["adgcode"], dishes.at(i)["id"],
                        dishes.at(i)["dish"], price, dishes.at(i)["qty"].toDouble(),
                        tempDisc);
        }

        if(data["amount_inc"].toDouble() > 0.001) {
            pt.addGoods(__cnfmaindb.fTaxDepartment.toInt(), dishes.at(0)["adgcode"], "1", tr("Service"), data["amount_inc"].toDouble(), 1.0, disc);
        }

        QString jsonIn, jsonOut, err;
        int result = 0;
        result = pt.makeJsonAndPrint(card, 0, jsonIn, jsonOut, err);
        v[":forder"] = order;
        v[":fdate"] = QDateTime::currentDateTime();
        v[":fin"] = jsonIn;
        v[":fout"] = jsonOut;
        v[":ferr"] = err;
        v[":fresult"] = result;

        if(!fDb.insert("o_tax_log", v)) {
            v[":forder"] = order;
            v[":fdate"] = QDateTime::currentDateTime();
            v[":fin"] = fDb.lastError();
            v[":fout"] = fDb.lastError();
            v[":ferr"] = err;
            v[":fresult"] = -999;
            fDb.insert("o_tax_log", v);
        }

        QJsonObject jo = QJsonDocument::fromJson(jsonOut.toUtf8()).object();

        if(result != pt_err_ok) {
            return jsonError(tr("Tax print error") + "\r\n" + err + jsonOut);
        } else {
            v[":fid"] = order;
            v[":fnumber"] = jo["rseq"].toInt();
            v[":jsonin"] = jsonIn;
            v[":json"] = jsonOut;
            v[":fcash"] = o["mode"].toInt() == PaymentCash ? total : 0;
            v[":fcard"] = o["mode"].toInt() == PaymentArca ? total : 0;
            v[":fidram"] = o["mode"].toInt() == PaymentIdram ? total : 0;

            if(!fDb.insert("o_tax", v)) {
                return jsonError(fDb.lastError());
            }

            data["fiscal"] = jsonOut;
        }
    }

    v[":id"] = order;
    fDb.select("update o_order set print_qty=abs(print_qty)+1 where id=:id", v, dr, false);
    MTPrintKitchen m(data, dishes, false, nullptr);
    m.run();
    obj["reply"] = "ok";
    return obj;
}

QString MJsonHandler::handleDishInfo(const QJsonObject &o)
{
    int dish = o["dish"].toString().toInt();
    return QString("%1").arg(dish).toUtf8();
}

QJsonObject MJsonHandler::handleSettings(const QJsonObject &o)
{
    Q_UNUSED(o);
    DatabaseResult dr;
    QMap<QString, QVariant> v;
    fDb.select("select id, name, price_inc, menu_id from sys_mobile_settings", v, dr);
    QJsonArray ja;

    for(int i = 0; i < dr.rowCount(); i++) {
        QJsonObject obj;
        obj["id"] = dr.value(i, "ID").toString();
        obj["name"] = dr.value(i, "NAME").toString();
        obj["priceinc"] = dr.value(i, "PRICE_INC").toString();
        obj["menuid"] = dr.value(i, "MENU_ID").toString();
        ja.append(obj);
    }

    QJsonObject jReply;
    jReply["reply"] = "ok";
    jReply["list"] = ja;
    return jReply;
}

QJsonObject MJsonHandler::handleListReceipt(const QJsonObject &o)
{
    Q_UNUSED(o);
    DatabaseResult dr;
    QMap<QString, QVariant> v;
    fDb.select("select show_name, printer_name from sys_mobile_receipt_printers order by show_name", v, dr);
    QJsonArray ja;

    for(int i = 0; i < dr.rowCount(); i++) {
        QJsonObject obj;
        obj["showname"] = dr.value(i, "SHOW_NAME").toString();
        obj["printername"] = dr.value(i, "PRINTER_NAME").toString();
        ja.append(obj);
    }

    QJsonObject jReply;
    jReply["reply"] = "ok";
    jReply["type"] = "receiptprinters";
    jReply["list"] = ja;
    return jReply;
}

QJsonObject MJsonHandler::handleCloseOrder(const QJsonObject &o)
{
    DatabaseResult dr;
    QMap<QString, QVariant> v;
    QString order = o["order"].toString();

    if(order.isEmpty()) {
        return jsonError(tr("Order is empty"));
    }

    v[":order_id"] = order;
    v[":state_id"] = DISH_STATE_NORMAL;
    fDb.select("select sum(qty)-sum(printed_qty) as printed from o_dishes where order_id=:order_id and state_id=:state_id", v, dr);

    if(dr.value("PRINTED").toDouble() > 0.1) {
        return jsonError(tr("Order is incomplete"));
    }

    v[":id"] = order;
    fDb.select("select staff_id, state_id, print_qty from o_order where id=:id ", v, dr);

    if(dr.rowCount() == 0) {
        return jsonError(tr("Incorrect order number"));
    }

    if(dr.value("STATE_ID").toInt() != ORDER_STATE_OPEN) {
        return jsonError(tr("Order is not opened"));
    }

    if(dr.value("PRINT_QTY").toInt() < 1) {
        return jsonError(tr("Receipt wasnt printed"));
    }

    int userId = o["session"].toInt();

    if(userId == 0) {
        if(o.contains("staff")) {
            if(dr.value("STAFF_ID").toInt() != o["staff"].toString().toInt()) {
                return jsonError(tr("You are not owner of this order"));
            }
        } else {
            return jsonError(tr("You are not owner of this order"));
        }
    } else {
        if(userId != dr.value("STAFF_ID").toInt()) {
            return jsonError(tr("You are not owner of this order"));
        }
    }

    QJsonObject jp = handleReceipt(o);

    if(jp["reply"].toString() != "ok") {
        return jp;
    }

    int payment = o["mode"].toString().toInt();
    v[":id"] = order;
    v[":state_id"] = ORDER_STATE_CLOSED;
    v[":payment"] = payment;
    v[":taxprint"] = -1;
    fDb.select("update o_order set state_id=:state_id, payment=:payment, taxprint=:taxprint where id=:id", v, dr);
    v[":order_id"] = order;
    fDb.select("update h_table set order_id='', lock_host=null where order_id=:order_id", v, dr);
    QJsonObject jReply;
    jReply["reply"] = "ok";
    jReply["orderclosed"] = "ok";
    return jReply;
}

QJsonObject MJsonHandler::handleDishComment(const QJsonObject &o)
{
    int recid = o["dish"].toString().toInt();

    if(recid == 0) {
        return jsonError(tr("Incorrect dish record id"));
    }

    DatabaseResult dr;
    QMap<QString, QVariant> v;
    QJsonObject jReply;

    if(o.contains("set")) {
        v[":comments"] = o["set"].toString();
        fDb.update("o_dishes", v, recid);
    } else {
        v[":id"] = recid;
        fDb.select("select comments, printed_qty as qty from o_dishes where id=:id", v, dr);

        if(dr.rowCount() == 0) {
            return jsonError(tr("Incorrect dish record id"));
        }

        jReply["comment"] = dr.value("COMMENTS").toString();

        if(dr.value("QTY").toDouble() > 0.1) {
            jReply["readonly"] = "true";
        }
    }

    jReply["reply"] = "ok";
    return jReply;
}

QJsonObject MJsonHandler::handleCommentsList(const QJsonObject &o)
{
    Q_UNUSED(o)
    DatabaseResult dr;
    QMap<QString, QVariant> v;
    QJsonObject jReply;
    QJsonArray ja;
    fDb.select("select name from me_dishes_comment order by queue", v, dr);

    for(int i = 0; i < dr.rowCount(); i++) {
        QJsonObject obj;
        obj["name"] = dr.value(i, "NAME").toString();
        ja.append(obj);
    }

    jReply["reply"] = "ok";
    jReply["commentslist"] = ja;
    return jReply;
}

QJsonObject MJsonHandler::handleReadyDishes(const QJsonObject &o)
{
    Q_UNUSED(o)
    DatabaseResult dr;
    QMap<QString, QVariant> v;
    QJsonObject jReply;
    QJsonArray ja;
    fDb.select("select dr.id, t.name as tablename, e.fname || ' ' || e.lname as staff, m.name, dr.qty "
               "from o_dishes_reminder dr, h_table t, employes e, me_dishes m "
               "where dr.table_id=t.id and dr.staff_id=e.id and dr.dish_id=m.id "
               "and dr.date_ready is not null and dr.date_done is null "
               "order by dr.date_ready", v, dr);

    for(int i = 0; i < dr.rowCount(); i++) {
        QJsonObject obj;
        obj["id"] = dr.value(i, "ID").toString();
        obj["dish"] = dr.value(i, "NAME").toString();
        obj["table"] = dr.value(i, "TABLENAME").toString();
        obj["staff"] = dr.value(i, "STAFF").toString();
        obj["qty"] = float_str(dr.value(i, "QTY").toDouble(), 1);
        ja.append(obj);
    }

    jReply["list"] = ja;
    jReply["reply"] = "ok";
    return jReply;
}

QJsonObject MJsonHandler::handleReadyDishDone(const QJsonObject &o)
{
    if(!o.contains("id")) {
        return jsonError("Invalid dish record id");
    }

    QMap<QString, QVariant> v;
    QJsonObject jReply;
    v[":date_done"] = QDateTime::currentDateTime();
    fDb.update("o_dishes_reminder", v, o["id"].toString().toInt());
    jReply["reply"] = "ok";
    jReply["readydishdone"] = "";
    jReply["id"] = o["id"].toString();
    return jReply;
}

QJsonObject MJsonHandler::handleMyMoney(const QJsonObject &o)
{
    QMap<QString, QVariant> v;
    QJsonObject jReply;
    v[":date_cash"] =  "'" + QDate::currentDate().toString("dd.MM.yyyy") + "'";
    v[":state_id"] = ORDER_STATE_CLOSED;
    v[":staff_id"] = o["session"];
    DatabaseResult dr;
    fDb.select("select count(id) as QNT, coalesce(sum(amount), 0) as AMOUNT from o_order where date_cash=:date_cash and state_id=:state_id and staff_id=:staff_id", v, dr);

    if(dr.rowCount()) {
        jReply["reply"] = "ok";
        jReply["qty"] = dr.value("QNT").toString();
        jReply["mymoney"] = dr.value("AMOUNT").toString();
        jReply["mymoney"] = dr.value("QNT").toString() + " / " + dr.value("AMOUNT").toString();
    }

    return jReply;
}

QJsonObject MJsonHandler::handlePrintFiscal(const QJsonObject &o)
{
    return QJsonObject{{"reply", "ok"}};
}

QJsonObject MJsonHandler::handleTaxCancel(const QJsonObject &o)
{
    QJsonObject jReply;
    QMap<QString, QVariant> bv;
    bv[":fid"] = o["order"].toString();
    DatabaseResult dr;
    fDb.select("select json from o_tax where fid=:fid", bv, dr, false);

    if(dr.rowCount() == 0) {
        jReply["reply"] = "Invalid order id";
        return jReply;
    }

    if(dr.value("jsonin").toString() == "Comp.") {
        jReply["reply"] = tr("This is a complimentary");
        return jReply;
    }

    QJsonObject tjson = QJsonDocument::fromJson(dr.toString(0, "JSON").toUtf8()).object();
    QString crn, in, out, err;
    int rseq = tjson["rseq"].toInt();
    crn = tjson["crn"].toString();
    MTFileLog::createLog(__LOG_PRINT, QString("%1,%2, %3").arg(rseq).arg(crn, dr.toString(0, "JSON")));
    PrintTaxN pt(__cnfapp.taxIP(), __cnfapp.taxPort(), __cnfapp.taxPassword(), "true", "3", "3", this);
    DatabaseResult em;
    bv[":order_id"] = o["order"].toString();
    fDb.select("select emarks from o_dishes where order_id=:order_id and CHAR_LENGTH(emarks)>0", bv, em);

    for(int i = 0; i < em.rowCount(); i++) {
        pt.fEmarks.append(em.value("emarks").toString());
    }

    int result = pt.printTaxback(rseq, crn, in, out, err);
    jReply["reply"] = result == 0 ? "ok" : QString("Error code: %1, description: %2").arg(QString::number(result), err);
    MTFileLog::createLog(__LOG_PRINT, QString("Tax error: %1/%2").arg(QString::number(result), err));
    QMap<QString, QVariant> v;
    v[":fdate"] = QDateTime::currentDateTime();
    v[":forder"] = o["order"].toString();
    v[":fin"] = in;
    v[":fout"] = out;
    v[":ferr"] = err;
    v[":fresult"] = result;
    fDb.open();
    fDb.insert("o_tax_log", v);
    fDb.close();

    if(result == pt_err_ok) {
        v.clear();
        v[":fid"] = o["order"].toString();
        fDb.select("delete from o_tax where fid=:fid", v, dr, false);
    }

    return jReply;
}

QString MJsonHandler::updateOrderAmount(const QString &id)
{
    QMap<QString, QVariant> v;
    v[":id"] = id;
    v[":state_id"] = DISH_STATE_NORMAL;
    DatabaseResult dr;
    fDb.select("select sum(qty*price) as total, sum(qty*price*price_inc) as totalinc, "
               "sum(((qty*price) + (qty*price*price_inc)) * price_dec) as totaldec "
               "from o_dishes where order_id=:id and state_id=:state_id", v, dr, false);
    double total = 0;

    if(dr.rowCount() > 0) {
        total = dr.value("TOTAL").toDouble() + dr.value("TOTALINC").toDouble() - dr.value("TOTALDEC").toDouble();
        v[":amount"] = total;
        v[":amount_inc"] = dr.value("TOTALINC");
        v[":amount_dec"] = dr.value("TOTALDEC");
        fDb.update("o_order", v, id);
    }

    return float_str(total, 0);
}

int MJsonHandler::checkPassword(const QString &pwd)
{
    QString pwdCrypt = QString(QCryptographicHash::hash(pwd.toLatin1(), QCryptographicHash::Md5).toHex());
    QMap<QString, QVariant> v;
    v[":password2"] = pwdCrypt;
    QString sql = "select id, group_id, fname || ' ' || lname as fullname, group_id "
                  "from employes where password2=:password2 and state_id=1";
    DatabaseResult dr;
    fDb.select(sql, v, dr);

    if(dr.rowCount() == 0) {
        return 0;
    }

    return dr.value("ID").toInt();
}

QString MJsonHandler::sessionNew()
{
    return QUuid::createUuid().toString();
}

QJsonObject MJsonHandler::jsonError(const QString &err)
{
    QJsonObject o;
    o["reply"] = err;
    return o;
}
