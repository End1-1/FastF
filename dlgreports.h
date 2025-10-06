#ifndef DLGREPORTS_H
#define DLGREPORTS_H

#include "fastfdialog.h"
#include "ff_user.h"
#include "ff_halldrv.h"
#include "qsqldrv.h"

namespace Ui
{
class dlgreports;
}

class dlgreports : public FastfDialog
{
    Q_OBJECT

public:
    explicit dlgreports(FF_User *user, FF_HallDrv *hall, QWidget *parent = 0);
    ~dlgreports();

private slots:
    void toError(const QString &msg);
    void toTableLocked(const QString &orderId, int tableId);
    void ordersHeaderClicked(const QModelIndex &index);
    void on_btnClose_clicked();
    void on_btnOrderFilter_clicked();
    void on_pushButton_clicked();
    void on_btnUp_clicked();
    void on_btnDown_clicked();
    void on_btnPrint_clicked();
    void on_btnDailySale_clicked();
    void on_btnPrintTaxReport_clicked();
    void on_btnTaxBack_clicked();

    void on_btnOrderDetails_clicked();

private:
    Ui::dlgreports* ui;
    FF_User* m_user;
    FF_HallDrv* m_hall;
    QSqlDrv* m_sqlDrv;
    QMap<QString, int> m_fields;
    QList<QList<QVariant> > m_data;
    QMap<QString, QString> m_filter;
    QMap<QString, QVariant> m_reports;
    QMap<QString, QStringList> m_repFields;
    QMap<QString, QStringList> m_totalFields;
    void getOrdersList();
};

#endif // DLGREPORTS_H
