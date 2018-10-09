#include "daily_sale.h"

#define u(x) QString::fromUtf8(x)

QString caption()
{
    return u("Օրեկան հասույթ");
}

QString sql () {
    return "select e.fname || ' ' || e.lname, count(o.id), sum(amount), ' ' as sign "
            "from o_order o, employes e "
            "where o.staff_id=e.id and o.state_id=2 "
            "and o.date_cash between :date1 and :date2 "
            "group by 1, 4 "
            "union "
            "select staff, qty, amount, sign from idram  (:date1, :date2)  "
            "union "
            "select staff, qty, amount, sign from twogo  (:date1, :date2)  "
            "order by 1 ";
}

QStringList fields() {
    QStringList l;
    l << u("Անուն") << u("Քնկ.") << u("Գումար") << u("Ստոր.");
    return l;
}

QStringList totalFields() {
    QStringList l;
    l << u("Քնկ.") << u("Գումար");
    return l;
}
