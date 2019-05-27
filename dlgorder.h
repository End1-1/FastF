#ifndef DLGORDER_H
#define DLGORDER_H

#include <QDialog>
#include <QItemDelegate>
#include "ff_user.h"
#include "ff_dishesdrv.h"
#include "ff_halldrv.h"
#include "orderwindowdriver.h"
#include "qnet.h"
#include <QListWidgetItem>

namespace Ui {
class dlgorder;
}

class QDishTableItemDelegate : public QItemDelegate {
private:
    const FF_DishesDrv &m_data;

public:
    QDishTableItemDelegate(const FF_DishesDrv &data);

protected:
    virtual void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
};

class QOrderItemDelegate : public QItemDelegate {
private:
    const QList<OD_Dish*> &m_data;

public:
    QOrderItemDelegate(const QList<OD_Dish*> &data);

protected:
    virtual void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
};

class dlgorder : public QDialog
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
    void discountCheckError();
    void discountChecked();
    void message(const QString &msg);
    void printRemovedDish(int index, float qty);
    void orderCounted(const QMap<QString, QString> &values);
    void dishRepaint(int dishIndex);
    void on_tblGroupOfGroup_clicked(const QModelIndex &index);
    void on_btnClearGroupFilter_clicked();
    void on_tblDishes_clicked(const QModelIndex &index);
    void on_btn1_clicked();
    void on_btn2_clicked();
    void on_btn3_clicked();
    void on_btn4_clicked();
    void on_btn5_clicked();
    void on_btn6_clicked();
    void on_btn7_clicked();
    void on_btn8_clicked();
    void on_btn9_clicked();
    void on_btnDishCancel_clicked();
    void on_btnDishOk_clicked();
    void on_btnQtyDot_clicked();
    void on_btnPrint_clicked();
    void on_btnPayment_clicked();
    void on_btnDuplicateDish_clicked();
    void on_btnExit_clicked();
    void on_btnRemoveOrder_clicked();
    void on_btnMoveOrder_clicked();
    void on_btnDiscount_clicked();
    void on_btnTotalOrders_clicked();
    void on_btnChangeStaff_clicked();
    void on_btnMoveDish_clicked();
    void on_btnTaxPrint_clicked();
    void on_btnDishUp_3_clicked();
    void on_btnGroupDown_clicked();
    void on_btnGroupUp_clicked();
    void on_pushButton_clicked();
    void on_btnDishDown_clicked();
    void on_btnDishUp_clicked();
    void on_btnComment_clicked();
    void on_tblDishGroup_clicked(const QModelIndex &index);
    void on_btn10_clicked();
    void on_btnHistory_clicked();
    void on_btnChangeMenu_clicked();
    void on_lstOrder_currentRowChanged(int currentRow);
    void on_btnPlugins_clicked();
    void on_btnComment_2_clicked();
    void on_btnCalculator_clicked();
    void on_pushButton_2_clicked();
    void on_lstOrder_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous);
    void on_btnRmDish_clicked(bool checked);
    void on_btnPresent_clicked();
    void on_btnOrange_clicked();
    void on_btnCandyCotton_clicked();
    void on_btnDiscount_2_clicked();

private:
    OD_Drv *m_ord;
    Ui::dlgorder *ui;
    FF_User *m_user;
    FF_DishesDrv *m_dishesDrv;
    OrderWindowDriver m_drv;
    QList<QPushButton*> m_btnQtySet;
    QList<QWidget*> m_widgetsSet;
    FF_HallDrv *m_hallDrv;
    int m_currentMenu;
    bool m_flagCashMode;
    void makeDishesList();
    void buildGroupOfGroups();
    void buildDishesView();
    void buildTypes(int menuId, int groupId);
    void buildDishes(int typeId);
    int dishIndexFromListWidget();
    void disableWidgetsSet(bool enabled);
    void setButtonsState();
    void moveTable();
    void insertDiscount();
    void moveDish();
    void beforeClose();
    void giftCard(const QString &code);

signals:
    void qtyClick(float qty);
};

#endif // DLGORDER_H
