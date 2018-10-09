#include "mainwindow.h"

#define u(x) QString::fromUtf8(x)

QString caption()
{
    return u("ՀԴՄ-ի ստուգում");
}

QString sql () {
    return "select t.name || '-' || o.id || '-', ol.fnumber  || '-' || o.amount \
            from o_order o, o_tax ol, h_table t \
            where o.id=ol.fid  and t.id=o.table_id and o.state_id=2 and o.date_cash between :date1 and :date2 \
            order by o.date_CLOSE ";
}

QStringList fields() {
    QStringList l;
    l << u("Սեղան-Պատվեր") << u("ՀԴՄ-Amount");
    return l;
}

QStringList totalFields() {
    QStringList l;
    //l << u("Քնկ.");
    return l;
}
