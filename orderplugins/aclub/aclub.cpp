#include "aclub.h"

QString caption()
{
    return QString::fromUtf8("AClub");
}

bool filterDate()
{
    return false;
}

bool exec(const QMap<QString, QString> &filter, OD_Drv *o, QString &msg) {

    Q_UNUSED(filter)
    QSqlDatabase db = QSqlDatabase::addDatabase("QIBASE");
    o->configureOtherDB(db);
    db.open();
    QSqlQuery ql(db);
    ql.prepare("select id from o_order_flags where order_id=:order_id and flag_id=5");
    ql.bindValue(":order_id", o->m_header.f_id);
    ql.exec();
    if (!ql.next()) {
        ql.prepare("insert into o_order_flags (order_id, flag_id) values (:order_id, 5)");
        ql.bindValue(":order_id", o->m_header.f_id);
        ql.exec();
    }
    db.close();
    msg = QString::fromUtf8("ԱՔլաբ-ը նշված է");
    return true;
}
