#ifndef OD_CONFIG
#define OD_CONFIG

#include "../dbdriver.h"

#define SQL_ODISHES "select od.id, od.order_id, od.state_id, od.dish_id, md.name, od.qty, od.printed_qty, od.price, od.last_user, " \
    "e.fname || ' ' || e.lname as lastusername, od.store_id, od.payment_mod, od.print1, od.print2, od.comments, od.remind, od.price_inc, od.price_dec, " \
    "mt.adgcode, od.flag14 " \
    "from o_dishes od, employes e, me_dishes md, me_types mt " \
    "where od.dish_id=md.id and od.last_user=e.id and od.order_id=:order_id and md.type_id=mt.id " \
    "order by od.id "

#define SQL_OORDER "select o.id, o.state_id, o.table_id, t.name, o.date_open, o.date_close, o.date_cash, " \
    "staff_id, e.fname || ' ' || e.lname, print_qty, amount, amount_inc, amount_dec, " \
    "amount_inc_value, amount_dec_value, payment, taxprint, comment " \
    "from o_order o, employes e, h_table t " \
    "where o.staff_id=e.id and o.id=:id and o.table_id=t.id "

#define OFLAG_IDRAM 1
#define OFLAG_MENU_AM 2
#define OFLAG_DELIVERY 3
#define OFLAG_KARAOKE_CLUB 4

#define ORDER_STATE_OPEN 1
#define ORDER_STATE_CLOSED 2
#define ORDER_STATE_EMTPY1 3 //Full empty
#define ORDER_STATE_EMPTY2 4 //Contain printed and then removed dishes
#define ORDER_STATE_REMOVED 5
#define ORDER_STATE_MERGE 6

#define DISH_STATE_NORMAL 1
#define DISH_STATE_REMOVED_NORMAL 2
#define DISH_STATE_REMOVED_PRINTED 3
#define DISH_STATE_MOVED 4
#define DISH_STATE_MOVE_PARTIALY 5
#define DISH_STATE_REMOVE_AFTER_CHECKOUT 6

#define DISH_MOD_NORMAL 1
#define DISH_MOD_NOINCDEC 2

#define dts(value) QString::number(value, 'f', 2)
#define its(value) QString::number(value)
#define datets(value) value.toString(DATE_FORMAT)

#define page_width 55

#endif // OD_CONFIG

