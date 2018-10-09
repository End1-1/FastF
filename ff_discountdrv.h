#ifndef FF_DISCOUNTDRV_H
#define FF_DISCOUNTDRV_H

#include "qsqldrv.h"
#include <QDate>

class FF_DiscountDrv : public QSqlDrv
{
    Q_OBJECT
public:
    explicit FF_DiscountDrv();
    bool checkCode(int user, QString &code, QMap<QString, QVariant> &output);

signals:

public slots:

private:
    QDate dateFromCode(const QString &code);
    double valueData(QString &code);
    int id(QString &code);
    int dateOfCode(const QString &code1, const QString &code2);

};

#endif // FF_DISCOUNTDRV_H
