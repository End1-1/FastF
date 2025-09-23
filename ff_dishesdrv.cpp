#include "ff_dishesdrv.h"
#include "ff_settingsdrv.h"

bool FF_DishesDrv::m_firstRefresh = false;
QList<DishesData::GroupOfGroup> FF_DishesDrv::m_groupOfGroup;
QMap<QString, int> FF_DishesDrv::m_fieldsMap;
QList<QList<QVariant> > FF_DishesDrv::m_fullMenu;
QList<int> FF_DishesDrv::m_dishPresent;
QMap<int, QList<int> > FF_DishesDrv::m_groupOfGroupMembers;

FF_DishesDrv::FF_DishesDrv() :
    QSqlDrv("FASTFF", "main")
{
    if (!m_firstRefresh) {
        refreshData();
        m_firstRefresh = true;
    }
}

void FF_DishesDrv::filterTypes(int menuId, int groupId)
{
    m_proxyTypes.clear();
    m_filterGroup = groupId;
    m_filterMenu = menuId;
    int menuIndex = m_fieldsMap["MENU_ID"];
    int typeIdIndex = m_fieldsMap["TYPE_ID"];
    int typeNameIndex = m_fieldsMap["TYPE_NAME"];
    bool checkMenu = menuId > 0;
    bool checkGroup = groupId > 0;
    for (QList<QList<QVariant> >::const_iterator it = m_fullMenu.begin(); it != m_fullMenu.end(); it++) {
        if (checkMenu)
            if (it->at(menuIndex).toInt() != menuId)
                continue;
        if (checkGroup)
            if (!m_groupOfGroupMembers[groupId].contains(it->at(typeIdIndex).toInt()))
                continue;
        m_proxyTypes[it->at(typeNameIndex).toString()] = it->at(typeIdIndex).toInt();
    }
}

void FF_DishesDrv::filterDishes(int typeId)
{
    m_proxyDishes.clear();
    int groupIndex = m_fieldsMap["GROUP_ID"];
    int menuIndex = m_fieldsMap["MENU_ID"];
    int typeIdIndex = m_fieldsMap["TYPE_ID"];
    int dishNameIndex = m_fieldsMap["DISH_NAME"];
    int index = -1;
    for (QList<QList<QVariant> >::const_iterator it = m_fullMenu.begin(); it != m_fullMenu.end(); it++) {
        index++;
        if (it->at(menuIndex).toInt() != m_filterMenu)
            continue;
//        if (m_filterGroup)
//            if (it->at(groupIndex).toInt() != m_filterGroup)
//                continue;
        if (it->at(typeIdIndex).toInt() != typeId)
            continue;
        m_proxyDishes[it->at(dishNameIndex).toString()] = index;
    }
}

int FF_DishesDrv::indexOfDishByBarcode(const QString &barcode)
{
    for (int i = 0; i < m_fullMenu.count(); i++) {
        if (prop(i, "BARCODE").toString() == barcode) {
            return i;
        }
    }
    return -1;
}

void FF_DishesDrv::refreshData()
{
    //Groups of groups
    m_groupOfGroup.clear();
    m_sql = "select id, name, menu_id from me_group_types order by 2";
    if (!openDB())
        return;
    if (!prepare())
        return;
    if (!execSQL())
        return;
    while (m_query->next()) {
        DishesData::GroupOfGroup g;
        g.id = m_query->value("ID").toInt();
        g.menu_id = m_query->value("MENU_ID").toInt();
        g.name = m_query->value("NAME").toString();
        m_groupOfGroup.append(g);
    }
    //Group of groups members
    m_groupOfGroupMembers.clear();
    m_sql = "select group_id, member_id from me_group_types_members";
    if (!prepare())
        return;
    if (!execSQL())
        return;
    while (m_query->next()) {
        if (!m_groupOfGroupMembers.contains(m_query->value(0).toInt()))
            m_groupOfGroupMembers[m_query->value(0).toInt()] = QList<int>();
        m_groupOfGroupMembers[m_query->value(0).toInt()].append(m_query->value(1).toInt());
    }

    //All menu
    QString where;
    if (FF_SettingsDrv::value(SD_AVAILABLE_MENU).toString().length())
        where = " and mm.id in (" + FF_SettingsDrv::value(SD_AVAILABLE_MENU).toString() + ")";
    m_proxyTypes.clear();
    m_proxyDishes.clear();
    m_sql = "select mm.id as menu_id, mm.name menu_name, "
            "mt.id as type_id, mt.name as type_name, "
            "md.id as dish_id, md.name as dish_name, md.payment_mod, md.color, md.queue, "
            "m.price, m.store_id, m.print_schema, md.remind, mt.adgcode, "
            "m.print1, m.print2, md.barcode, md.qr, md.f_addbymanager "
            "from me_dishes_menu m "
            "left join me_menus mm on mm.id=m.menu_id "
            "left join me_dishes md on md.id=m.dish_id "
            "left join me_types mt on mt.id=md.type_id "
            "where mm.name is not null and m.state_id=1 " + where +
            "order by mm.name, mt.name, md.queue, md.name ";
    if (!prepare())
        return;
    if (!execSQL())
        return;

    fillData(m_fieldsMap, m_fullMenu);

    m_dishPresent.clear();
    if (FF_SettingsDrv::value(SD_AVAILABLE_PRESENT).toString().length()) {
        m_sql = "select distinct(id) from me_dishes_present where set_id in(" + FF_SettingsDrv::value(SD_AVAILABLE_PRESENT).toString() + ") ";
        if (!prepare())
            return;
        if (!execSQL())
            return;
        while (m_query->next())
            m_dishPresent.append(m_query->value(0).toInt());
    }

    close();
}

QVariant FF_DishesDrv::prop(int index, const QString &name) const
{
    return QSqlDrv::prop(index, name, m_fieldsMap, m_fullMenu);
}

QMap<QString, QVariant> FF_DishesDrv::getMenuList()
{
    int indexId = m_fieldsMap["MENU_ID"];
    int indexName = m_fieldsMap["MENU_NAME"];
    QMap<QString, QVariant> menuMap;
    for (int i = 0; i < m_fullMenu.count(); i++)
        menuMap[m_fullMenu.at(i).at(indexName).toString()] = m_fullMenu.at(i).at(indexId).toInt();
    return menuMap;
}

bool FF_DishesDrv::canPresent(int id)
{
    return m_dishPresent.contains(id);
}
