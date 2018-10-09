#include "j6_debt.h"

#define u(x) QString::fromUtf8(x)

QString caption()
{
    return u("Ապրանքներ պարտքերում");
}

QString sql () {
    return "select md.name,sum(od.qty) as qty "
            "from o_order oo, o_dishes od, me_dishes md, h_table ht "
            "where oo.id=od.order_id and ht.id=oo.table_id and od.dish_id=md.id "
            "and oo.state_id=1 and ht.hall_id=5 "
            "and (date_cash = :date1 or date_cash = :date2 or date_cash is not null) "
            "group by md.name "
            "order by md.name ";
}

QStringList fields() {
    QStringList l;
    l << u("Անուն") << u("Քնկ.");
    return l;
}

QStringList totalFields() {
    QStringList l;
    l << u("Քնկ.");
    return l;
}
