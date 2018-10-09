#include "menu_am.h"

#define u(x) QString::fromUtf8(x)

QString caption() {
    return u("MENU.AM v2");
}

bool exec(const QMap<QString, QString> &filter, OD_Drv *o, QString &msg) {
    Q_UNUSED(filter)
    o->m_header.f_amount_dec = 0;
    o->m_header.f_amount_dec_value = 0;
    o->m_header.f_amount_inc = 0;
    o->m_header.f_amount_inc_value = 0;
    for (QList<OD_Dish*>::iterator it = o->m_dishes.begin(); it != o->m_dishes.end(); it++) {
        (*it)->f_priceDec = 0;
        (*it)->f_priceInc = 0;
    }
    o->countAmounts();
    o->saveAll();
    msg = "";
    return true;
}
