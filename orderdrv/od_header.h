#ifndef OD_HEADER_H
#define OD_HEADER_H

#include "dbdriver.h"

class OD_Header
{
public:
    QString f_id;
    int f_stateId;
    int f_tableId;
    QString f_tableName;
    QDateTime f_dateOpen;
    QDateTime f_dateClose;
    QDate f_dateCash;
    int f_staffId;
    QString f_staffName;
    int f_printQty;
    float f_amount;
    float f_amountCard;
    float f_amount_inc;
    float f_amount_dec;
    float f_amount_inc_value;
    float f_amount_dec_value;
    int f_payment;
    int f_taxPrint;
    int f_requestremoveal = 0;
    QString f_comment;
    QString f_gat;

    int f_currStaffId;
    QString f_currStaffName;

    OD_Header();
    void loadFromDb(DbDriver &drv);
    int saveToDb(DbDriver &drv, const QString &orderPrefix);
    bool m_saved;
};

#endif // OD_HEADER_H
