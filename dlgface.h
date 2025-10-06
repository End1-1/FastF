#ifndef DLGFACE_H
#define DLGFACE_H

#include "fastfdialog.h"
#include <QItemDelegate>
#include "ff_halldrv.h"
#include <QTimer>

namespace Ui
{
class DlgFace;
}

class ReadyDishDelegate : public QItemDelegate
{
protected:
    virtual void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
};

class HallItemDelegate : public QItemDelegate
{
private:
    FF_HallDrv* m_hallDrv;

public:
    HallItemDelegate(FF_HallDrv *hallDrv);

protected:
    virtual void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
};

class DlgFace : public FastfDialog
{
    Q_OBJECT

public:
    explicit DlgFace(QWidget *parent = nullptr);
    ~DlgFace();

private slots:
    void toErrText(const QString &msg);
    void toTableLocketText(int tableId);
    void toError(const QString &msg);
    void toTableLocked(int tableId);
    void timer();
    void sqlError(const QString &msg);
    void on_btnExit_clicked();
    void on_tblHall_clicked(const QModelIndex &index);
    void on_btnSettings_clicked();
    void on_btnReport_clicked();
    void on_btnChangePass_clicked();
    void on_btnFilter_clicked();
    void on_lstReminder_clicked(const QModelIndex &index);
    void on_btnFilterUsedTables_clicked();
    void onlineUpReply(const QString &data, bool isError);
    void on_btnConfigMobile_clicked();
    void on_btnRegisterCard_clicked();
    void on_btnIn_clicked();
    void on_btnOut_clicked();
    void on_btnListOfWorkers_clicked();

private:
    int m_timeout;
    int m_filterHall;
    bool m_filterUsedTables;
    Ui::DlgFace* ui;
    FF_HallDrv* m_hallDrv;
    QTimer m_timer;
    void configHallGrid();
    void initFace();
    void loadReadyDishes();
    void correctTime();
    void startService();
};

#endif // DLGFACE_H
