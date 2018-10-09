#include "ff_discountdrv.h"
#include "logthread.h"


FF_DiscountDrv::FF_DiscountDrv() :
    QSqlDrv("FASTFF", "main")
{
}

bool FF_DiscountDrv::checkCode(int user, QString &code, QMap<QString, QVariant> &output)
{
    code.replace(";", "");
    code.replace("?", "");
    if (!code.length())
        return false;

    m_sql = "select c.id, mod_id, value_data, c.code from costumers_names c, mod_order m "
                    "where c.mod_id=m.id and c.code=:code";
    if (!prepare())
        return false;
    m_query->bindValue(":code", code);
    if (!execSQL())
        return false;
    if (m_query->next()) {
        output["ID"] = m_query->value("ID");
        output["MOD_ID"] = m_query->value("MOD_ID");
        output["VALUE_DATA"] = m_query->value("VALUE_DATA");
        output["CODE"] = m_query->value("CODE");
    } else {
        LogThread::logDiscountFailureThread(user, code);
    }

//    //Check code and register if needed
//    if (code.length() == 14) {
//        if (output.count() > 0)  {
//            if (output["CODE"].toString().length() == 14) {
//                switch(dateOfCode(code, output["CODE"].toString())) {
//                case 0:
//                    break;
//                case 1:
//                    m_sql = "select id from mod_order where value_data=:value_data and mod_type=2";
//                    if (!prepare())
//                        return false;
//                    output["VALUE_DATA"] = valueData(code);
//                    m_query->bindValue(":value_data", output["VALUE_DATA"]);
//                    if (!execSQL())
//                        return false;
//                    if (!m_query->next())
//                        return false;
//                    output["MOD_ID"] = m_query->value(0);
//                    m_sql = "update costumers_names set mod_id=:mod_id, code=:code where id=:id";
//                    if (!prepare())
//                        return false;
//                    m_query->bindValue(":mod_id", output["MOD_ID"]);
//                    m_query->bindValue(":code", code);
//                    m_query->bindValue(":id", id(code));
//                    if (!execSQL())
//                        return false;
//                    break;
//                case -1:
//                    QSqlLog::write(TABLE_HISTORY, tr("Old card"), code, "FASTFF", 0);
//                    return false;
//                }
//            }
//        } else {
//            m_sql = "select id from mod_order where value_data=:value_data and mod_type=2";
//            if (!prepare())
//                return false;
//            output["ID"] = id(code);
//            output["VALUE_DATA"] = valueData(code);
//            m_query->bindValue(":value_data", output["VALUE_DATA"]);
//            if (!execSQL())
//                return false;
//            if (!m_query->next())
//                return false;
//            output["MOD_ID"] = m_query->value(0);
//            m_sql = "update or insert into costumers_names (id, name, mod_id, code) values (:id, '', :mod_id, :code) matching(id)";
//            if (!prepare())
//                return false;
//            m_query->bindValue(":id", id(code));
//            m_query->bindValue(":mod_id", output["MOD_ID"]);
//            m_query->bindValue(":code", code);
//            if (!execSQL())
//                return false;
//        }
//    }

    if (!output.count())
        return false;

    return true;
}

QDate FF_DiscountDrv::dateFromCode(const QString &code)
{
    QString day = code.mid(1, 2);
    QString month = code.mid(3, 2);
    QString year = "20" + code.mid(5, 2);
    return QDate::fromString(day + "." + month + ":" + year, DATE_FORMAT);
}

double FF_DiscountDrv::valueData(QString &code)
{
    return code.mid(7, 2).toDouble() / 100;
}

int FF_DiscountDrv::id(QString &code)
{
    return code.mid(9, 5).toInt();
}

int FF_DiscountDrv::dateOfCode(const QString &code1, const QString &code2)
{
    if (dateFromCode(code1) == dateFromCode(code2))
        return 0;
    if (dateFromCode(code1) > dateFromCode(code2))
        return 1;
    else
        return -1;
}
