#include "od_header.h"
#include "od_drv.h"

OD_Header::OD_Header()
{
    m_saved = true;
}

void OD_Header::loadFromDb(DbDriver &drv)
{
    int i = 0;
    f_id = drv.v_str(i++);
    f_stateId = drv.v_int(i++);
    f_tableId = drv.v_int(i++);
    f_tableName = drv.v_str(i++);
    f_dateOpen = drv.v_dateTime(i++);
    f_dateClose = drv.v_dateTime(i++);
    f_dateCash = drv.v_date(i++);
    f_staffId = drv.v_int(i++);
    f_staffName = drv.v_str(i++);
    f_printQty = drv.v_int(i++);
    f_amount = drv.v_dbl(i++);
    f_amount_inc = drv.v_dbl(i++);
    f_amount_dec = drv.v_dbl(i++);
    f_amount_inc_value = drv.v_dbl(i++);
    f_amount_dec_value = drv.v_dbl(i++);
    f_payment = drv.v_int(i++);
    f_taxPrint = drv.v_int(i++);
    f_comment = drv.v_str(i++);
}

int OD_Header::saveToDb(DbDriver &drv, const QString &orderPrefix)
{
    if (m_saved)
        return 0;
    if (!f_id.length()) {
        int new_id = drv.genId("GEN_O_ORDER_ID");
        if (!new_id)
            return -1;
        f_id = QString("%1%2").arg(orderPrefix).arg(new_id);
        if (!drv.prepare("insert into o_order (id) values (:id)"))
            return -1;
        drv.bindValue(":id", f_id);
        if (!drv.execSQL())
            return -1;
        //LogThread::logOrderThread(f_currStaffId, f_id, "New order", "");
        f_stateId = ORDER_STATE_OPEN;
        f_dateOpen = QDateTime::currentDateTime();
        f_dateClose = QDateTime::currentDateTime();
        f_dateCash = DbDriver::serverDate();
        f_staffId = f_currStaffId;
        f_staffName = f_currStaffName;
        f_printQty = 0;
        f_payment = 0;
        f_taxPrint = 0;
    }
    if (!drv.prepare("update o_order set state_id=:state_id, table_id=:table_id, date_open=:date_open, date_close=:date_close, "
                 "date_cash=:date_cash, staff_id=:staff_id, print_qty=:print_qty, "
                 "amount=:amount, amount_inc=:amount_inc, amount_dec=:amount_dec, "
                 "amount_inc_value=:amount_inc_value, amount_dec_value=:amount_dec_value, payment=:payment, taxprint=:taxprint, "
                 "comment=:comment where id=:id"))
        return -1;
    drv.bindValue(":state_id", f_stateId);
    drv.bindValue(":table_id", f_tableId);
    drv.bindValue(":date_open", f_dateOpen);
    drv.bindValue(":date_close", f_dateClose);
    drv.bindValue(":date_cash", f_dateCash);
    drv.bindValue(":staff_id", f_staffId);
    drv.bindValue(":print_qty", f_printQty);
    drv.bindValue(":payment", f_payment);
    drv.bindValue(":taxprint", f_taxPrint);
    drv.bindValue(":amount", f_amount);
    drv.bindValue(":amount_inc", f_amount_inc);
    drv.bindValue(":amount_dec", f_amount_dec);
    drv.bindValue(":amount_inc_value", f_amount_inc_value);
    drv.bindValue(":amount_dec_value", f_amount_dec_value);
    drv.bindValue(":comment", f_comment);
    drv.bindValue(":id", f_id);
    if (!drv.execSQL())
        return -1;
    /*
    if (!drv.prepare("update o_dishes set price_inc=:price_inc, price_dec=:price_dec where order_id=:order_id and state_id=:state_id ")) {
        return -1;
    }

    drv.bindValue(":price_inc", f_amount_inc_value);
    drv.bindValue(":price_dec", f_amount_dec_value);

    drv.bindValue(":order_id", f_id);
    drv.bindValue(":state_id", 1);
    if (!drv.execSQL()) {
        return -1;
    }
    */
    m_saved = true;
    return f_stateId;
}

