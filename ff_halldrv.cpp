#include "ff_halldrv.h"
#include "ff_settingsdrv.h"
#include "cnfmaindb.h"
#include "od_config.h"
#include <QHeaderView>

QItemDelegate *FF_HallDrv::m_itemDelegate = 0;

void FF_HallDrv::loadHall()
{
    m_hall.clear();
    m_tables.clear();
    m_proxyTables.clear();
    m_tableIdIndexMap.clear();
    m_hallTotal.clear();

    m_totalQty = 0;
    m_totalAmount = 0;

    if (!openDB())
        return;
    if (!prepare("select count(id), sum(amount) from o_order where (state_id=2 and date_cash=:date_cash) or (state_id=1)"))
        return;
    bindValue(":date_cash", FF_SettingsDrv::cashDate());
    if (!execSQL())
        return;
    if (next()) {
        m_totalQty = v_int(0);
        m_totalAmount = v_dbl(1);
    }

    QString where;
    if (FF_SettingsDrv::value(SD_AVAILABLE_HALL).toString().length())
        where = " where id in (" + FF_SettingsDrv::value(SD_AVAILABLE_HALL).toString() + ") ";
    if (!execSQL("select id, name from h_hall " + where + " order by name"))
        return;
    while (next()) {
        Hall h;
        h.id = v_int(0);
        h.name = v_str(1);
        m_hall.append(h);
    }

    QString m_sql = "select h.id, "
            "h.hall_id, "
            "h.name, "
            "h.queue, "
            "h.lock_host, "
            "h.reserved, "
            "h.flags "
            "from h_table h "
            "order by h.queue";
    if (!prepare(m_sql))
        return;
    if (!execSQL())
        return;
    while (next()) {
        Table t;
        t.id = v_int(0);
        t.hall = getHall(v_int(1));
        if (!t.hall)
            continue;
        t.name = v_str(2);
        t.queue = v_int(3);
        t.lock = v_str(4);
        t.reserved = v_str(5);
        t.flags = v_str(6);
        t.printed = 0;
        t.amount = 0;
        t.orderComment = "";
        t.orderId = "";

        m_tables.append(t);
        m_tableIdIndexMap[t.id] = m_tables.count() - 1;

        if (!m_hallTotal.contains(t.hall->id))
            m_hallTotal[t.hall->id] = HallTotal();
        if (t.amount > 0) {
            m_hallTotal[t.hall->id].qty++;
            m_hallTotal[t.hall->id].amount += t.amount;
        }
    }
    m_sql = "select o.id, "
                "o.table_id, "
                "o.print_qty, "
                "o.amount, "
                "o.date_open, "
                "e.fname || ' ' || e.lname as staff_name, "
                "o.comment "
                "from o_order o, employes e "
                "where o.state_id=1 and o.staff_id=e.id ";
    if (!prepare(m_sql))
        return;
    if (!execSQL())
        return;
    while (next()) {
    Table *t = table(v_int(1));
        if (!t)
            continue;
        t->orderId = v_str(0);
        t->printed = v_int(2);
        t->amount = v_dbl(3);
        t->dateOpen = v_dateTime(4).toString(DATETIME_FORMAT);
        t->staffName = v_str(5);
        t->orderComment = v_str(6);
        if (!m_hallTotal.contains(t->hall->id))
            m_hallTotal[t->hall->id] = HallTotal();
        if (t->amount > 0) {
            m_hallTotal[t->hall->id].qty++;
            m_hallTotal[t->hall->id].amount += t->amount;
        }
    }

    int maxid = FF_SettingsDrv::value(SD_MESSANGER_MAX_ID).toInt();
    m_sql = "select id, dst from messanger where id>:id and type_id=2 order by 1";
    if (!prepare(m_sql))
        return;
    bindValue(":id", maxid);
    if (!execSQL())
        return;
    QList<Table*> tables;
    while (next()) {
        Table *t = table(v_int(1));
        if (!t)
            continue;
        tables.append(t);
        maxid = v_int(0);
    }
    FF_SettingsDrv::m_settings[SD_MESSANGER_MAX_ID] = maxid;
    prepare("update sys_settings_values set key_value=:key_value where settings_id=:settings_id and key_name=:key_name");
    bindValue(":key_value", maxid);
    bindValue(":settings_id", FF_SettingsDrv::m_id);
    bindValue(":key_name", SD_MESSANGER_MAX_ID);
    execSQL();
    closeDB();
    for (QList<Table*>::iterator it = tables.begin(); it != tables.end(); it++)
        setFlag((*it)->id, TFLAG_CALLSTAFF, '1');
    filter(m_filterHallId, m_filterOnlyBusy);
}

FF_HallDrv::FF_HallDrv() :
    DbDriver()
{
    m_filterHallId = 0;
    m_filterOnlyBusy = false;
    configureDb(__cnfmaindb.fHost, __cnfmaindb.fDatabase, __cnfmaindb.fUser, __cnfmaindb.fPassword);
    loadHall();
}

FF_HallDrv::Hall *FF_HallDrv::getHall(int id)
{
    for (int i = 0; i < m_hall.count(); i++)
        if (m_hall.at(i).id == id)
            return &m_hall[i];
    return 0;
}

FF_HallDrv::Table *FF_HallDrv::table(int id)
{
    if (!m_tableIdIndexMap.contains(id))
        return 0;
    return &(m_tables[m_tableIdIndexMap[id]]);
}

QString FF_HallDrv::total()
{
    int qty = 0;
    float amount = 0;

    if (m_filterHallId) {
        qty = m_hallTotal[m_filterHallId].qty;
        amount = m_hallTotal[m_filterHallId].amount;
    } else {
        for (QList<Hall>::const_iterator it = m_hall.begin(); it != m_hall.end(); it++) {
            qty += m_hallTotal[it->id].qty;
            amount += m_hallTotal[it->id].amount;
        }
    }

    return QString::number(m_hallTotal[m_filterHallId].qty) + " / " + double_str(m_hallTotal[m_filterHallId].amount)
            + " [" + double_str(m_totalAmount) + " / " + QString::number(m_totalQty) + "] ";
}

void FF_HallDrv::filter(int hallId, bool onlyBusy)
{
    m_filterHallId = hallId;
    m_filterOnlyBusy = onlyBusy;
    m_proxyTables.clear();

    for (int i = 0; i < m_tables.count(); i++) {
        bool proxy = true;
        if (hallId)
            if (hallId != m_tables.at(i).hall->id)
                proxy = false;
        if (onlyBusy)
            if (!m_tables.at(i).orderId.length())
                proxy = false;
        if (proxy)
            m_proxyTables.append(m_tables.at(i).id);
    }
}

void FF_HallDrv::refresh()
{
    loadHall();
}

void FF_HallDrv::configGrid(QTableWidget *t, QItemDelegate *itemDelegate)
{
    t->clearContents();
    int colWidth = t->horizontalHeader()->defaultSectionSize();
    int colCount = (t->width() - 5) / colWidth;
    int colWidthDelta = ((t->width() - 5) - (colCount * colWidth)) / colCount;
    colWidth += colWidthDelta;
    int rowCount = m_proxyTables.count() / colCount;
    if (m_proxyTables.count() % colCount)
        rowCount++;
    for (int i = 0; i < t->columnCount(); i++)
        t->setColumnWidth(i, colWidth);

    if (itemDelegate)
        m_itemDelegate = itemDelegate;
    if (m_itemDelegate)
        t->setItemDelegate(m_itemDelegate);
    t->horizontalHeader()->setDefaultSectionSize(colWidth);
    t->setColumnCount(colCount);

    t->setRowCount(rowCount);
    int col = 0, row = 0;
    for (int i = 0; i < m_proxyTables.count(); i++) {
        QTableWidgetItem *item = new QTableWidgetItem(QString::number(table(m_proxyTables.at(i))->id));
        item->setData(Qt::UserRole, m_proxyTables.at(i));
        t->setItem(row, col, item);
        col++;
        if (col >= colCount) {
            col = 0;
            row++;
        }
    }
}

QMap<QString, QVariant> FF_HallDrv::getHallMap()
{
    QMap<QString, QVariant> hallMap;
    for (QList<Hall>::const_iterator it = m_hall.begin(); it != m_hall.end(); it++)
        hallMap[it->name] = it->id;
    return hallMap;
}

bool FF_HallDrv::tableFlag(const FF_HallDrv::Table &t, int flag)
{
    if (flag > t.flags.length() -1)
        return false;
    return t.flags.at(flag) == '1';
}

void FF_HallDrv::setFlag(int tableId, int flag, const QChar &value)
{
    Table *t = table(tableId);
    if (!t)
        return;
    if (flag > t->flags.length() -1)
        return;
    t->flags[flag] = value;
    openDB();
    prepare("update h_table set flags=:flags where id=:id");
    bindValue(":flags", t->flags);
    bindValue(":id", t->id);
    execSQL();
    closeDB();
}
