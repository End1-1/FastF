#include "ff_moddrv.h"

FF_ModDrv::FF_ModDrv() :
    QSqlDrv("FASTF", "main")
{
}

void FF_ModDrv::init()
{
    m_fields.clear();
    m_data.clear();

    if (!execSQL("select * from mod_order"))
        return;
    fillData(m_fields, m_data);
    close();
}

void FF_ModDrv::autoMod(FF_OrderDrv *orderDrv)
{
    for (int i = 0; i < m_data.count(); i++) {
        if (prop(i, "VALUE_TYPE", m_fields, m_data).toInt() != MOD_TYPE_INC)
            continue;
        if (prop(i, "AUTO", m_fields, m_data).toInt()) {
            orderDrv->incMod(prop(i, "VALUE_DATA", m_fields, m_data).toDouble());
            break;
        }
    }
}

FF_ModDrv &FF_ModDrv::operator=(const FF_ModDrv &src)
{
    m_fields = src.m_fields;
    m_data = src.m_data;
    m_db = src.m_db;
    return *this;
}
