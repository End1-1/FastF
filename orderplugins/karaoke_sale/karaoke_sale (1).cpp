#include "../../dbdriver.h"
#include "../../ff_settingsdrv.h"
#include "karaoke_sale.h"
#include "../../dlgreportfilter.h"
#include <QDialog>
#include "../../orderdrv/od_drv.h"


QString caption() {
    return QString::fromUtf8("Կարաոկեի վաճառք");
}

bool filterDate() {
    return true;
}

bool exec(const QMap<QString, QString> &filter, OD_Drv *o, QString &msg) {

    QSqlDatabase db = QSqlDatabase::addDatabase("QIBASE");
    o->configureOtherDB(db);
    db.open();
    QSqlQuery ql(db);
    ql.prepare("select id from o_order where table_id=112 and date_close between :date1 and :date2");
    ql.bindValue(":date1", QDate::fromString(filter["date1"], DATE_FORMAT));
    ql.bindValue(":date2", QDate::fromString(filter["date2"], DATE_FORMAT));
    ql.exec();
    if (ql.next()) {
        db.close();
        msg = QString::fromUtf8("Նշված ժամանակահատվածում առկա է Կարաոկեի պատվերներ");
        return false;
    }

    QList<int> dishes;
    ql.exec("select id from me_dishes");
    while (ql.next())
        dishes.append(ql.value(0).toInt());
    db.close();

    db = QSqlDatabase::addDatabase("QIBASE", "d");
    db.setDatabaseName("10.0.10.2:db");
    db.setUserName("SYSDBA");
    db.setPassword("masterkey");
    db.setConnectOptions("lc_ctype=UTF-8");
    if (!db.open()) {
        msg = db.lastError().databaseText();
        return false;
    }

    QSqlQuery q(db);
    if (!q.prepare("select "
            "me.rdish, "
            "me.name as dish_name, "
            "od.qty, "
            "od.printed_qty, "
            "me.rprice as price "
            "from o_order o, o_dishes od, me_dishes me "
            "where o.id=od.order_id and od.dish_id=me.id "
            "and o.state_id=2 and od.state_id=1 "
            "and o.date_cash between :date1 and :date2 and od.store_id=2")) {
        msg = q.lastError().databaseText();
        return false;
    }

    q.bindValue(":date1", QDate::fromString(filter["date1"], DATE_FORMAT));
    q.bindValue(":date2", QDate::fromString(filter["date2"], DATE_FORMAT));
    if (!q.exec()) {
        msg = q.lastError().databaseText();
        return false;
    }
    QMap<int, QString> unknownDishes;
    QMap<int, float> uniqueDishes;
    o->m_header.f_amount_dec_value = 0.3;
    o->m_header.f_amount_inc_value = 0;
    while (q.next()) {
        int i = 0;
        OD_Dish *dish = new OD_Dish();
        dish->f_id = 0;
        dish->f_stateId = DISH_STATE_NORMAL;
        dish->f_dishId = q.value(i++).toInt();
        dish->f_dishName = q.value(i++).toString();
        dish->f_totalQty = q.value(i++).toFloat();
        dish->f_printedQty = q.value(i++).toFloat();
        dish->f_price = q.value(i++).toFloat();
        dish->f_storeId = 3;
        dish->f_print1 = "";
        dish->f_print2 = "";
        dish->f_lastUser = o->m_header.f_currStaffId;
        dish->f_lastUserName = o->m_header.f_currStaffName;
        dish->f_paymentMod = 1;
        dish->f_priceDec = 0.3;
        dish->f_priceInc = 0;
        dish->f_comments = "";
        dish->f_remind = 0;

        uniqueDishes[dish->f_dishId] = 0;

        if (!dishes.contains(dish->f_dishId)) {
            unknownDishes[dish->f_dishId] = dish->f_dishName;
            continue;
        }
        o->appendDish(dish);
    }

    bool f = true;
    QString sql;
    for (QMap<int, float>::const_iterator it = uniqueDishes.begin(); it != uniqueDishes.end(); it++) {
        if (f)
            f = false;
        else
            sql += ",";
        sql += QString::number(it.key());
    }
    sql = "select dish_id, price from me_dishes_menu where menu_id=1 and dish_id in (" + sql + ")";
    db.close();

    o->countAmounts();

    if (unknownDishes.count()) {
        msg = QString::fromUtf8("Անհայտ ուտեստներ");
        for (QMap<int, QString>::const_iterator i = unknownDishes.begin(); i != unknownDishes.end(); i++)
            msg += QString::fromUtf8("Անհայտ ուտեստ") + ": " + its(i.key()) + "/" + i.value() + "\r\n";

    }

    return true;
}
