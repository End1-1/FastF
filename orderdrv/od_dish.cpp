#include "od_dish.h"
#include "classes.h"

OD_Dish::OD_Dish()
{
    f_totalQty = 0.0;
    f_printedQty = 0.0;
    f_priceInc = 0;
    f_priceDec = 0;
    m_qty05 = false;
    m_saved = true;
    flag14 = 0;
    f_storestate = 0;
    f_qr = 0;
}

OD_Dish *OD_Dish::copy()
{
    OD_Dish *dish = new OD_Dish();
    dish->f_id = 0;
    dish->f_orderId = f_orderId;
    dish->f_stateId = f_stateId;
    dish->f_dishId = f_dishId;
    dish->f_dishName = f_dishName;
    dish->f_totalQty = f_totalQty;
    dish->f_price = f_price;
    dish->f_printedQty = f_printedQty;
    dish->f_lastUser = f_lastUser;
    dish->f_lastUserName = f_lastUserName;
    dish->f_storeId = f_storeId;
    dish->f_paymentMod = f_paymentMod;
    dish->f_print1 = f_print1;
    dish->f_print2 = f_print2;
    dish->f_comments = f_comments;
    dish->f_remind = f_remind;
    dish->f_priceInc = f_priceInc;
    dish->f_priceDec = f_priceDec;
    dish->f_adgCode = f_adgCode;
    dish->m_saved = false;
    dish->flag14 = flag14;
    dish->f_storestate = f_storestate;
    dish->f_qr = f_qr;
    return dish;
}

void OD_Dish::loadFromDb(DbDriver &db)
{
    int i = 0;
    f_id = db.v_int(i++);
    f_orderId = db.v_str(i++);
    f_stateId = db.v_int(i++);
    f_dishId = db.v_int(i++);
    f_dishName = db.v_str(i++);
    f_totalQty = db.v_dbl(i++);
    f_printedQty = db.v_dbl(i++);
    f_price = db.v_dbl(i++);
    f_lastUser = db.v_int(i++);
    f_lastUserName = db.v_str(i++);
    f_storeId = db.v_int(i++);
    f_paymentMod = db.v_int(i++);
    f_print1 = db.v_str(i++);
    f_print2 = db.v_str(i++);
    f_comments = db.v_str(i++);
    f_remind = db.v_int(i++);
    f_priceInc = db.v_dbl(i++);
    f_priceDec = db.v_dbl(i++);
    f_adgCode = db.v_str(i++);
    flag14 = db.v_int(i++);
    f_storestate = db.v_int(i++);
    f_cancelrequest = db.v_int(i++);
    f_removeReason = db.v_str(i++);
    f_emarks = db.v_str(i++);
    f_qr = db.v_int(i++);
}

bool OD_Dish::saveToDB(DbDriver &db)
{
    if (m_saved)
        return true;
    if (!f_id) {
        f_id = db.genId("GEN_O_DISH_ID");
        if (!f_id)
            return false;
        if (!db.prepare("insert into o_dishes (id) values (:id)"))
            return false;
        db.bindValue(":id", f_id);
        if (!db.execSQL())
            return false;
    }
    if (!db.prepare("update o_dishes set order_id=:order_id, state_id=:state_id, dish_id=:dish_id, qty=:qty, printed_qty=:printed_qty, price=:price,"
                 "last_user=:last_user, store_id=:store_id, print_schema=:print_schema, comments=:comments,"
                 "print1=:print1, print2=:print2, f_removereason=:f_removereason, "
                 "payment_mod=:payment_mod, price_inc=:price_inc, price_dec=:price_dec, remind=:remind,"
                 "flag14=:flag14, f_storestate=:f_storestate, cancelrequest=:f_cancelrequest,"
                 "emarks=:emarks, qr=:qr where id=:id"))
        return false;
    db.bindValue(":order_id", f_orderId);
    db.bindValue(":state_id", f_stateId);
    db.bindValue(":dish_id", f_dishId);
    db.bindValue(":qty", f_totalQty);
    db.bindValue(":printed_qty", f_printedQty);
    db.bindValue(":price", f_price);
    db.bindValue(":last_user", f_lastUser);
    db.bindValue(":store_id", f_storeId);
    db.bindValue(":print1", f_print1);
    db.bindValue(":print2", f_print2);
    db.bindValue(":comments", f_comments);
    db.bindValue(":payment_mod", f_paymentMod);
    db.bindValue(":price_inc", f_priceInc);
    db.bindValue(":price_dec", f_priceDec);
    db.bindValue(":remind", f_remind);
    db.bindValue(":id", f_id);
    db.bindValue(":flag14", flag14);
    db.bindValue(":f_storestate", f_storestate);
    db.bindValue(":f_removereason", f_removeReason);
    db.bindValue(":cancelrequest", f_cancelrequest);
    db.bindValue(":emarks", f_emarks.isEmpty() ? QVariant() : f_emarks);
    db.bindValue(":qr", f_qr);
    if (!db.execSQL())
        return false;
    m_saved = true;
    return true;
}

void OD_Dish::setPriceMod(float inc, float dec)
{
    switch (f_paymentMod) {
    case 1:
        if (f_priceInc < 0.01)
            f_priceInc = inc;
        if (f_priceDec < 0.01)
            f_priceDec = dec;
        break;
    case 2:
        f_priceInc = 0;
        f_priceDec = 0;
        break;
    default:
        break;
    }
    m_saved = false;
}

float OD_Dish::getTotal()
{
    f_amount = f_totalQty * f_price;
    f_amountInc = f_amount * f_priceInc;
    f_amountDec = (f_amount + f_amountInc) * f_priceDec;
    return f_amount;
}

float OD_Dish::getTotalInc()
{
    return f_amountInc;
}

float OD_Dish::getTotalDec()
{
    return f_amountDec;
}

void OD_Dish::setOldQty()
{
    m_oldQty = f_totalQty;
}

void OD_Dish::commitDecQty()
{
    if (f_totalQty < f_printedQty) {
        float rm = f_printedQty - f_totalQty;
        f_printedQty = f_totalQty;
        emit removed(m_index, rm);
    }
    if (f_totalQty < 0.01) {
        f_stateId = DISH_STATE_REMOVED_NORMAL;
    }
    m_saved = false;
    emit update(m_index);
}

void OD_Dish::rollbackDecQty()
{
    f_totalQty = m_oldQty;
    m_saved = false;
    emit update(m_index);
}

float OD_Dish::availableForRemove()
{
    if (!m_removePrinted)
        return f_totalQty - f_printedQty;
    else
        return f_totalQty;
}

