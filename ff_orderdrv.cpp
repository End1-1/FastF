#include "ff_orderdrv.h"
#include <math.h>
#include "ff_settingsdrv.h"
#include <QFile>
#include <QProcess>

void FF_OrderDrv::countOrder()
{
    m_qtyEqualPrintQty = true;
    float totalInc = 0, totalNoInc = 0;

    for (QList<OrderDish>::const_iterator it = m_dishes.begin(); it != m_dishes.end(); it++)
        if (it->state_id == DISH_STATE_NORMAL) {
            if (it->qty != it->pqty)
                m_qtyEqualPrintQty = false;
            if (it->payment_mod == DISH_MOD_NORMAL)
                totalInc += it->qty * it->price;
            else
                totalNoInc += it->qty * it->price;
        }

    m_header.amount = totalInc;
    if (m_header.amount_inc_value > 0) {
        m_header.amount_inc = totalInc * m_header.amount_inc_value;
        totalInc += (totalInc * m_header.amount_inc_value);

    }
    if (m_header.amount_dec_value > 0) {
        m_header.amount_dec = totalInc * m_header.amount_dec_value;
        totalInc -= (totalInc * m_header.amount_dec_value);
    }
    m_header.amount = totalInc + totalNoInc;

    emit dishQtyEqualPrintQty(m_qtyEqualPrintQty);
    emit counted();
}

bool FF_OrderDrv::moveOrder(int dstTableId, FF_User *user, FF_HallDrv *hallDrv)
{
    m_db.transaction();
    m_sql = "select order_id from h_table where id=:id";
    QString logSrc;
    QString logDst;
    int dishesCount = m_dishes.count();
    if (!prepare())
        return false;
    m_query->bindValue(":id", dstTableId);
    if (!execSQL())
        return false;
    m_query->next();
    QString dstOrder = m_query->value(0).toString();
    if (!dstOrder.length()) {
        //Destinatin table empty
        logSrc = QString("%1 [%2] >> %3 [0]").arg(m_header.table_name)
                                .arg(m_header.id)
                                .arg(hallDrv->table(dstTableId)->name);
    } else {
        //Merge with destination table
        logSrc = QString("%1 [%2] >> %3 [%4]").arg(m_header.table_name)
                                .arg(m_header.id)
                                .arg(hallDrv->table(dstTableId)->name)
                                .arg(dstOrder);
        logDst = QString("%1 [%2] >> %3 [%4]").arg(m_header.table_name)
                .arg(m_header.id)
                .arg(hallDrv->table(dstTableId)->name)
                .arg(dstOrder);

        //Delete discount history
        m_sql = "delete from costumers_history where order_id=:order_id";
        if (!prepare())
            return false;
        m_query->bindValue(":order_id", dstOrder);
        if (!execSQL())
            return false;

        //Get dishes from destination tables
        m_sql = "select me.name as dish_name, dish_id, qty, printed_qty, price, last_user, store_id, od.payment_mod, "
                "print_schema, od.comments "
              "from o_dishes od, me_dishes me where dish_id=me.id and order_id=:order_id and state_id=:state_id";
        if (!prepare())
            return false;
        m_query->bindValue(":order_id", dstOrder);
        m_query->bindValue(":state_id", DISH_STATE_NORMAL);
        if (!execSQL())
            return false;

        while (m_query->next()) {
            OrderDish dish;
            dish.id = genId("GEN_O_DISH_ID");
            dish.state_id = DISH_STATE_NORMAL;
            dish.dish_id = valInt("DISH_ID");
            dish.dish_name = valStr("DISH_NAME");
            dish.qty = valFloat("QTY");
            dish.pqty = valFloat("PRINTED_QTY");
            dish.price = valFloat("PRICE");
            dish.store_id = valInt("STORE_ID");
            dish.print_schema = valInt("PRINT_SCHEMA");
            dish.payment_mod = valInt("PAYMENT_MOD");
            dish.last_user = valInt("LAST_USER");
            dish.comment = valStr("COMMENTS");
            dish.rqty = 0;
            dish.saved = 0;
            dish.remind = valInt("REMIND") == 1;
            appendDish(dish);
        }

        m_sql = "update o_order set state_id=:state_id where id=:id";
        if (!prepare())
            return false;
        m_query->bindValue(":state_id", ORDER_STATE_MERGE);
        m_query->bindValue(":id", dstOrder);
        if (!execSQL()) {
            removeDishesFrom(dishesCount);
            return false;
        }

        m_sql = "update o_dishes set state_id=:new_state_id where order_id=:order_id and state_id=:old_state_id";
        if (!prepare()) {
            removeDishesFrom(dishesCount);
            return false;
        }
        m_query->bindValue(":new_state_id", DISH_STATE_MOVED);
        m_query->bindValue(":order_id", dstOrder);
        m_query->bindValue(":old_state_id", DISH_STATE_NORMAL);
        if (!execSQL()) {
            removeDishesFrom(dishesCount);
            return false;
        }
    }

    //Change order id of tables
    m_sql = "update h_table set order_id=:order_id where id=:id";
    if (!prepare()){
        removeDishesFrom(dishesCount);
        return false;
    }
    m_query->bindValue(":order_id", m_header.id);
    m_query->bindValue(":id", dstTableId);
    if (!execSQL()){
        removeDishesFrom(dishesCount);
        return false;
    }

    m_sql = "update h_table set order_id='' where id=:id";
    if (!prepare()){
        removeDishesFrom(dishesCount);
        return false;
    }
    m_query->bindValue(":id", m_header.table_id);
    if (!execSQL()){
        removeDishesFrom(dishesCount);
        return false;
    }

    //Change table id of current order
    m_sql = "update o_order set table_id=:table_id where id=:id";
    if (!prepare()){
        removeDishesFrom(dishesCount);
        return false;
    }
    m_query->bindValue(":table_id", dstTableId);
    m_query->bindValue(":id", m_header.id);
    if (!execSQL()){
        removeDishesFrom(dishesCount);
        return false;
    }
    int srcTableId = m_header.table_id;
    m_header.table_id = dstTableId;
    m_header.table_name = hallDrv->table(dstTableId)->name;
    m_db.commit();

    countOrder();
    hallDrv->unlockTable(srcTableId);

    QSqlLog::write(TABLE_HISTORY, tr("Order moved"), logSrc, user->fullName, 0, m_header.id);

    if (logDst.length())
        QSqlLog::write(TABLE_HISTORY, tr("Order merged"), logDst, user->fullName, 0, dstOrder);

    return true;
}

bool FF_OrderDrv::insertDish(OrderDish &dish)
{
    m_sql = "insert into o_dishes (id, order_id, state_id, dish_id, qty, printed_qty, "
            "price, last_user, store_id, payment_mod, print_schema, comments, remind, "
            "price_inc, price_dec) values "
            "(:id, :order_id, :state_id, :dish_id, :qty, :printed_qty, "
            ":price, :last_user, :store_id, :payment_mod, :print_schema, :comments, :remind, "
            ":price_inc, :price_dec)";
    if (!prepare())
        return false;
    m_query->bindValue(":id", dish.id);
    m_query->bindValue(":order_id",  m_header.id);
    m_query->bindValue(":state_id", dish.state_id);
    m_query->bindValue(":dish_id", dish.dish_id);
    m_query->bindValue(":qty", dish.qty);
    m_query->bindValue(":printed_qty", dish.pqty);
    m_query->bindValue(":price", dish.price);
    m_query->bindValue(":last_user", dish.last_user);
    m_query->bindValue(":store_id", dish.store_id);
    m_query->bindValue(":payment_mod", dish.payment_mod);
    m_query->bindValue(":print_schema", dish.print_schema);
    m_query->bindValue(":comments", dish.comment);
    m_query->bindValue(":remind", dish.remind);
    m_query->bindValue(":price_inc", dish.price_inc);
    m_query->bindValue(":price_dec", dish.price_dec);
    if (!execSQL())
        return false;

    QSqlLog::write(TABLE_HISTORY, tr("Append dish"), QString("ID: %1, Name: %2, Qty: 1, Price: %3, State: %4")
                   .arg(dish.id)
                   .arg(dish.dish_name)
                   .arg(dish.price)
                   .arg(dish.state_id),
                   m_username, 0, m_header.id);

    return true;
}

FF_OrderDrv::FF_OrderDrv(FF_HallDrv *hallDrv, const QString &username) :
    QSqlDrv(username, "main")
{
    m_hallDrv = hallDrv;
}

int FF_OrderDrv::readTable(int tableId, FF_User *user)
{
    int result = ORDER_READ_NEW;
    if (!openDB())
        return ORDER_READ_FAILED;

    m_db.transaction();

    m_sql = QString("select order_id, lock_host from h_table where id=%1").arg(tableId);
    if (!execSQL(m_sql))
        return ORDER_READ_FAILED;

    if (!m_query->next()) {
        return ORDER_READ_FAILED;
    }
    m_header.id = m_query->value(0).toString();
    QString lockBy = m_query->value(1).toString();
    if (lockBy.length())
        if (lockBy != getLockName()) {
            return ORDER_READ_FAILED;
        }

    m_sql = QString("update h_table set lock_host='%1' where id='%2'")
            .arg(getLockName()).arg(tableId);
    if (!execSQL(m_sql))
        return ORDER_READ_FAILED;

    if (!m_header.id.length()) {
        m_header.id = QString("%1-%2").arg(FF_SettingsDrv::value(SD_ORDER_ID_PREFIX).toString())
                .arg(genId("GEN_O_ORDER_ID"));
        m_header.state_id = ORDER_STATE_OPEN;
        m_header.table_id = tableId;
        m_header.table_name = m_hallDrv->table(tableId)->name;
        m_header.staff_id = user->id;
        m_header.amount = 0;
        m_header.amount_inc = 0;
        m_header.amount_inc_value = 0;
        m_header.amount_dec = 0;
        m_header.amount_dec_value = 0;
        m_header.date_open = QDateTime::currentDateTime();
        m_header.date_close = QDateTime::currentDateTime();
        m_header.date_cash = QDate::currentDate();
        m_header.printed = 0;
        m_header.paymentType = 1;
        m_header.printed_tax = 0;

        m_sql = QString("update h_table set order_id='%1' where id=%2")
                .arg(m_header.id)
                .arg(tableId);
        if (!execSQL(m_sql))
            return ORDER_READ_FAILED;
        m_sql = "insert into o_order (id, state_id, table_id, date_open, date_close, date_cash, "
                "staff_id, amount, print_qty, amount_inc, amount_dec, amount_inc_value, amount_dec_value, "
                "payment, taxprint) "
                "values (:id, :state_id, :table_id, current_timestamp, current_timestamp, current_date, "
              ":staff_id, 0, 0, 0, 0, 0, 0, 0, 0)";
        if (!prepare())
            return ORDER_READ_FAILED;
        m_query->bindValue(":id", m_header.id);
        m_query->bindValue(":state_id", ORDER_STATE_OPEN);
        m_query->bindValue(":table_id", tableId);
        m_query->bindValue(":staff_id", user->id);
        if (!execSQL())
            return ORDER_READ_FAILED;
        close();

        QSqlLog::write(TABLE_HISTORY, tr("New order"), QString("%1: %2").arg(tr("Table"))
                       .arg(m_header.table_name), user->fullName, 0, m_header.id);


    } else {
        result = ORDER_READ_EXISTS;
        readOrder(m_header.id);
        QSqlLog::write(TABLE_HISTORY, tr("Open order"), QString("%1: %2").arg(tr("Table")).arg(m_header.table_name), user->fullName, 0, m_header.id);
    }

    m_header.current_staff_id = user->id;
    m_header.current_staff_name = user->fullName;

    return result;
}

bool FF_OrderDrv::readOrder(QString id)
{
    if (!openDB())
        return false;

    m_db.transaction();
    m_sql = "select "
            "od.id as rec_id, "
            "od.state_id, "
            "od.dish_id, "
            "me.name as dish_name, "
            "od.qty, "
            "od.printed_qty, "
            "od.price, "
            "od.store_id, "
            "od.print_schema, "
            "od.last_user, "
            "me.payment_mod, "
            "od.comments, "
            "od.remind, "
            "od.price_inc, "
            "od.price_dec "
            "from o_dishes od, me_dishes me "
            "where od.order_id='" + id + "' and od.dish_id=me.id ";

    if (!execSQL(m_sql))
        return false;
    while (next()) {
        OrderDish d;
        d.id = val().toInt();
        d.state_id = val().toInt();
        d.dish_id = val().toInt();
        d.dish_name = val().toString();
        d.qty = val().toFloat();
        d.pqty = val().toFloat();
        d.price = val().toFloat();
        d.store_id = val().toInt();
        d.print_schema = val().toInt();
        d.last_user = val().toInt();
        d.payment_mod = val().toInt();
        d.comment = val().toString();
        d.remind = val().toInt() == 1;
        d.rqty = 0;
        d.saved = 1;
        d.price_inc = val().toFloat();
        d.price_dec = val().toFloat();
        m_dishes.append(d);
    }

    m_sql = QString("select "
                    "oo.id, "
                    "oo.date_open, "
                    "oo.date_close, "
                    "oo.date_cash, "
                    "oo.state_id, "
                    "oo.staff_id, "
                    "e.fname || ' ' || e.lname as staff_name, "
                    "oo.amount, "
                    "oo.print_qty, "
                    "oo.amount_inc, "
                    "oo.amount_inc_value, "
                    "oo.amount_dec, "
                    "oo.amount_dec_value, "
                    "oo.table_id, "
                    "t.name as table_name, "
                    "oo.payment, "
                    "oo.taxprint, "
                    "oo.comment "
                    "from o_order oo, h_table t,  employes e "
                    "where oo.staff_id=e.id and oo.table_id=t.id and oo.id='%1'")
                    .arg(id);

    if (!execSQL(m_sql))
        return false;
    m_query->next();
    m_header.id = val().toString();
    m_header.date_open = val().toDateTime();
    m_header.date_close = val().toDateTime();
    m_header.date_cash = val().toDate();
    m_header.state_id = val().toInt();
    m_header.staff_id = val().toInt();
    m_header.staff_name = val().toString();
    m_header.amount = val().toFloat();
    m_header.printed = val().toInt();
    m_header.amount_inc = val().toFloat();
    m_header.amount_inc_value = val().toFloat();
    m_header.amount_dec = val().toFloat();
    m_header.amount_dec_value = val().toFloat();
    m_header.table_id = val().toInt();
    m_header.table_name = val().toString();
    m_header.paymentType = val().toInt();
    m_header.printed_tax = val().toInt();
    m_header.current_staff_id = 0;
    m_header.comment = val().toString();
    close();

    return true;
}

void FF_OrderDrv::freeTable()
{
    if (!m_header.id.length())
        return;

    if (!prepare("update h_table set lock_host = null, flags=:flags where id=:id"))
        return;
    bind(":flags", TFLAG_DEFAULT);
    bind(":id", m_header.table_id);
    if (!execSQL())
        return;
}

int FF_OrderDrv::dishCount(int state)
{
    int count = 0;
    for (QList<OrderDish>::const_iterator i = m_dishes.begin(); i != m_dishes.end(); i++)
        if (i->state_id == state)
            count++;
    return count;
}

bool FF_OrderDrv::appendDish(OrderDish dish)
{
    dish.id = genId("GEN_O_DISH_ID");
    if (!dish.id)
        return false;
    m_dishes.append(dish);
    if (insertDish(dish))
        countOrder();
    else
        return false;
    return true;
}

int FF_OrderDrv::duplicateDish(int dishIndex)
{
    m_dishes.append(m_dishes.at(dishIndex));
    int i = m_dishes.count() - 1;
    m_dishes[i].id = genId("GEN_O_DISH_ID");
    insertDish(m_dishes[i]);
    return i;
}

void FF_OrderDrv::increaseDishQty(int dishIndex, float qty)
{
    if (m_dishes[dishIndex].pqty > 0)
        m_dishes[dishIndex].qty += qty;
    else {
        switch((int)trunc(qty * 10)) {
        case 5:
            m_dishes[dishIndex].qty += qty;
            break;
        case 100:
            if (trunc(m_dishes[dishIndex].qty * 100) == 100)
                m_dishes[dishIndex].qty = qty;
            else
                m_dishes[dishIndex].qty += qty;
            break;
        default:
            m_dishes[dishIndex].qty += qty;
            break;
        }
    }
    emit dishRepaint(dishIndex);
    countOrder();

    QSqlLog::write(TABLE_HISTORY, tr("Change qty of dish"), QString("ID: %1, Name: %2, Qty: %3")
                   .arg(m_dishes[dishIndex].id)
                   .arg(m_dishes[dishIndex].dish_name)
                   .arg(m_dishes[dishIndex].qty), m_username, 0, m_header.id);


}

float FF_OrderDrv::decreaseDishQty(int dishIndex, float rmQty)
{
    float result = 0;
    float qty = m_dishes[dishIndex].qty;
    float printedQty = m_dishes[dishIndex].pqty;
    qty -= rmQty;
    if (qty < printedQty) {
        result = printedQty - qty;
        printedQty = qty;
        m_dishes[dishIndex].pqty = qty;
    }

    QString reason;
    m_dishes[dishIndex].qty = qty;
    if (qty <= 0) {
        if (result > 0) {
            reason = tr("Decrease qty of printed dish");
            m_dishes[dishIndex].state_id = DISH_STATE_REMOVED_PRINTED;
        }
        else {
            reason = tr("Decrease qty of dish");
            m_dishes[dishIndex].state_id = DISH_STATE_REMOVED_NORMAL;
        }
    }
    m_dishes[dishIndex].saved = false;

    QSqlLog::write(TABLE_HISTORY, reason, QString("ID: %1, Name: %2, Qty: %3")
                   .arg(m_dishes[dishIndex].id)
                   .arg(m_dishes[dishIndex].dish_name)
                   .arg(qty), m_username, 0, m_header.id);

    if ((result > 0) && (qty > 0)) {
        int removedDish = duplicateDish(dishIndex);
        m_dishes[removedDish].qty = result;
        m_dishes[removedDish].pqty = result;
        m_dishes[removedDish].state_id = DISH_STATE_REMOVED_PRINTED;
        m_dishes[removedDish].saved = false;
    }

    countOrder();

    return result;
}

void FF_OrderDrv::changeDishQty(int dishIndex, float qty)
{
    float dishQty = m_dishes[dishIndex].qty;
    float dishPrinted = m_dishes[dishIndex].pqty;

    if (qty < dishPrinted)
            return;
    dishQty = qty;

    m_dishes[dishIndex].qty = dishQty;
    m_dishes[dishIndex].saved = false;

    QSqlLog::write(TABLE_HISTORY, tr("Change qty of dish"), QString("ID: %1, Name: %2, Qty: %3")
                   .arg(m_dishes[dishIndex].id)
                   .arg(m_dishes[dishIndex].dish_name)
                   .arg(dishQty), m_username, 0, m_header.id);

    emit dishRepaint(dishIndex);
    countOrder();
}

void FF_OrderDrv::getDishesForServicePrint(QMap<int, float> &list)
{
    for (int i = 0; i < m_dishes.count(); i++) {
        if (m_dishes[i].state_id != DISH_STATE_NORMAL)
            continue;
        float qty = m_dishes[i].qty - m_dishes[i].pqty;
        if (qty > 0) {
            list[i] = qty;
            m_dishes[i].pqty = m_dishes[i].qty;
            m_dishes[i].saved = false;
            emit dishRepaint(i);
        }
    }
}

bool FF_OrderDrv::saveOrder(bool closeTransaction)
{
    openDB();
    m_db.transaction();

    if (!prepare("update o_dishes set state_id=:state_id, qty=:qty, printed_qty=:printed_qty, comments=:comments where id=:id"))
        return false;

    for (int i = 0; i < m_dishes.count(); i++) {
        OrderDish &d = m_dishes[i];
        if (!d.saved) {
            m_query->bindValue(":state_id", d.state_id);
            m_query->bindValue(":qty", d.qty);
            m_query->bindValue(":printed_qty", d.pqty);
            m_query->bindValue(":id", d.id);
            m_query->bindValue(":comments", d.comment);
            if (!execSQL())
                return false;
            d.saved = true;
        }
    }

    if (!prepare("update o_order set table_id=:table_id, staff_id=:staff_id, print_qty=:print_qty, "
                 "amount=:amount, amount_inc=:amount_inc, amount_inc_value=:amount_inc_value, "
                 "amount_dec=:amount_dec, amount_dec_value=:amount_dec_value,  "
                 "payment=:payment, taxprint=:taxprint, comment=:comment where id=:id"))
        return false;
    m_query->bindValue(":table_id", m_header.table_id);
    m_query->bindValue(":staff_id", m_header.staff_id);
    m_query->bindValue(":print_qty", m_header.printed);
    m_query->bindValue(":amount", m_header.amount);
    m_query->bindValue(":amount_inc", m_header.amount_inc);
    m_query->bindValue(":amount_inc_value", m_header.amount_inc_value);
    m_query->bindValue(":amount_dec", m_header.amount_dec);
    m_query->bindValue(":amount_dec_value", m_header.amount_dec_value);
    m_query->bindValue(":payment", m_header.paymentType);
    m_query->bindValue(":taxprint", m_header.printed_tax);
    m_query->bindValue(":comment", m_header.comment);
    m_query->bindValue(":id", m_header.id);
    if (!execSQL())
        return false;

    if (closeTransaction)
        m_db.commit();

    return true;
}

bool FF_OrderDrv::closeOrder(int state, const QDate &cashDate)
{
    saveOrder(false);

    m_sql = "update h_table set order_id='', flags=:flags, lock_host=null where id=:id";
    if (!prepare())
        return false;
    m_query->bindValue(":flags", TFLAG_DEFAULT);
    m_query->bindValue(":id", m_header.table_id);
    if (!execSQL())
        return false;

    m_sql = "update o_order set state_id=:state_id, date_close=current_timestamp, date_cash=:date_cash where id=:id";
    if (!prepare())
        return false;
    m_query->bindValue(":state_id", state);
    m_query->bindValue(":date_cash", cashDate);
    m_query->bindValue(":id", m_header.id);
    if (!execSQL())
        return false;

    if (state != ORDER_STATE_CLOSED) {
        m_sql = "delete from costumers_history where order_id=:order_id";
        if (!prepare())
            return false;
        m_query->bindValue(":order_id", m_header.id);
        if (!execSQL())
            return false;
    }

    m_db.commit();

    return true;
}

void FF_OrderDrv::clearOrder()
{
    m_dishes.clear();
    m_header.id = "";
}

void FF_OrderDrv::removeDishesFrom(int index)
{
    for (int i = m_dishes.count() - 1; i > index - 1; i--)
        m_dishes.removeAt(i);
}

void FF_OrderDrv::incMod(float value)
{
    m_header.amount_inc_value = value;
    countOrder();
}

void FF_OrderDrv::decMod(int costumerId, float value)
{
    m_header.amount_dec_value = value;
    for (QList<OrderDish>::iterator i = m_dishes.begin(); i != m_dishes.end(); i++)
        i->price_dec = value;
    countOrder();

    if (!prepare("insert into costumers_history values (:order_id, :costumer_id, :val)"))
        return;
    m_query->bindValue(":order_id", m_header.id);
    m_query->bindValue(":costumer_id", costumerId);
    m_query->bindValue(":val", value);
    if (!execSQL())
        return;
}

bool FF_OrderDrv::moveDish(int dstTableId, FF_User *user, int dishIndex, float qty)
{
    FF_OrderDrv *o = new FF_OrderDrv(m_hallDrv, user->fullName);
    /*
    if (o->readTable(dstTableId, user) == ORDER_READ_NEW) {
        FF_SettingsDrv::orderAutoMod(o);
        o->saveOrder();
    }
    */
    o->readOrder(o->m_header.id);
    o->appendDish(m_dishes.at(dishIndex));
    int dstDishIndex = o->m_dishes.count() - 1;
    o->m_dishes[dstDishIndex].qty = qty;
    o->m_dishes[dstDishIndex].pqty = qty;
    o->m_dishes[dstDishIndex].saved = 0;
    o->countOrder();
    o->saveOrder();

    QSqlLog::write(TABLE_HISTORY, tr("Move dish"), QString("Source table: %1, ID: %2, Name: %3, Qty: %4, Price: %5, State: %6")
                   .arg(o->m_header.table_name)
                   .arg(o->m_dishes[dstDishIndex].id)
                   .arg(o->m_dishes[dstDishIndex].dish_name)
                   .arg(qty)
                   .arg(o->m_dishes[dstDishIndex].price)
                   .arg(o->m_dishes[dstDishIndex].state_id), m_username, 0, m_header.id);

    if (m_dishes[dishIndex].qty - qty == 0) {
        m_dishes[dishIndex].state_id = DISH_STATE_MOVED;
        m_dishes[dishIndex].saved = false;

        QSqlLog::write(TABLE_HISTORY, tr("Move dish"), QString("Destination table: %1, ID: %2, Name: %3, Qty: %4, Price: %5, State: %6")
                       .arg(o->m_header.table_name)
                       .arg(m_dishes[dishIndex].id)
                       .arg(m_dishes[dishIndex].dish_name)
                       .arg(qty)
                       .arg(m_dishes[dishIndex].price)
                       .arg(m_dishes[dishIndex].state_id), m_username, 0, m_header.id);
    } else {
        m_dishes[dishIndex].qty = m_dishes[dishIndex].qty - qty;
        m_dishes[dishIndex].pqty =  m_dishes[dishIndex].qty;
        m_dishes[dishIndex].saved = false;
        appendDish(m_dishes.at(dishIndex));
        dishIndex = m_dishes.count() - 1;
        m_dishes[dishIndex].state_id = DISH_STATE_MOVE_PARTIALY;
        m_dishes[dishIndex].qty = qty;
        m_dishes[dishIndex].pqty = qty;
        m_dishes[dishIndex].saved = false;
        QSqlLog::write(TABLE_HISTORY, tr("Move dish partialy"), QString("Destination table: %1, ID: %2, Name: %3, Qty: %4, Price: %5, State: %6")
                       .arg(o->m_header.table_name)
                       .arg(m_dishes[dishIndex].id)
                       .arg(m_dishes[dishIndex].dish_name)
                       .arg(qty)
                       .arg(m_dishes[dishIndex].price)
                       .arg(m_dishes[dishIndex].state_id), m_username, 0, m_header.id);
    }
    delete o;
    countOrder();
    m_hallDrv->unlockTable(dstTableId);
    saveOrder();

    return true;
}

void FF_OrderDrv::makeTaxPrintCheck(bool print)
{

}

int FF_OrderDrv::setOrderFlag(int flagId, const QString &data, bool unique)
{
    if (!openDB())
        return false;
    if (unique) {
        if (!prepare("select id from o_order_flags where order_id=:order_id and flag_id=:flag_id")) {
            close();
            return -1;
        }
        bind(":order_id", m_header.id);
        bind(":flag_id", flagId);
        execSQL();
        if (next()) {
            close();
            return 1;
        }
    }
    if (!prepare("insert into o_order_flags values (null, :order_id, :flag_id, :data)")) {
        close();
        return -1;
    }
    bind(":order_id", m_header.id);
    bind(":flag_id", flagId);
    bind(":data", data);
    if (!execSQL()) {
        close();
        return -1;
    }
    close();
    return 0;
}

RemindThread::RemindThread(const QString &username)
{
    connect(this, SIGNAL(finished()), this, SLOT(deleteLater()));
    m_username = username;
}

void RemindThread::setData(int staff, int table, int dish, float qty)
{
    m_staff = staff;
    m_table = table;
    m_dish = dish;
    m_qty = qty;
}

void RemindThread::run()
{
    QMutexLocker ml(&___dbMutex);

    QSqlDatabase &db = QSqlDB::dbByName(MAIN);
    if (!db.open()) {
        // TODO QSystem::log("[RemindThread] " + db.lastError().databaseText(), false);
        quit();
        return;
    }
    QSqlQuery q(db);
    if (!q.prepare("insert into o_dishes_reminder (staff_id, table_id, dish_id, qty) values "
              "(:staff_id, :table_id, :dish_id, :qty)")) {
        //TODO QSystem::log("[RemindThread] " + q.lastError().databaseText(), false);
        quit();
        return;
    }
    q.bindValue(":staff_id", m_staff);
    q.bindValue(":table_id", m_table);
    q.bindValue(":dish_id", m_dish);
    q.bindValue(":qty", m_qty);
    if (!q.exec()) {
        //TODO QSystem::log("[RemindThread] " + q.lastError().databaseText(), false);
        quit();
        return;
    }
    db.close();
    quit();
}
