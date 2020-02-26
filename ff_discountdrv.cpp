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

    m_sql = "select c.id, c.card_value, c.code from costumers_names c where c.code=:code";
    if (!prepare())
        return false;
    m_query->bindValue(":code", code);
    if (!execSQL())
        return false;
    if (m_query->next()) {
        output["ID"] = m_query->value("ID");
        output["VALUE_DATA"] = m_query->value("CARD_VALUE");
        output["CODE"] = m_query->value("CODE");
    } else {
        LogThread::logDiscountFailureThread(user, code);
    }

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
