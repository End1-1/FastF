#include "od_drv.h"

OD_Drv::OD_Drv() :
    OD_Base()
{
    mfSpecialActionsDishes = false;
    mfSpecialActionsOrders = false;
    mfPayOnlyOwnOrders = true;
    mfDefaultPriceDec = 0.0;
    mfDefaultPriceInc = 0.0;
}

OD_Drv::~OD_Drv()
{
    qDeleteAll(m_dishes);
}

bool OD_Drv::openTable(const QString &orderId, bool isClosed, int userId)
{
    if (!openDB())
        return false;
    if (m_header.f_tableId && (!isClosed)) {
        if (!prepare("select id from o_order where table_id=:table_id and state_id=1"))
            return false;
        bindValue(":table_id", m_header.f_tableId);
        if (!execSQL())
            return false;
        if (next())
            m_header.f_id = v_str(0);
        else
            m_header.f_id = orderId;
        if (!prepare("select gat from gat where order_id=:order_id"))
            return false;
        bindValue(":order_id", m_header.f_id);
        if (!execSQL())
            return -1;
        if (next())
            m_header.f_gat = v_str(0);
    } else
        m_header.f_id = orderId;
    if (!m_header.f_id.length()) {
        m_header.m_saved = false;
        if (!m_header.saveToDb(m_dbDrv, mfOrderIdPrefix))
            return false;
    } else {
        //LogThread::logOrderThread(userId,  m_header.f_id, "Open order", "");
        if (!prepare(SQL_OORDER))
            return false;
        bindValue(":id", m_header.f_id);
        if (!execSQL())
            return false;
        if (next()) {
            m_header.loadFromDb(m_dbDrv);
            mfDefaultPriceDec = m_header.f_amount_dec_value;
        } else {
            closeDB();
            return false;
        }
        if (!prepare(SQL_ODISHES))
            return false;
        bindValue(":order_id", m_header.f_id);
        if (!execSQL())
            return false;
        while (next()) {
            OD_Dish *d = new OD_Dish();
            d->loadFromDb(m_dbDrv);
            d->m_index = m_dishes.count();
            m_dishes.append(d);
        }
        m_flags.loadFromDB(m_header.f_id, m_dbDrv);
    }
    if (!updateTableInfo(m_header.f_id, m_header.f_tableId, m_header.f_stateId))
        return false;
    closeDB();
    countAmounts();
    return true;
}

bool OD_Drv::updateTableInfo(const QString &orderId, int tableId, int state)
{
    if (!prepare("update h_table set order_id=:order_id where id=:id"))
        return false;
    bindValue(":order_id", (state == ORDER_STATE_OPEN ? orderId : ""));
    bindValue(":id", tableId);
    if (!execSQL())
        return false;
    return true;
}

bool OD_Drv::closeTable()
{
    qDeleteAll(m_dishes);
    m_dishes.clear();
    m_header.f_id = "";
    m_header.f_tableId = 0;
    m_header.f_currStaffId = 0;
    m_header.f_staffId = 0;
    return true;
}

bool OD_Drv::saveAll()
{
    for (int i = 0; i < m_dishes.count(); i++)
        if (!m_dishes.at(i)->saveToDB(m_dbDrv))
            return false;
    if (m_header.saveToDb(m_dbDrv, mfOrderIdPrefix) < 0)
        return false;
    if (!updateTableInfo(m_header.f_id, m_header.f_tableId, m_header.f_stateId))
        return false;
    m_dbDrv.commit();
    return true;
}

bool OD_Drv::saveGat()
{
    if (!openDB())
        return false;
    if (!prepare("insert into gat (gat, order_id) values (:gat, :order_id)"))
        return false;
    bindValue(":gat", m_header.f_gat);
    bindValue(":order_id", m_header.f_id);
    if (!execSQL())
        return false;
    closeDB();
    return true;
}

bool OD_Drv::discount(int id, float value)
{
    if (!openDB())
        return false;
    m_header.f_amount_dec_value = value;
    for (int i = 0; i < m_dishes.count(); i++)
        m_dishes[i]->setPriceMod(m_header.f_amount_inc_value, m_header.f_amount_dec_value);
    countAmounts();
    if (!saveAll())
        return false;
    if (!prepare("insert into costumers_history (order_id, costumer_id) values (:order_id, :costumer_id)"))
        return false;
    bindValue(":order_id", m_header.f_id);
    bindValue(":costumer_id", id);
    if (!execSQL())
        return false;
    closeDB();
    return true;
}

void OD_Drv::countAmounts()
{
    bool flag14_1 = false;
    bool flag14_2 = false;
    QMap<QString, QString> values;
    m_header.f_amount = 0;
    m_header.f_amount_inc = 0;
    m_header.f_amount_dec = 0;
    for (QList<OD_Dish*>::iterator it = m_dishes.begin(); it != m_dishes.end(); it++) {
        if ((*it)->f_stateId != DISH_STATE_NORMAL)
            continue;
        if ((*it)->flag14 > 0) {
            if (!flag14_1 && !flag14_2) {
                flag14_1 = (*it)->flag14 == 1;
                flag14_2 = (*it)->flag14 == 2;
                if (flag14_1) {
                    m_header.f_amount += 14000;
                }
                if (flag14_2) {
                    m_header.f_amount += 7000;
                }
            }
        }
        m_header.f_amount += (*it)->getTotal();
        m_header.f_amount_inc += (*it)->getTotalInc();
        m_header.f_amount_dec += (*it)->getTotalDec();
    }
    values["counted"] = dts(m_header.f_amount);
    m_header.f_amount = m_header.f_amount + m_header.f_amount_inc - m_header.f_amount_dec;
    m_header.m_saved = false;
    values["inc_value"] = dts(m_header.f_amount_inc_value * 100);
    values["inc"] = dts(m_header.f_amount_inc);
    values["dec_value"] = dts(m_header.f_amount_dec_value * 100);
    values["dec"] = dts(m_header.f_amount_dec);
    values["total"] = dts(m_header.f_amount);
    emit counted(values);
}

int OD_Drv::appendDish(OD_Dish *newDish)
{
    if (!openDB())
        return -1;
    if (appendDish2(newDish) < 0)
        return -1;
    closeDB();
    return m_dishes.count() - 1;
}

int OD_Drv::appendDish2(OD_Dish *newDish)
{
    newDish->f_orderId = m_header.f_id;
    newDish->m_index = m_dishes.count();
    newDish->m_saved = false;
    if (!newDish->saveToDB(m_dbDrv))
        return -1;
    m_dishes.append(newDish);
    if (m_header.f_printQty > 0)
        m_header.f_printQty *= -1;
    countAmounts();
    m_header.m_saved = false;
    if (!m_header.saveToDb(m_dbDrv, mfOrderIdPrefix))
        return -1;
    return m_dishes.count() - 1;
}

OD_Dish *OD_Drv::dish(int index)
{
    if (m_dishes.count() - 1 < index)
        return 0;
    return m_dishes[index];
}

