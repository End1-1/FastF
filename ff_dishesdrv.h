#ifndef FF_DISHESDRV_H
#define FF_DISHESDRV_H

#include "qsqldrv.h"

namespace DishesData {
    struct GroupOfGroup {
        int id;
        int menu_id;
        QString name;
    };
}

class FF_DishesDrv : public QSqlDrv
{
    Q_OBJECT

private:
    static bool m_firstRefresh;
    static QMap<QString, int> m_fieldsMap;
    static QList<QList<QVariant> > m_fullMenu;
    static QMap<int, QList<int> > m_groupOfGroupMembers;
    int m_filterGroup;
    int m_filterMenu;

public:
    explicit FF_DishesDrv();
    static QList<DishesData::GroupOfGroup> m_groupOfGroup;
    static QList<int> m_dishPresent;
    QMap<QString, int> m_proxyTypes;
    QMap<QString, int> m_proxyDishes;
    void filterTypes(int menuId, int groupId);
    void filterDishes(int typeId);
    int indexOfDishByBarcode(const QString &barcode);
    void refreshData();
    QVariant prop(int index, const QString &name) const;
    QMap<QString, QVariant> getMenuList();
    static bool canPresent(int id);

signals:

public slots:

};

#endif // FF_DISHESDRV_H
