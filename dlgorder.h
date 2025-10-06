#ifndef DLGORDER_H
#define DLGORDER_H

#include "fastfdialog.h"
#include <QItemDelegate>
#include "ff_user.h"
#include "ff_dishesdrv.h"
#include "od_dish.h"
#include "od_drv.h"
#include "ff_halldrv.h"
#include <QTimer>
#include <QListWidgetItem>

namespace Ui
{
class dlgorder;
}

class QDishTableItemDelegate : public QItemDelegate
{
private:
    const FF_DishesDrv& m_data;

public:
    QDishTableItemDelegate(const FF_DishesDrv &data);

protected:
    virtual void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
};

class QOrderItemDelegate : public QItemDelegate
{
private:
    const QList<OD_Dish*>& m_data;

public:
    QOrderItemDelegate(const QList<OD_Dish*>& data);

protected:
    virtual void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
};

class dlgorder : public FastfDialog
{
    Q_OBJECT

public:
    explicit dlgorder(QWidget *parent);
    ~dlgorder();
    void setCashMode();
    bool setData(FF_User *user, FF_HallDrv *hallDrv, int tableId, QString orderId);
    void accept();
    void reject();

private slots:
    void toLockError(const QString &msg);
    void message(const QString &msg);
    void orderCounted(const QMap<QString, QString>& values);
    void dishRepaint(int dishIndex);
    void on_tblGroupOfGroup_clicked(const QModelIndex &index);
    void on_tblDishes_clicked(const QModelIndex &index);
    void on_btn1_clicked();
    void on_btn2_clicked();
    void on_btn3_clicked();
    void on_btn4_clicked();
    void on_btnPrint_clicked();
    void on_btnPayment_clicked();
    void on_btnDuplicateDish_clicked();
    void on_btnExit_clicked();
    void on_btnMoveOrder_clicked();
    void on_btnDiscount_clicked();
    void on_btnTotalOrders_clicked();
    void on_btnChangeStaff_clicked();
    void on_btnMoveDish_clicked();
    void on_btnDishUp_3_clicked();
    void on_btnGroupDown_clicked();
    void on_btnGroupUp_clicked();
    void on_pushButton_clicked();
    void on_btnDishDown_clicked();
    void on_btnDishUp_clicked();
    void on_btnComment_clicked();
    void on_tblDishGroup_clicked(const QModelIndex &index);
    void on_btnPlugins_clicked();
    void on_btnCalculator_clicked();
    void on_lstOrder_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous);
    void on_btnPresent_clicked();
    void on_btnDiscount_2_clicked();
    void on_btnPluse05_clicked();
    void on_btnMinus05_clicked();
    void on_btnPlus1_clicked();
    void on_btnMinus1_clicked();
    void on_btnRmDish_clicked();
    void on_btnAnyQty_clicked();
    void on_btnRequestOrderRemove_clicked();
    void on_btnEmark_clicked();
    void on_leCmd_returnPressed();

    void on_btnOrderComment_clicked();

private:
    OD_Drv* m_ord;
    Ui::dlgorder* ui;
    FF_User* m_user;
    FF_DishesDrv* m_dishesDrv;
    QList<QWidget*> m_widgetsSet;
    FF_HallDrv* m_hallDrv;
    QTimer mTimer;
    int m_currentMenu;
    bool m_flagCashMode;
    void setQty(double qty);
    void decQty(double qty);
    void incQty(double qty);
    void makeDishesList();
    void buildGroupOfGroups();
    void buildDishesView();
    void buildTypes(int menuId, int groupId);
    void buildDishes(int typeId);
    int dishIndexFromListWidget();
    int listIndexOfDish(OD_Dish *d);
    OD_Dish* dishFromListWidget();
    void setButtonsState();
    void moveDish(int index, int dtid, const QString &dtname);
    void insertDiscount();
    void beforeClose();
    void giftCard(const QString &code);
    void moveOrderDish(int index, int tableId, QString tableName);
    int addNewDish(int dishIndex);
    void toTableLockedMove(int index, const QString &tableName, int tableId);
};

#endif // DLGORDER_H
