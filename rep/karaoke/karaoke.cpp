#include "karaoke.h"

#define u(x) QString::fromUtf8(x)

QString caption()
{
    return u("Ընդհանուր հաշվետվություն");
}

QString sql () {
    return "select md.name, sum(od.qty), sum(od.qty*od.price) "
            "from o_order o, o_dishes od, me_dishes md "
            "where o.id=od.order_id and od.dish_id=md.id "
            "and o.state_id=2 and od.state_id=1 "
            "and od.dish_id = 100011 "
            "and o.date_cash between :date1 and :date2 "
            "group by 1 "
            "union "
            "select '" + u("Գենացվալե") + "', sum(od.qty), sum(od.qty*od.price) "
            "from o_order o, o_dishes od, me_dishes md "
            "where o.id=od.order_id and od.dish_id=md.id "
            "and o.state_id=2 and od.state_id=1 "
            "and od.store_id=6 "
            "and o.date_cash between :date1 and :date2 "
            "group by 1 "
            "union "
            "select '" + u("Մուտք") + "', sum(od.qty), sum(od.qty*od.price) "
            "from o_order o, o_dishes od, me_dishes md "
            "where o.id=od.order_id and od.dish_id=md.id "
            "and o.state_id=2 and od.state_id=1 "
            "and od.dish_id in (100010, 12024, 100035, 100039, 100050) "
            "and o.date_cash between :date1 and :date2 "
            "group by 1 "
            "union "
            "select '" + u("Բար") + "', sum(od.qty), sum(od.qty*od.price) "
            "from o_order o, o_dishes od, me_dishes md "
            "where o.id=od.order_id and od.dish_id=md.id "
            "and o.state_id=2 and od.state_id=1 "
            "and od.store_id=2 "
            "and o.date_cash between :date1 and :date2 "
            "and od.dish_id not in (100011, 100010, 12024, 100035, 100039, 100050) "
            "group by 1 "
            "union "
            "select '" + u("Խոհանոց") + "', sum(od.qty), sum(od.qty*od.price) "
            "from o_order o, o_dishes od, me_dishes md "
            "where o.id=od.order_id and od.dish_id=md.id "
            "and o.state_id=2 and od.state_id=1 "
            "and od.store_id=3 "
            "and o.date_cash between :date1 and :date2 "
            "group by 1 "
            ;
}

QStringList fields() {
    QStringList l;
    l << u("Անուն") << u("Քնկ.") << u("Գումար");
    return l;
}

QStringList totalFields() {
    QStringList l;
    l << u("Քնկ.") << u("Գումար");
    return l;
}
