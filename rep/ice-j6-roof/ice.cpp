#include "ice.h"

#define u(x) QString::fromUtf8(x)

QString caption()
{
    return u("Տանիքի սարնարանների վաճառք");
}

QString sql () {
    return "select s.name as store_name, md.name as dish_name, sum(od.qty) as qty "
            "from o_dishes od, o_order o, st_storages s, me_dishes md "
            "where o.id=od.order_id and od.dish_id=md.id and od.store_id=s.id "
            "and o.state_id=2 and od.state_id=1 and o.date_cash between :date1 and :date2 "
            "and od.store_id = 12 and od.dish_id in (100322,90649,90648,90530,90532,90531,90524,90525,100001,90526,83896,90528, "
            "90529,100056,78959,100054,90646,90645,100328,100295,100296,100320,100145,90644,90647) "
            "group by 1, 2 "
            "order by 1, 2 ";
}

QStringList fields() {
    QStringList l;
    l << u("Սրահ") << u("Ապրանք") << u("Քնկ.");
    return l;
}

QStringList totalFields() {
    QStringList l;
    l << u("Քնկ.");
    return l;
}
