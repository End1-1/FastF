#include "ice.h"

#define u(x) QString::fromUtf8(x)

QString caption()
{
    return u("Սարնարանների վաճառք");
}

QString sql () {
    return "select s.name as store_name, md.name as dish_name, sum(od.qty) as qty "
            "from o_dishes od, o_order o, st_storages s, me_dishes md "
            "where o.id=od.order_id and od.dish_id=md.id and od.store_id=s.id "
            "and o.state_id=2 and od.state_id=1 and o.date_cash between :date1 and :date2 "
            "and od.store_id in (4) "
            "group by 1, 2 "
            "order by 1, 2 ";
}

QStringList fields() {
    QStringList l;
    l << u("Սեղան") << u("Պատվեր") << u("ՀԴՄ") << u("Amount");
    return l;
}

QStringList totalFields() {
    QStringList l;
    //l << u("Քնկ.");
    return l;
}
