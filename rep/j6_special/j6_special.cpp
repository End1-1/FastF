#include "j6_special.h"

#define u(x) QString::fromUtf8(x)

QString caption()
{
    return u("Ջ6 հատուկ հաշվետվություն");
}

QString sql () {
    return "select sum(amount) + sum(amount_dec) - sum(amount_inc), "
            "sum(amount_inc), sum(amount_dec), sum(amount), "
            "sum(amount) - sum(amount_inc) "
            "from o_order oo "
            "where oo.state_id=2 "
            "and oo.date_cash between :date1 and :date2";
}

QStringList fields() {
    QStringList l;
    l << u("Հաշվարկված է") << u("Սպասարկում") << u("Զեղջ") << u("Ընդամենը") << u("Վերջնական");
    return l;
}

QStringList totalFields() {
    QStringList l;
    l << u("Վերջնական");
    return l;
}
