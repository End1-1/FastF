#include "od_flags.h"
#include "od_drv.h"

OD_Flags::OD_Flags()
{
}

bool OD_Flags::loadFromDB(const QString &orderId, DbDriver &db)
{
    /* Database must be opened */
    m_orderId = orderId;
    if (!db.prepare("select flag_id, data from o_order_flags where order_id=:order_id"))
        return false;
    db.bindValue(":order_id", m_orderId);
    if (!db.execSQL())
        return false;
    while (db.next())
        m_flags.insert(db.v_int(0), db.v_str(1));
    return true;
}

int OD_Flags::setFlag(int flagId, const QString &data, DbDriver &db)
{
    if (m_flags.contains(flagId))
        return 1;
    if (!db.openDB())
        return -1;
    if (!db.prepare("insert into o_order_flags values (null, :order_id, :flag_id, :data)"))
        return -1;
    db.bindValue(":order_id", m_orderId);
    db.bindValue(":flag_id", flagId);
    db.bindValue(":data", data);
    if (!db.execSQL())
        return -1;
    db.closeDB();
    m_flags.insert(flagId, data);
    return 0;
}

