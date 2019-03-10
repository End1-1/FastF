#include "dlgorder.h"
#include "ui_dlgorder.h"
#include <QPainter>
#include <math.h>
#include "dlgmessage.h"
#include "dlgtableformovement.h"
#include "ff_settingsdrv.h"
#include "../CafeV4/core/printing.h"
#include "dlginput.h"
#include "ff_discountdrv.h"
#include "dlggetpassword.h"
#include "dlgdishcomment.h"
#include "cnfmaindb.h"
#include "dlghistory.h"
#include "dlgreportfilter.h"
#include <QCryptographicHash>
#include "dlglist.h"
#include "dlgpayment.h"
#include "dlgcalcchange.h"
#include <QScrollBar>
#include <QDir>
#include <QLibrary>
#include "orderwindowdriver.h"
#include "dlggatreader.h"
#include "cnfmaindb.h"
#include "dlgremovereason.h"
#include <QException>
#include "logthread.h"
#include "dlg14.h"

#define total_row_count 8

#define RTableName 0
#define RStaffName 1
#define ROrderId 2
#define RCounted 3
#define RService 4
#define RDiscount 5
#define RTotal 6
#define RComment 7

QStringList fGiftCards;

dlgorder::dlgorder(QWidget *parent) :
    QDialog(parent, Qt::FramelessWindowHint),
    ui(new Ui::dlgorder)
{
    ui->setupUi(this);

    if (fGiftCards.count() == 0) {
        fGiftCards << "549874523124";
    }

    ui->tblTotal->setColumnWidth(0, 150);
    ui->tblTotal->setRowCount(total_row_count);
    ui->tblTotal->setMaximumHeight((total_row_count * ui->tblTotal->verticalHeader()->defaultSectionSize()) + 5);
    for (int i = 0; i < total_row_count; i++)
        for (int j = 0; j < 2; j++)
            ui->tblTotal->setItem(i, j, new QTableWidgetItem());

    m_btnQtySet.append(ui->btn1);
    m_btnQtySet.append(ui->btn2);
    m_btnQtySet.append(ui->btn3);
    m_btnQtySet.append(ui->btn4);
    m_btnQtySet.append(ui->btn5);
    m_btnQtySet.append(ui->btn6);
    m_btnQtySet.append(ui->btn7);
    m_btnQtySet.append(ui->btn8);
    m_btnQtySet.append(ui->btn9);
    m_btnQtySet.append(ui->btn10);
    m_btnQtySet.append(ui->btnQtyDot);

    m_widgetsSet.append(ui->wdtGroup_Btn);
    m_widgetsSet.append(ui->wdtDish);
    m_widgetsSet.append(ui->wdtDishGroup);
    m_widgetsSet.append(ui->wdtOrderService);
    m_widgetsSet.append(ui->wdtMainBtn);
    m_widgetsSet.append(ui->lstOrder);

    m_dishesDrv = new FF_DishesDrv();
    m_ord = new OD_Drv();
    m_ord->mfDefaultPriceInc = FF_SettingsDrv::value(SD_SERVICE_VALUE).toFloat();
    m_ord->mfOrderIdPrefix = FF_SettingsDrv::value(SD_ORDER_ID_PREFIX).toString();
    m_ord->mfObjectName = FF_SettingsDrv::value(SD_OBJECT_NAME).toString();
    m_ord->configureDB(__cnfmaindb.fHost, __cnfmaindb.fDatabase, __cnfmaindb.fUser, __cnfmaindb.fPassword);
    m_ord->openDB();
    m_ord->m_print.getPrinterSchema(m_ord);
    m_ord->closeDB();
    showFullScreen();
}

dlgorder::~dlgorder()
{
    delete ui;
}

void dlgorder::setCashMode()
{
    m_flagCashMode = true;
    ui->wdtDishGroup->setEnabled(false);
    ui->wdtOrderService->setEnabled(false);
    for(QList<QPushButton*>::iterator i = m_btnQtySet.begin(); i != m_btnQtySet.end(); i++)
        (*i)->setEnabled(false);
    ui->lstOrder->setEnabled(false);

    ui->btnDishOk->setEnabled(false);
    ui->btnDishCancel->setEnabled(false);
    setButtonsState();
}

bool dlgorder::setData(FF_User *user, FF_HallDrv *hallDrv, int tableId, QString orderId)
{
    qApp->processEvents();
    m_hallDrv = hallDrv;
    m_user = user;

    bool isClosed = false;
    if (!tableId) {
        isClosed = true;
        m_ord->openDB();
        m_ord->prepare("select table_id from o_order where id=:id");
        m_ord->bindValue(":id", orderId);
        m_ord->execSQL();
        if (m_ord->next())
            tableId = m_ord->v_int(0);
        m_ord->closeDB();
    }
    if (!tableId) {
        DlgMessage::Msg(tr("Program error"));
        return false;
    }

    double defaultPriceInc;
    if (FF_SettingsDrv::tableOption1(tableId, defaultPriceInc))
        m_ord->mfDefaultPriceInc = defaultPriceInc;

    connect(m_ord, SIGNAL(dishRepaint(int)), this, SLOT(dishRepaint(int)));
    m_ord->m_header.f_currStaffId = user->id;
    m_ord->m_header.f_currStaffName = user->fullName;
    m_ord->m_header.f_tableId = tableId;
    if (hallDrv->table(tableId)) {
        m_ord->m_header.f_tableName = hallDrv->table(tableId)->name;
    }
    m_ord->m_header.f_amount_inc_value = m_ord->mfDefaultPriceInc;
    m_ord->m_header.f_amount = 0.0;
    m_ord->m_header.f_amount_inc = 0.0;
    m_ord->m_header.f_amount_dec = 0.0;
    m_ord->m_header.f_amount_dec_value = 0.0;
    m_ord->m_header.f_dateCash = QDate::currentDate();
    m_ord->m_header.f_comment = "";
    m_ord->m_header.f_taxPrint = 0;
    m_ord->m_header.f_payment = 0;

    if (!m_ord->openTable(orderId, isClosed, user->id))
        return false;
    connect(m_ord, SIGNAL(counted(QMap<QString,QString>)), this, SLOT(orderCounted(QMap<QString,QString>)));
    ui->tblTotal->item(RTableName, 0)->setText(tr("Table"));
    ui->tblTotal->item(RTableName, 1)->setText(m_ord->m_header.f_tableName);
    ui->tblTotal->item(RStaffName, 0)->setText(tr("Waiter"));
    ui->tblTotal->item(RStaffName, 1)->setText(m_user->fullName);
    ui->tblTotal->setSpan(RComment, 0, 1, 2);
    ui->tblTotal->item(RComment, 0)->setText(m_ord->m_header.f_comment);
    ui->tblTotal->item(ROrderId, 0)->setText(QString("%1").arg(tr("Order number")));
    ui->tblTotal->item(ROrderId, 1)->setText(m_ord->m_header.f_id);
    ui->tblTotal->item(RCounted, 0)->setText(tr("Counted"));
    ui->tblTotal->item(RCounted, 1)->setText("0");
    ui->tblTotal->item(RTotal, 0)->setText(tr("Total"));
    ui->tblTotal->item(RTotal, 1)->setText("0");
    m_ord->countAmounts();

    if (m_user->id != m_ord->m_header.f_staffId)
        DlgMessage::Msg(tr("The order opened by") + "\n" + m_ord->m_header.f_staffName + "\n"
                        + QString("%1:%2").arg(m_user->id).arg(m_ord->m_header.f_staffId));
    m_ord->m_header.f_currStaffId = m_user->id;
    m_ord->m_header.f_currStaffName = m_user->fullName;

    m_flagCashMode = false;
    m_currentMenu = FF_SettingsDrv::value(SD_DEFAULT_MENU_ID).toInt();
    makeDishesList();

    ui->tblDishes->setItemDelegate(new QDishTableItemDelegate(*m_dishesDrv));
    ui->lstOrder->setItemDelegate(new QOrderItemDelegate(m_ord->m_dishes));

    setButtonsState();
    ui->btnChangeStaff->setEnabled(m_user->roleRead(ROLE_ORDER_CHANGE_STAFF));
    buildDishesView();

    return true;
}

void dlgorder::accept()
{
    beforeClose();
    QDialog::accept();
}

void dlgorder::reject()
{
    beforeClose();
    QDialog::reject();
}

void dlgorder::discountCheckError()
{
    disconnect(&m_drv, SIGNAL(discountCheckError()), this, SLOT(discountCheckError()));
    disconnect(&m_drv, SIGNAL(discountChecked()), this, SLOT(discountChecked()));
    DlgMessage::Msg(tr("Cannot check online discount"));
    discountChecked();
}

void dlgorder::discountChecked()
{
    disconnect(&m_drv, SIGNAL(discountChecked()), this, SLOT(discountChecked()));
    disconnect(&m_drv, SIGNAL(discountCheckError()), this, SLOT(discountCheckError()));
    QString checkPrinterName = FF_SettingsDrv::value(SD_CHECK_PRINTER_NAME).toString();
    setButtonsState();
    if(!___printerInfo->printerExists(checkPrinterName)) {
        DlgMessage::Msg(tr("Printer not exists") + " :" + checkPrinterName);
        return;
    }

    if (DlgMessage::Msg(tr("Confirm to checkout")) != QDialog::Accepted)
        return;

    m_ord->m_print.printCheckout(checkPrinterName, m_ord);
    LogThread::logOrderThread(m_ord->m_header.f_currStaffId, m_ord->m_header.f_id, "Print receipt check", "");
    setButtonsState();
}

void dlgorder::discountRemoved()
{
//    disconnect(&m_drv, SIGNAL(discountRemoved()));
//    if (DlgMessage::Msg(tr("Confirm to close order")) != QDialog::Accepted)
//        return;

//    DlgSelectePaymentType *d = new DlgSelectePaymentType(this);
//    m_ord->m_header.f_payment = d->exec();
//    if (m_ord->m_header.f_payment == 2) {
//        m_ord->m_header.f_amountCard = m_ord->m_header.f_amount;
//    }
//    if (m_ord->m_header.f_payment == 3) {
//        //TODO: find payment by jazzve card
//        QString cardCode;
//        if (!DlgInput::getString(cardCode, tr("Enter card code"), this))
//            return;
//        if (cardCode.length() == 0) {
//            return;
//        }
//    }

//    if (FF_SettingsDrv::value(SD_TAX_PRINT).toInt()) {
//        QString orderNum = m_ord->m_header.f_id.right(1);
//        if (FF_SettingsDrv::value(SD_TAX_PRINT_FREQ).toString().contains(orderNum))
//            m_ord->m_print.printTax(FF_SettingsDrv::value(SD_TAX_PRINT_IP).toString(),
//                                      FF_SettingsDrv::value(SD_TAX_PRINT_PORT).toString(),
//                                      FF_SettingsDrv::value(SD_TAX_PRINT_PASS).toString(), m_ord);
//        else if (m_ord->m_header.f_amountCard > 0.01)
//            m_ord->m_print.printTax(FF_SettingsDrv::value(SD_TAX_PRINT_IP).toString(),
//                                      FF_SettingsDrv::value(SD_TAX_PRINT_PORT).toString(),
//                                      FF_SettingsDrv::value(SD_TAX_PRINT_PASS).toString(), m_ord);
//    }

//    m_ord->m_header.f_dateCash = FF_SettingsDrv::cashDate();
//    m_ord->m_header.f_stateId = ORDER_STATE_CLOSED;
//    LogThread::logOrderThread(m_ord->m_header.f_currStaffId, m_ord->m_header.f_id, "Close order", "");
//    accept();
}

void dlgorder::message(const QString &msg)
{
    DlgMessage::Msg(msg);
}

void dlgorder::printRemovedDish(int index, float qty)
{
    if (!m_ord->openDB())
        return;
    m_ord->prepare("select id, name from o_remove_reason order by name");
    m_ord->execSQL();
    QMap<int, QString> rv;
    while (m_ord->next()) {
        rv[m_ord->v_int(0)] = m_ord->v_str(1);
    }
    DlgRemoveReason *dlg = new DlgRemoveReason(rv, this);
    dlg->exec();
    int reasonId = dlg->m_reasonId;
    QString reasonName = dlg->m_reasonName;
    delete dlg;

    m_ord->m_print.printRemoved(index, qty, m_ord, reasonName);

    OD_Dish *d = m_ord->dish(index);
    QString sql = "insert into o_important (order_id, user_id, action_id, data, reason_id) values (:order_id, :user_id, :action_id, :data, :reason_id)";
    QString data = QString("%1;%2;%3;%4").arg(d->f_dishName).arg(qty).arg(d->f_price).arg(d->f_price * qty);
    if (!m_ord->prepare(sql))
        return;
    m_ord->bindValue(":order_id", m_ord->m_header.f_id);
    m_ord->bindValue(":user_id", m_ord->m_header.f_currStaffId);
    m_ord->bindValue(":action_id", 1);
    m_ord->bindValue(":data", data);
    m_ord->bindValue(":reason_id", reasonId);
    if (!m_ord->execSQL())
        return;
    m_ord->closeDB();
}

void dlgorder::orderCounted(const QMap<QString, QString> &values)
{
    ui->tblTotal->item(RService, 0)->setText(QString("%1: %2%").arg(tr("Service sum")).arg(values["inc_value"]));
    ui->tblTotal->item(RService, 1)->setText(values["inc"]);
    ui->tblTotal->item(RDiscount, 0)->setText(QString("%1: %2%").arg(tr("Discount sum")).arg(values["dec_value"]));
    ui->tblTotal->item(RDiscount, 1)->setText(values["dec"]);
    ui->tblTotal->item(ROrderId, 1)->setText(m_ord->m_header.f_id);
    ui->tblTotal->item(RCounted, 1)->setText(values["counted"]);
    ui->tblTotal->item(RTotal, 1)->setText(values["total"]);
    ui->tblTotal->item(RTableName, 1)->setText(m_ord->m_header.f_tableName);
    m_ord->saveAll();
}

void dlgorder::dishRepaint(int dishIndex)
{
    m_ord->countAmounts();
    setButtonsState();
    for (int i = 0; i < ui->lstOrder->count(); i++) {
        QModelIndex item = ui->lstOrder->model()->index(i, 0);
        if (item.data(Qt::UserRole).toInt() == dishIndex) {
            ui->lstOrder->update(item);
            return;
        }
    }
}

void dlgorder::buildTypes(int menuId, int groupId)
{
    buildDishes(0);
    ui->tblDishGroup->clear();
    ui->tblDishGroup->setColumnCount((ui->tblDishGroup->width() - 5) / ui->tblDishGroup->horizontalHeader()->defaultSectionSize());
    ui->tblDishGroup->setRowCount(1);
    m_dishesDrv->filterTypes(menuId, groupId);
    int r = 0, c = 0;
    for (QMap<QString, int>::const_iterator it = m_dishesDrv->m_proxyTypes.begin(); it != m_dishesDrv->m_proxyTypes.end(); it++) {
        QTableWidgetItem *i = new QTableWidgetItem(it.key());
        i->setData(Qt::UserRole, it.value());
        ui->tblDishGroup->setItem(r, c++, i);
        if (c >= ui->tblDishGroup->columnCount()) {
            c = 0;
            r++;
            ui->tblDishGroup->setRowCount(r + 1);
        }
    }
}

void dlgorder::buildDishes(int typeId)
{
    ui->tblDishes->clear();
    ui->tblDishes->setRowCount(0);

    int colCount = (ui->tblDishes->width() - 5) / ui->tblDishes->horizontalHeader()->defaultSectionSize(), rowCount = 0;
    ui->tblDishes->setColumnCount(colCount);
    int colDelta = (ui->tblDishes->width() - 5) - (colCount * ui->tblDishes->horizontalHeader()->defaultSectionSize());
    colDelta /= colCount;
    for (int i = 0; i < colCount; i++)
        ui->tblDishes->setColumnWidth(i, ui->tblDishes->horizontalHeader()->defaultSectionSize() + colDelta);

    if (!typeId)
        return;
    m_dishesDrv->filterDishes(typeId);
    rowCount = m_dishesDrv->m_proxyDishes.count() / colCount;
    ui->tblDishes->setRowCount(rowCount);
    if (m_dishesDrv->m_proxyDishes.count() % colCount)
        rowCount++;
    ui->tblDishes->setRowCount(rowCount);
    int r = 0, c = 0;
    for (QMap<QString, int>::const_iterator it = m_dishesDrv->m_proxyDishes.begin(); it != m_dishesDrv->m_proxyDishes.end(); it++) {
        QTableWidgetItem *i = new QTableWidgetItem(it.key());
        i->setData(Qt::UserRole, it.value() + 1);
        ui->tblDishes->setItem(r, c, i);
        c++;
        if (c == ui->tblDishes->columnCount()) {
            c = 0;
            r++;
        }
    }
}

int dlgorder::dishIndexFromListWidget()
{
    QList<QListWidgetItem*> selectedIndexes = ui->lstOrder->selectedItems();
    if (!selectedIndexes.count())
        return -1;

    return selectedIndexes.at(0)->data(Qt::UserRole).toInt();
}

void dlgorder::disableWidgetsSet(bool enabled)
{
    for (QList<QWidget *>::const_iterator it = m_widgetsSet.begin(); it != m_widgetsSet.end(); it++)
        (*it)->setEnabled(enabled);

    ui->btn10->setText(enabled ? "+10" : "10");
    ui->btnDishOk->setEnabled(!enabled);
    ui->btnDishCancel->setEnabled(!enabled);
    ui->btnRmDish->setEnabled(enabled);
}

void dlgorder::setButtonsState()
{
    if (ui->btnRmDish->isChecked())
        return;
    bool btnPrint = false;
    bool btnPay = true;
    bool btnRemoveOrder = true;
    bool btnMoveOrder = true;
    bool btnDiscount = true;
    bool btnMoveDish = true;

    for (int i = 0 ; i < m_ord->m_dishes.count(); i++) {
        if (m_ord->dish(i)->f_stateId != DISH_STATE_NORMAL)
            continue;
        if (m_ord->dish(i)->f_totalQty > m_ord->dish(i)->f_printedQty) {
            btnPrint = true;
            btnPay = false;
            break;
        }
    }

    if (btnPrint)
        if (FF_SettingsDrv::value(SD_SERIVCE_CHECK_PAY) == 1) {
            btnPay = true;
            btnPrint = false;
        }

    btnPrint = btnPrint && !m_flagCashMode;

    btnRemoveOrder = m_user->roleRead(ROLE_REMOVE_ORDER);
    if (m_flagCashMode)
        btnRemoveOrder = btnRemoveOrder && m_user->roleRead(ROLE_REMOVE_ORDER_FROM_CASH);
    if (m_ord->m_header.f_printQty > 0)
        btnRemoveOrder = btnRemoveOrder && m_user->roleRead(ROLE_REMOVE_ORDER_AFTER_CHECKOUT);

    btnMoveOrder = m_ord->m_header.f_amount > 0.01;
    btnMoveOrder = btnMoveOrder && !m_flagCashMode && m_user->roleRead(ROLE_MOVE_ORDER);
    if (m_ord->m_header.f_printQty > 0)
        btnMoveOrder = btnMoveOrder && m_user->roleRead(ROLE_REMOVE_ORDER_AFTER_CHECKOUT);

    btnMoveDish = m_user->roleRead(ROLE_MOVE_DISH);
    btnMoveDish = btnMoveDish && !m_flagCashMode && !(m_ord->m_header.f_printQty > 0) && !m_user->roleRead(ROLE_REMOVE_ORDER_AFTER_CHECKOUT);

    btnDiscount = btnDiscount && (m_ord->m_header.f_amount_dec_value < 0.01);
    if (m_ord->m_header.f_printQty > 0)
        btnDiscount = btnDiscount && m_user->roleRead(ROLE_ORDER_DISCOUNT_AFTER_CHECKOUT);
    btnDiscount = btnDiscount && !m_flagCashMode;

    ui->btnTotalOrders->setEnabled(m_user->roleRead(ROLE_W_VIEW_SALES_AMOUNT));
    ui->btnPrint->setEnabled(btnPrint);
    //ui->btnTaxPrint->setEnabled(btnCheck);
    ui->btnPayment->setEnabled(btnPay);
    ui->btnRemoveOrder->setEnabled(btnRemoveOrder || m_ord->m_header.f_currStaffId == 1);
    ui->btnMoveOrder->setEnabled(btnMoveOrder);
    ui->btnMoveDish->setEnabled(btnMoveDish);
    ui->btnDiscount->setEnabled(btnDiscount);
    if (m_ord->m_header.f_printQty)
        ui->tblDishes->setEnabled(m_user->roleRead(ROLE_REMOVE_ORDER) && m_user->roleRead(ROLE_REMOVE_ORDER_AFTER_CHECKOUT));

    bool enableControls = !(m_ord->m_header.f_printQty > 0) && !m_flagCashMode && !m_user->roleRead(ROLE_REMOVE_ORDER_AFTER_CHECKOUT);
    for(QList<QPushButton*>::iterator i = m_btnQtySet.begin(); i != m_btnQtySet.end(); i++)
        (*i)->setEnabled(enableControls);
    ui->tblDishes->setEnabled(enableControls);
    ui->wdtDishBtn->setEnabled(enableControls && !m_flagCashMode);
    ui->wdtDishGroup->setEnabled(enableControls);
    ui->btnPlugins->setEnabled(m_user->roleRead(ROLE_W_SPESIAL_ACTIONS));
    ui->wdtHdm->setEnabled(true);
    ui->btnCandyCotton->setEnabled(m_ord->m_header.f_amount > 9999);
    if (m_ord->m_header.f_printQty > 0) {
        ui->wdtDishQty->setEnabled(false);
    }
}

void dlgorder::moveTable()
{
    int tableId = m_ord->m_header.f_tableId;
    QString tableName = m_ord->m_header.f_tableName;
    if (DlgTableForMovement::getTable(tableId, this, m_hallDrv) != QDialog::Accepted)
        return;

    if (m_hallDrv->lockTable(tableId) != LOCK_SUCCESS) {
        DlgMessage::Msg(tr("Cannot lock table"));
        return;
    }

    if (!m_ord->openDB()) {
        DlgMessage::Msg(tr("Connectin error"));
        return;
    }
    if (!m_ord->prepare("select order_id, name from h_table where id=:id")) {
        DlgMessage::Msg(tr("SQL error"));
        return;
    }
    m_ord->bindValue(":id", tableId);
    if (!m_ord->execSQL()) {
        DlgMessage::Msg(tr("SQL error"));
        return;
    }
    /* Check for empty, if dst order is not empty, merge
        Since 12.02.2017, impossible to move the order on the table,
        which is printed receipt */
    QString dstOrdId;
    QString dstTableName;
    QString dstTableState = tr("Empty");
    QString dstTableItems;
    if (m_ord->next()) {
         dstOrdId = m_ord->v_str(0);
         dstTableName = m_ord->v_str(1);
    }
    if (dstOrdId.length()) {
        /* Check destination table for receipt */
        if (!m_ord->prepare("select print_qty from o_order where id=:id")) {
            DlgMessage::Msg(tr("SQL error"));
            return;
        }
        m_ord->bindValue(":id", dstOrdId);
        if (!m_ord->execSQL()) {
            DlgMessage::Msg(tr("SQL error"));
            return;
        }
        m_ord->next();
        if (m_ord->v_int(0)) {
            m_ord->closeDB();
            DlgMessage::Msg(tr("Cannot to move the order on the table, which is printed receipt."));
            return;
        }
        /* Merge orders */
        dstTableState = tr("Not empty: ") + dstOrdId;
        DbDriver db;
        db.configureDb(__cnfmaindb.fHost, __cnfmaindb.fDatabase, __cnfmaindb.fUser, __cnfmaindb.fPassword);
        if (!db.openDB()) {
            DlgMessage::Msg(tr("Cannot connect to database"));
            return;
        }
        if (!db.prepare(SQL_ODISHES)) {
            DlgMessage::Msg(tr("SQL error"));
            return;
        }
        db.bindValue(":order_id", dstOrdId);
        if (!db.execSQL()) {
            DlgMessage::Msg(tr("SQL error"));
            return;
        }
        while (db.next()) {
            OD_Dish *dish = new OD_Dish();
            dish->loadFromDb(db);
            dish->m_index = m_ord->m_dishes.count();
            m_ord->appendDish2(dish);
            dstTableItems += QString::number(dish->f_id) + ";";
        }
        /* Update destination table status, set to merged */
        if (!db.prepare("update o_order set state_id=:state_id where id=:id")) {
            DlgMessage::Msg(tr("SQL error. Movement error, please check source and destination tables"));
            return;
        }
        db.bindValue(":state_id", ORDER_STATE_MERGE);
        db.bindValue(":id", dstOrdId);
        if (!db.execSQL()) {
            DlgMessage::Msg(tr("SQL error. Movement error, please check source and destination tables"));
            return;
        }
        db.closeDB();
    }
    /* Update table info */
    if (!m_ord->prepare("update h_table set order_id='' where id=:id")) {
        DlgMessage::Msg(tr("SQL error. Movement error, please check source and destination tables"));
        return;
    }
    m_ord->bindValue(":id", m_ord->m_header.f_tableId);
    if (!m_ord->execSQL()) {
        DlgMessage::Msg(tr("SQL error. Movement error, please check source and destination tables"));
        return;
    }
    if (!m_ord->prepare("update h_table set order_id=:order_id where id=:id")) {
        DlgMessage::Msg(tr("SQL error. Movement error, please check source and destination tables"));
        return;
    }
    m_ord->bindValue(":order_id", m_ord->m_header.f_id);
    m_ord->bindValue(":id", m_ord->m_header.f_id);
    if (!m_ord->execSQL()) {
        DlgMessage::Msg(tr("SQL error. Movement error, please check source and destination tables"));
        return;
    }
    m_ord->closeDB();
    int oldTableId = m_ord->m_header.f_tableId;
    m_ord->m_header.f_tableId = tableId;
    m_ord->m_header.f_tableName = m_hallDrv->table(tableId)->name;
    m_ord->countAmounts();
    if (!m_ord->openDB()) {
        DlgMessage::Msg(tr("SQL error. Movement error, please check source and destination tables"));
        return;
    }
    if (!m_ord->saveAll()) {
        DlgMessage::Msg(tr("SQL error. Movement error, please check source and destination tables"));
        return;
    }
    m_hallDrv->unlockTable(oldTableId);
    m_ord->closeDB();
    makeDishesList();
    /* Log */
    if (!m_ord->openDB())
        return;
    QString sql = "insert into o_important (order_id, user_id, action_id, data) values (:order_id, :user_id, :action_id, :data)";
    QString data = QString("%1 > %2;%3;%4").arg(tableName).arg(dstTableName).arg(dstTableState).arg(dstTableItems);
    if (!m_ord->prepare(sql))
        return;
    LogThread::logOrderThread(m_ord->m_header.f_currStaffId, m_ord->m_header.f_id, "Table moved", data);
    m_ord->bindValue(":order_id", m_ord->m_header.f_id);
    m_ord->bindValue(":user_id", m_ord->m_header.f_currStaffId);
    m_ord->bindValue(":action_id", 4);
    m_ord->bindValue(":data", data);
    if (!m_ord->execSQL())
        return;
    m_ord->closeDB();
   /* End log */
}

void dlgorder::insertDiscount()
{
    if (m_ord->m_header.f_printQty > 0) {
        return;
    }
    QString cardCode;
    if (!DlgInput::getString(cardCode, tr("Enter card code"), this))
        return;
    cardCode.replace(";", "").replace("?", "");
    if (fGiftCards.contains(cardCode)) {
        giftCard(cardCode);
        return;
    }
    QMap<QString, QVariant> output;
    FF_DiscountDrv *d = new FF_DiscountDrv();
    bool ok = false;
    if (d->checkCode(m_ord->m_header.f_currStaffId, cardCode, output)) {
        if (!m_ord->discount(output["ID"].toInt(), output["VALUE_DATA"].toDouble()))
            DlgMessage::Msg(tr("Discount failed due program error"));
        m_ord->mfDefaultPriceDec = m_ord->m_header.f_amount_dec_value;
        for (int i = 0; i < m_ord->m_dishes.count(); i++)
            m_ord->m_dishes[i]->setPriceMod(m_ord->mfDefaultPriceInc, m_ord->mfDefaultPriceDec);
        m_ord->countAmounts();
        LogThread::logOrderThread(m_ord->m_header.f_currStaffId, m_ord->m_header.f_id, "Discount", QString("%1, %2%")
                                  .arg(output["ID"].toString())
                                  .arg(output["VALUE_DATA"].toString()));
        ok = true;
    }
    if (!ok) {
        DlgMessage::Msg(tr("Discount failed"));
    }
    delete d;
}

void dlgorder::moveDish()
{
//    int dishIndex = dishIndexFromListWidget();
//    if (dishIndex < 0)
//        return;

//    int tableId = m_orderDrv->m_header.table_id;
//    if (DlgTableForMovement::getTable(tableId, this, m_hallDrv) != QDialog::Accepted)
//        return;

//    if (!m_orderDrv->moveDish(tableId, m_user, dishIndex, m_orderDrv->m_dishes[dishIndex].rqty)) {
//        DlgMessage::Msg(tr("Movement of dish failed"));
//        return;
//    }

//    if (m_orderDrv->m_dishes[dishIndex].state_id != DISH_STATE_NORMAL)
//        delete ui->lstOrder->selectedItems().at(0);
//    else
//        dishRepaint(dishIndex);
}

void dlgorder::beforeClose()
{
    m_hallDrv->setFlag(m_ord->m_header.f_tableId, TFLAG_CALLSTAFF, '0');
    m_hallDrv->setFlag(m_ord->m_header.f_tableId, TFLAG_NEW, '0');
    m_ord->disconnect();
    m_ord->openDB();
    if (!m_flagCashMode) {
        m_ord->m_header.f_dateCash = FF_SettingsDrv::cashDate();
        m_ord->m_header.f_dateClose = QDateTime::currentDateTime();
        m_ord->m_header.m_saved = false;
        if (m_ord->m_header.f_stateId == ORDER_STATE_OPEN) {
            if (!m_ord->m_dishes.count()) {
                m_ord->m_header.f_stateId = ORDER_STATE_EMTPY1;
                /*
                OrderWindowDriver *owd = new OrderWindowDriver(0);
                connect(owd, SIGNAL(discountRemoved()), owd, SLOT(deleteLater()));
                owd->removeDiscountFromApp(FF_SettingsDrv::value(SD_DISCOUNT_APP_QUERY).toString() + m_ord->m_header.f_tableName);
                */
            } else {
                bool empty = true;
                for (int i = 0; i < m_ord->m_dishes.count(); i++) {
                    OD_Dish *d = m_ord->dish(i);
                    if (d->f_stateId == DISH_STATE_NORMAL) {
                        if (d->f_totalQty < 0.01 && d->f_printedQty < 0.01) {
                            d->f_stateId = DISH_STATE_REMOVED_NORMAL;
                            d->m_saved = false;
                            continue;
                        }
                        empty = false;
                        break;
                    }
                }
                if (empty) {
                    m_ord->m_header.f_stateId = ORDER_STATE_EMPTY2;
                    FF_SettingsDrv::value(SD_DISCOUNT_APP_QUERY).toString() + m_ord->m_header.f_tableName;
                }
            }
        }
        m_ord->saveAll();
        m_ord->closeDB();
    }
    m_ord->closeTable();
    delete m_ord;
}

void dlgorder::giftCard(const QString &code)
{
    if (!fGiftCards.contains(code)) {
        message(tr("Invalid gift card"));
        return;
    }
    DbDriver db;
    db.configureDb("10.1.0.2", "maindb", "SYSDBA", "masterkey");
    if (!db.openDB()) {
        message("Cannot connect to main server!");
        return;
    }
    db.prepare("select sum(f_amount) from o_gift_card where f_code=:f_code");
    db.bindValue(":f_code", code);
    db.execSQL();
    if (!db.next()) {
        message(tr("Invalid gift card"));
        return;
    }
    double total = db.v_dbl(0);
    m_ord->prepare("select * from o_gift_card where f_order=:f_order");
    m_ord->bindValue(":f_order", m_ord->m_header.f_id);
    m_ord->execSQL();
    if (m_ord->next()) {
        m_ord->prepare("delete from o_gift_card where f_order=:f_order");
        m_ord->bindValue(":f_order", m_ord->m_header.f_id);
        m_ord->execSQL();
    }
    m_ord->prepare("insert into o_gift_card (f_code, f_order, f_amount) values (:f_code, :f_order, :f_amount)");
    m_ord->bindValue(":f_code", code);
    m_ord->bindValue(":f_order", m_ord->m_header.f_id);
    m_ord->bindValue(":f_amount", total);
    m_ord->execSQL();

    message(QString("%1<br>%2: %3").arg(tr("Gift card")).arg(tr("Available amount")).arg(float_str(total, 2)));
}

void dlgorder::buildDishesView()
{
    //Groups of groups
    QFont f(qApp->font());
    f.setPointSize(f.pointSize());
    f.setBold(true);
    ui->tblGroupOfGroup->setFont(f);
    buildGroupOfGroups();

    //Dishes groups
    ui->tblDishGroup->setFont(f);
    buildTypes(FF_SettingsDrv::value(SD_DEFAULT_MENU_ID).toInt(), 0);

    //Dishes
}

void dlgorder::on_tblGroupOfGroup_clicked(const QModelIndex &index)
{
    if (!index.isValid())
        return;
    int groupId = index.data(Qt::UserRole).toInt();
    buildTypes(m_currentMenu, groupId);
}

void dlgorder::on_btnClearGroupFilter_clicked()
{
    buildTypes(m_currentMenu, 0);
}

QDishTableItemDelegate::QDishTableItemDelegate(const FF_DishesDrv &data) :
    QItemDelegate(),
    m_data(data)
{

}

void QDishTableItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if (!index.isValid()) {
        QItemDelegate::paint(painter, option, index);
        return;
    }

    int dataIndex = index.data(Qt::UserRole).toInt() - 1;
    if (dataIndex < 0)
        return;

    QFont font = qApp->font();
    font.setPointSize(font.pointSize());
    font.setBold(true);
    painter->setFont(font);

    QFontMetrics fm(font);

    QPen pen(Qt::NoPen);
    painter->setPen(pen);

    QBrush brush(Qt::SolidPattern);
    brush.setColor(QColor(m_data.prop(dataIndex, "COLOR").toInt()));
    painter->setBrush(brush);

    QRect rectName = option.rect;
    rectName.adjust(3, 2, 2, 2);

    QTextOption to;

    painter->drawRect(option.rect);

    pen.setStyle(Qt::SolidLine);
    painter->setPen(pen);
    painter->drawText(rectName, m_data.prop(dataIndex, "DISH_NAME").toString(), to);

    QString price = QString::number(m_data.prop(dataIndex, "PRICE").toDouble(), 'f', 0);
    font.setBold(true);
    font.setPointSize(font.pointSize() - 1);
    painter->setFont(font);
    QRect rectPrice = option.rect;
    rectPrice.adjust(rectPrice.width() - fm.width(price) - 5, rectPrice.height() - fm.height(), 2, 2);
    //painter->drawRect(rectPrice);
    painter->drawText(rectPrice, price);
}

void dlgorder::on_tblDishes_clicked(const QModelIndex &index)
{
    if (!index.isValid())
        return;

    int dishIndex = index.data(Qt::UserRole).toInt() - 1;
    if (dishIndex < 0)
        return;

    OD_Dish *dish = new OD_Dish();
    dish->f_id = 0;
    dish->f_stateId = DISH_STATE_NORMAL;
    dish->f_dishId = m_dishesDrv->prop(dishIndex, "DISH_ID").toInt();
    dish->f_dishName = m_dishesDrv->prop(dishIndex, "DISH_NAME").toString();
    dish->f_totalQty = 1;
    dish->f_printedQty = 0;
    dish->f_price = m_dishesDrv->prop(dishIndex, "PRICE").toFloat();
    dish->f_priceInc = m_ord->m_header.f_amount_inc_value;
    dish->f_priceDec = m_ord->m_header.f_amount_dec_value;
    dish->f_storeId = m_dishesDrv->prop(dishIndex, "STORE_ID").toInt();
    dish->f_print1 = m_dishesDrv->prop(dishIndex, "PRINT1").toString();
    dish->f_print2 = m_dishesDrv->prop(dishIndex, "PRINT2").toString();
    dish->f_lastUser = m_user->id;
    dish->f_lastUserName = m_user->fullName;
    dish->f_paymentMod = m_dishesDrv->prop(dishIndex, "PAYMENT_MOD").toInt();
    dish->f_comments = "";
    dish->f_remind = m_dishesDrv->prop(dishIndex, "REMIND").toBool();
    dish->f_adgCode = m_dishesDrv->prop(dishIndex, "ADGCODE").toString();
    dish->setPriceMod(m_ord->mfDefaultPriceInc, m_ord->mfDefaultPriceDec);
    if (FF_SettingsDrv::value(SD_AUTODISCOUNT_V1).toString().length()) {
        QStringList params = FF_SettingsDrv::value(SD_AUTODISCOUNT_V1).toString().split(";", QString::SkipEmptyParts);
        QTime time = QTime::currentTime();
        QTime time1 = QTime::fromString(params.at(0), TIME_FORMAT);
        QTime time2 = QTime::fromString(params.at(1), TIME_FORMAT);
        int store = params.at(2).toInt();
        float value = params.at(3).toFloat();
        if ((time > time1) and (time < time2)) {
            if (dish->f_storeId == store && dish->f_paymentMod == 1)
                dish->f_priceDec = value;
        }
    }
    connect(dish, SIGNAL(removed(int,float)), this, SLOT(printRemovedDish(int,float)));

    int newDishIndex = m_ord->appendDish(dish);
    if (newDishIndex < 0) {
        DlgMessage::Msg(tr("Cannot append dish"));
        return;
    }

    QListWidgetItem *item = new QListWidgetItem(ui->lstOrder);
    item->setSizeHint(QSize(ui->lstOrder->width(), 50));
    ui->lstOrder->addItem(item);
    ui->lstOrder->setItemSelected(item, true);
    item->setData(Qt::UserRole, newDishIndex);
    setButtonsState();

    ui->lstOrder->verticalScrollBar()->setValue(ui->lstOrder->verticalScrollBar()->maximum());
    ui->lstOrder->setCurrentRow(ui->lstOrder->count() - 1);
}


QOrderItemDelegate::QOrderItemDelegate(const QList<OD_Dish *> &data) :
    QItemDelegate(),
    m_data(data)
{

}

void QOrderItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if (!index.isValid()) {
        QItemDelegate::paint(painter, option, index);
        return;
    }

    int dishIndex = index.data(Qt::UserRole).toInt();
    if (dishIndex > m_data.count() - 1)
        return;

    const OD_Dish *dish = m_data[dishIndex];

    painter->save();

    QPen pen(QColor::fromRgb(152, 200, 239));
    painter->setPen(pen);
    QBrush brush(Qt::SolidPattern);
    brush.setColor(Qt::white);
    if (option.state & QStyle::State_Selected)
        brush.setColor(Qt::yellow);
    painter->setBrush(brush);
    painter->drawRect(option.rect);

    int leftLineX = option.rect.right() - 70;
    painter->drawLine(leftLineX, option.rect.top(), leftLineX, option.rect.bottom());
    int middle = option.rect.top() + ((option.rect.bottom() - option.rect.top()) / 2);
    painter->drawLine(option.rect.right() - 70, middle, option.rect.right(), middle);
    int middleLineX = leftLineX + ((option.rect.right() - leftLineX) / 2);
    painter->drawLine(middleLineX, option.rect.top(), middleLineX, middle);

    pen.setColor(Qt::black);
    painter->setPen(pen);
    QTextOption to;
    QFont font = qApp->font();
    font.setBold(false);
    font.setPointSize(font.pointSize() - 1);
    painter->setFont(font);

    QRect nameRect = option.rect;
    nameRect.adjust(3, 2, -36, -2);
    painter->drawText(nameRect, dish->f_dishName, to);

    font.setPointSize(font.pointSize() - 4);
    painter->setFont(font);
    QRect commentRect = option.rect;
    commentRect.adjust(3, commentRect.height() - QFontMetrics(font).height() - 2, -33, -2);
    painter->drawText(commentRect, dish->f_comments, to);

    font.setPointSize(font.pointSize() + 3);
    font.setBold(true);
    painter->setFont(font);
    QRect qtyRect;
    qtyRect.setLeft(leftLineX + 4);
    qtyRect.setTop(option.rect.top() + 2);
    qtyRect.setRight(middleLineX - 2);
    qtyRect.setBottom(middle - 2);
    painter->drawText(qtyRect, dts(dish->f_totalQty), to);

    QRect qtypRect;
    qtypRect.setLeft(middleLineX + 4);
    qtypRect.setTop(option.rect.top() + 2);
    qtypRect.setRight(option.rect.right() - 2);
    qtypRect.setBottom(middle - 2);
    painter->drawText(qtypRect, dts(dish->f_printedQty), to);

    QString amount = dts(dish->f_amount);
    QRect amountRect;
    amountRect.setLeft(leftLineX + 4);
    amountRect.setTop(middle + 2);
    amountRect.setRight(option.rect.right() - 2);
    amountRect.setBottom(option.rect.bottom() - 2);
    painter->drawText(amountRect, amount, to);

    painter->restore();
}

void dlgorder::on_btn1_clicked()
{
    emit qtyClick(1);
}

void dlgorder::on_btn2_clicked()
{
    emit qtyClick(2);
}

void dlgorder::on_btn3_clicked()
{
    emit qtyClick(3);
}

void dlgorder::on_btn4_clicked()
{
    emit qtyClick(4);
}

void dlgorder::on_btn5_clicked()
{
    emit qtyClick(5);
}

void dlgorder::on_btn6_clicked()
{
    emit qtyClick(6);
}

void dlgorder::on_btn7_clicked()
{
    emit qtyClick(7);
}

void dlgorder::on_btn8_clicked()
{
    emit qtyClick(8);
}

void dlgorder::on_btn9_clicked()
{
    emit qtyClick(9);
}

void dlgorder::on_btnDishCancel_clicked()
{
    int i = dishIndexFromListWidget();
    if (i > -1) {
        OD_Dish *d = m_ord->dish(i);
        d->rollbackDecQty();
        d->disconnect();
        connect(d, SIGNAL(update(int)), this, SLOT(dishRepaint(int)));
        connect(d, SIGNAL(removed(int,float)), this, SLOT(printRemovedDish(int,float)));
        connect(this, SIGNAL(qtyClick(float)), d, SLOT(incQty(float)));
    }
    ui->btnDishOk->setEnabled(false);
    ui->btnRmDish->setChecked(false);
    ui->btnDishCancel->setEnabled(false);
    disableWidgetsSet(true);
}

void dlgorder::on_btnDishOk_clicked()
{
    int i = dishIndexFromListWidget();
    if (i > -1) {
        OD_Dish *d = m_ord->dish(i);
        d->commitDecQty();
        d->disconnect();
        if (d->f_stateId != DISH_STATE_NORMAL) {
            int currItemIndex = ui->lstOrder->currentRow();
            if (currItemIndex > -1)
                delete ui->lstOrder->item(currItemIndex);
        } else {
            connect(d, SIGNAL(update(int)), this, SLOT(dishRepaint(int)));
            connect(d, SIGNAL(removed(int,float)), this, SLOT(printRemovedDish(int,float)));
            connect(this, SIGNAL(qtyClick(float)), d, SLOT(incQty(float)));
        }
    }
    ui->btnRmDish->setChecked(false);
    ui->btnDishOk->setEnabled(false);
    ui->btnDishCancel->setEnabled(false);
    disableWidgetsSet(true);
    setButtonsState();
}

void dlgorder::on_btnQtyDot_clicked()
{
    emit qtyClick(0.5);
}

void dlgorder::on_btnPrint_clicked()
{
    m_ord->m_print.printService(FF_SettingsDrv::value(SD_REMINDER).toInt(), m_ord->mfObjectName, m_ord->m_dishes, m_ord->m_header, m_ord->m_dbDrv);
    for (int i = 0; i < ui->lstOrder->count(); i++)
        dishRepaint(i);
    LogThread::logOrderThread(m_ord->m_header.f_currStaffId, m_ord->m_header.f_id, "Print service check", "");

}

void dlgorder::on_btnPayment_clicked()
{
    if (!DlgPayment::payment(m_ord, this)) {
        setButtonsState();
        return;
    }

//        DlgSelectePaymentType *d = new DlgSelectePaymentType(this);
//        m_ord->m_header.f_payment = d->exec();
//        if (m_ord->m_header.f_payment == 2) {
//            m_ord->m_header.f_amountCard = m_ord->m_header.f_amount;
//        }
//        if (m_ord->m_header.f_payment == 3) {
//            //TODO: find payment by jazzve card
//            QString cardCode;
//            if (!DlgInput::getString(cardCode, tr("Enter card code"), this))
//                return;
//            if (cardCode.length() == 0) {
//                return;
//            }
//        }

//        if (FF_SettingsDrv::value(SD_TAX_PRINT).toInt()) {
//            QString orderNum = m_ord->m_header.f_id.right(1);
//            if (FF_SettingsDrv::value(SD_TAX_PRINT_FREQ).toString().contains(orderNum))
//                m_ord->m_print.printTax(FF_SettingsDrv::value(SD_TAX_PRINT_IP).toString(),
//                                          FF_SettingsDrv::value(SD_TAX_PRINT_PORT).toString(),
//                                          FF_SettingsDrv::value(SD_TAX_PRINT_PASS).toString(), m_ord);
//            else if (m_ord->m_header.f_amountCard > 0.01)
//                m_ord->m_print.printTax(FF_SettingsDrv::value(SD_TAX_PRINT_IP).toString(),
//                                          FF_SettingsDrv::value(SD_TAX_PRINT_PORT).toString(),
//                                          FF_SettingsDrv::value(SD_TAX_PRINT_PASS).toString(), m_ord);
//        }

//        m_ord->m_header.f_dateCash = FF_SettingsDrv::cashDate();
//        m_ord->m_header.f_stateId = ORDER_STATE_CLOSED;
        LogThread::logOrderThread(m_ord->m_header.f_currStaffId, m_ord->m_header.f_id, "Close order", "");
        accept();
}

void dlgorder::on_btnDuplicateDish_clicked()
{
    int currDish = dishIndexFromListWidget();
    if (currDish < 0)
        return;

    OD_Dish *dish = m_ord->dish(currDish)->copy();
    dish->f_lastUser = m_ord->m_header.f_currStaffId;
    dish->f_lastUserName = m_ord->m_header.f_currStaffName;
    dish->f_totalQty = 1;
    dish->f_printedQty = 0;
    int newDish = m_ord->appendDish(dish);
    QListWidgetItem *item = new QListWidgetItem(ui->lstOrder);
    item->setSizeHint(QSize(ui->lstOrder->width(), 50));
    item->setData(Qt::UserRole, newDish);
    ui->lstOrder->addItem(item);
    ui->lstOrder->setItemSelected(item, true);
    setButtonsState();

    ui->lstOrder->verticalScrollBar()->setValue(ui->lstOrder->verticalScrollBar()->maximum());
}

void dlgorder::on_btnExit_clicked()
{
    LogThread::logOrderThread(m_ord->m_header.f_currStaffId, m_ord->m_header.f_id, "Quit", "");
    accept();
}

void dlgorder::on_btnRemoveOrder_clicked()
{
    if (!m_ord->openDB())
        return;
    m_ord->prepare("select id, name from o_remove_reason order by name");
    m_ord->execSQL();
    QMap<int, QString> rv;
    while (m_ord->next()) {
        rv[m_ord->v_int(0)] = m_ord->v_str(1);
    }

    int reasonId = 0;
    QString reasonName;

    if (DlgMessage::Msg(tr("Confirm order delete")) == QDialog::Accepted) {
        //m_ord->o_removed(m_orderDrv->m_header.current_staff_id, tr("Order"), m_orderDrv->m_header.id, 1, m_orderDrv->m_header.amount);
        //TODO print removed item
        DlgRemoveReason *dlg = new DlgRemoveReason(rv, this);
        dlg->exec();
        reasonId = dlg->m_reasonId;
        reasonName = dlg->m_reasonName;
        delete dlg;
        for (int i = 0; i < m_ord->m_dishes.count(); i++) {
            OD_Dish *d = m_ord->dish(i);
            if (d->f_stateId == DISH_STATE_NORMAL)
                if (d->f_printedQty > 0.01) {
                    d->f_stateId = DISH_STATE_REMOVED_PRINTED;
                    d->m_saved = false;
                    m_ord->m_print.printRemoved(i, d->f_printedQty, m_ord, reasonName);
                }
        }
        m_ord->m_header.f_stateId = ORDER_STATE_REMOVED;
        m_ord->m_header.f_dateCash = FF_SettingsDrv::cashDate();
        QString sql = "insert into o_important (order_id, user_id, action_id, data, reason_id) values (:order_id, :user_id, :action_id, :data, :reason_id)";
        QString data = "";
        if (!m_ord->openDB())
            return;
        if (!m_ord->prepare(sql))
            return;
        m_ord->bindValue(":order_id", m_ord->m_header.f_id);
        m_ord->bindValue(":user_id", m_ord->m_header.f_currStaffId);
        m_ord->bindValue(":action_id", 2);
        m_ord->bindValue(":data", data);
        m_ord->bindValue(":reason_id", reasonId);
        if (!m_ord->execSQL())
            return;
        m_ord->closeDB();
        LogThread::logOrderThread(m_ord->m_header.f_currStaffId, m_ord->m_header.f_id, "Order canceled", "");
        accept();
    }
}

void dlgorder::on_btnMoveOrder_clicked()
{
    moveTable();
}

void dlgorder::makeDishesList()
{
    ui->lstOrder->clear();
    for (int i = 0; i < m_ord->m_dishes.count(); i++)
        if (m_ord->m_dishes[i]->f_stateId == DISH_STATE_NORMAL) {
            connect(m_ord->m_dishes[i], SIGNAL(removed(int,float)), this, SLOT(printRemovedDish(int,float)));
            QListWidgetItem *item = new QListWidgetItem(ui->lstOrder);
            item->setSizeHint(QSize(ui->lstOrder->width(), 50));
            item->setData(Qt::UserRole, i);
            ui->lstOrder->addItem(item);
        }
}

void dlgorder::buildGroupOfGroups()
{
    ui->tblGroupOfGroup->clearContents();
    ui->tblGroupOfGroup->setColumnCount(2);
    ui->tblGroupOfGroup->setRowCount(4);
    int c = 0, r = 0;
    for (QList<DishesData::GroupOfGroup>::const_iterator it = m_dishesDrv->m_groupOfGroup.begin(); it != m_dishesDrv->m_groupOfGroup.end(); it++) {
        if (m_currentMenu == it->menu_id) {
            QTableWidgetItem *item = new QTableWidgetItem(it->name);
            item->setData(Qt::UserRole, it->id);
            ui->tblGroupOfGroup->setItem(r, c++, item);
            if (c > 1) {
                c = 0;
                r++;
            }
            if (r > 3)
                return;
        }
    }
}

void dlgorder::on_btnDiscount_clicked()
{
    insertDiscount();
}

void dlgorder::on_btnTotalOrders_clicked()
{
    QSqlDrv qd(m_user->fullName, "main");
    if (!qd.prepare("select count(id), sum (amount) from o_order "
                     "where state_id=:state_id and date_cash=:date_cash and staff_id=:staff_id"))
        return;

    qd.m_query->bindValue(":state_id", ORDER_STATE_CLOSED);
    qd.m_query->bindValue(":date_cash", FF_SettingsDrv::cashDate());
    qd.m_query->bindValue(":staff_id", m_user->id);
    if (!qd.execSQL())
        return;

    int qty = 0;
    double amount = 0;
    if (qd.m_query->next()) {
        qty = qd.m_query->value(0).toInt();
        amount = qd.m_query->value(1).toDouble();
    }

    DlgMessage::Msg(QString("%1 / %2").arg(qty).arg(amount));
}

void dlgorder::on_btnChangeStaff_clicked()
{
    QString newStaffPwd;
    if (!DlgGetPassword::password(tr("Password of new staff"), newStaffPwd, true, this))
        return;
    QSqlDrv d(m_user->fullName, "main");

    if (!d.prepare("select id, fname || ' ' || lname as name from employes where password2=:password2")) {
        DlgMessage::Msg(tr("SQL error"));
        return;
    }
    d.bind(":password2", QString(QCryptographicHash::hash(newStaffPwd.toLatin1(), QCryptographicHash::Md5).toHex()));
    if (!d.execSQL()) {
        DlgMessage::Msg(tr("SQL error"));
        return;
    }
    if (!d.m_query->next()) {
        DlgMessage::Msg(tr("Invalid password"));
        return;
    }

    m_ord->m_header.f_staffId = d.m_query->value(0).toInt();
    m_ord->m_header.f_staffName = d.m_query->value(1).toString();
    m_ord->m_header.m_saved = false;

    DlgMessage::Msg(tr("New owner of order") + "\n" + m_ord->m_header.f_staffName);
}

void dlgorder::on_btnMoveDish_clicked()
{
//    int dishIndex = dishIndexFromListWidget();
//    if (dishIndex < 0)
//        return;

//    double qty = m_orderDrv->m_dishes[dishIndex].qty;
//    double qtyPrinted = m_orderDrv->m_dishes[dishIndex].pqty;
//    if (qty > qtyPrinted) {
//        DlgMessage::Msg(tr("Print all quantity"));
//        return;
//    }
//    m_orderDrv->m_dishes[dishIndex].rqty = 0;

//    disableWidgetsSet(false);

//    int btnEnabled = trunc(qty);
//    for (int i = btnEnabled; i < 9; i++)
//        m_btnQtySet.at(i)->setEnabled(false);
}

void dlgorder::on_btnTaxPrint_clicked()
{
    if (DlgMessage::Msg(tr("Confirm to tax checkout")) != QDialog::Accepted)
        return;

    m_ord->m_print.printTax(FF_SettingsDrv::value(SD_TAX_PRINT_IP).toString(),
                              FF_SettingsDrv::value(SD_TAX_PRINT_PORT).toString(),
                              FF_SettingsDrv::value(SD_TAX_PRINT_PASS).toString(), m_ord);
    LogThread::logOrderThread(m_ord->m_header.f_currStaffId, m_ord->m_header.f_id, "Print tax", "");
}

void dlgorder::on_btnDishUp_3_clicked()
{
    ui->lstOrder->verticalScrollBar()->setValue(ui->lstOrder->verticalScrollBar()->value() + 5);
}

void dlgorder::on_btnGroupDown_clicked()
{
    ui->tblDishGroup->verticalScrollBar()->setValue(ui->tblDishGroup->verticalScrollBar()->value() + 3);
}

void dlgorder::on_btnGroupUp_clicked()
{
    ui->tblDishGroup->verticalScrollBar()->setValue(ui->tblDishGroup->verticalScrollBar()->value() - 3);
}

void dlgorder::on_pushButton_clicked()
{
    ui->lstOrder->verticalScrollBar()->setValue(ui->lstOrder->verticalScrollBar()->value() - 5);
}

void dlgorder::on_btnDishDown_clicked()
{
    ui->tblDishes->verticalScrollBar()->setValue(ui->tblDishes->verticalScrollBar()->value() + 5);
}

void dlgorder::on_btnDishUp_clicked()
{
    ui->tblDishes->verticalScrollBar()->setValue(ui->tblDishes->verticalScrollBar()->value() - 5);
}

void dlgorder::on_btnComment_clicked()
{
    int dishIndex = dishIndexFromListWidget();
    if (dishIndex < 0)
        return;

    //QStringList comments = m_orderDrv->m_dishes[dishIndex].comment.split(",");
    DlgDishComment *d = new DlgDishComment(this);
    if (d->exec() == QDialog::Accepted) {
        m_ord->m_dishes[dishIndex]->f_comments = d->result();
        m_ord->m_dishes[dishIndex]->m_saved = false;
        ui->lstOrder->repaint();
    }
    delete d;
}

void dlgorder::on_tblDishGroup_clicked(const QModelIndex &index)
{
    if (!index.isValid())
        return;
    if (!index.data(Qt::UserRole).toInt())
        return;
    buildDishes(index.data(Qt::UserRole).toInt());
}


void dlgorder::on_btn10_clicked()
{
    emit qtyClick(10);
}

void dlgorder::on_btnHistory_clicked()
{
    DlgHistory *d = new DlgHistory(m_ord->m_header.f_id, m_user->fullName, this);
    d->exec();
    delete d;
}

void dlgorder::on_btnChangeMenu_clicked()
{
    QVariant out;
    if (!DlgList::value(m_dishesDrv->getMenuList(), out, this))
        return;
    buildTypes(out.toInt(), 0);
}

void dlgorder::on_lstOrder_currentRowChanged(int currentRow)
{
    if (currentRow < 0) {
        ui->btnRmDish->setEnabled(false);
        return;
    }

    ui->btnRmDish->setEnabled(m_ord->m_header.f_printQty == 0 ? true : m_user->roleRead(ROLE_REMOVE_ORDER_AFTER_CHECKOUT));
}

void dlgorder::on_btnPlugins_clicked()
{
    QMap<QString, QVariant> data;
    QDir d(QSystem::appPath() + "/orderplugins/");
    QStringList files = d.entryList();
    for (QStringList::const_iterator i = files.begin(); i != files.end(); i++) {
        if ((*i) == "." || (*i) == ".." )
            continue;

        QLibrary l(QSystem::appPath() + "/orderplugins/" + (*i));
        if (!l.load())
            continue;

        typedef QString (*caption)();
        caption c = (caption) l.resolve("caption");

        if (!c) {
            l.unload();
            continue;
        }
        data[c()] = QSystem::appPath() + "/orderplugins/" + (*i);
        l.unload();
    }

    QVariant out;
    if (!DlgList::value(data, out, this))
        return;

    QLibrary l(out.toString());
    if (!l.load()) {
        DlgMessage::Msg(tr("Could not load library") + "\n" + out.toString());
        return;
    }
    typedef bool (*exec)(const QMap<QString, QString> &filter, OD_Drv *o, QString &out);
    exec e = (exec) l.resolve("exec");
    if (!e) {
        DlgMessage::Msg(tr("Could not find entry point for") + "\nexec(QWidget *parent, FF_OrderDrv *o, QString &out)");
        l.unload();
        return;
    }
    QString msg;
    QMap<QString, QString> filter;
    typedef bool (*filterDate)();
    filterDate fd = (filterDate)l.resolve("filterDate");
    if (fd) {
        DlgReportFilter *dr = new DlgReportFilter(filter, this);
        if (dr->exec() != QDialog::Accepted) {
            delete dr;
            l.unload();
            return;
        }
    }
    if (!e(filter, m_ord, msg)) {
        DlgMessage::Msg(msg);
    }
    makeDishesList();
    for (int i = 0; i < m_ord->m_dishes.count(); i++)
        if (m_ord->m_dishes[i]->f_stateId == 1)
            m_ord->m_dishes[i]->m_saved = false;
    m_ord->openDB();
    m_ord->saveAll();
    m_ord->closeDB();
    setButtonsState();
    DlgMessage::Msg(tr("Complete") + "\n" + msg);
}

void dlgorder::on_btnComment_2_clicked()
{
    DlgDishComment *d = new DlgDishComment(this);
    if (d->exec() == QDialog::Accepted) {
        m_ord->m_header.f_comment = d->result();
        ui->tblTotal->item(RComment, 0)->setText(m_ord->m_header.f_comment);
    }
    delete d;
}

void dlgorder::on_btnCalculator_clicked()
{
    DlgCalcChange *d = new DlgCalcChange(m_ord->m_header.f_amount, this);
    d->exec();
    delete d;
}

void dlgorder::on_pushButton_2_clicked()
{
    m_drv.checkOnlinePayment(m_ord);
}

void dlgorder::on_lstOrder_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous)
{
    OD_Dish *d;
    if (previous) {
        d = m_ord->dish(previous->data(Qt::UserRole).toInt());
        d->disconnect();
        disconnect(this, SIGNAL(qtyClick(float)), d, SLOT(incQty(float)));
    }

    if (current) {
        d = m_ord->dish(current->data(Qt::UserRole).toInt());
        connect(d, SIGNAL(update(int)), this, SLOT(dishRepaint(int)));
        connect(d, SIGNAL(removed(int,float)), this, SLOT(printRemovedDish(int,float)));
        connect(this, SIGNAL(qtyClick(float)), d, SLOT(incQty(float)));
        if (d->f_price > 0.001)
            ui->btnPresent->setEnabled(FF_DishesDrv::canPresent(d->f_dishId) && m_user->roleRead(ROLE_W_PRESENT));
    }
}

void dlgorder::on_btnRmDish_clicked(bool checked)
{
    int i = ui->lstOrder->currentRow();
    if (i < 0) {
        ui->btnRmDish->setChecked(false);
        return;
    }
    OD_Dish *d = m_ord->dish(dishIndexFromListWidget());
    if (!d) {
        ui->btnRmDish->setChecked(false);
        return;
    }
    d->setOldQty();
    d->disconnect();
    d->m_removePrinted = m_user->roleRead(ROLE_ORDER_REMOVE_PRINTED_QTY);
    disconnect(d);
    disconnect(this, SIGNAL(qtyClick(float)), d, SLOT(incQty(float)));
    connect(this, SIGNAL(qtyClick(float)), d, SLOT(decQty(float)));
    connect(d, SIGNAL(update(int)), this, SLOT(dishRepaint(int)));
    connect(d, SIGNAL(removed(int,float)), this, SLOT(printRemovedDish(int,float)));
    connect(d, SIGNAL(message(QString)), this, SLOT(message(QString)));
    d->decQty(d->f_totalQty - d->availableForRemove());
    ui->btnDishOk->setEnabled(checked);
    ui->btnDishCancel->setEnabled(checked);
    disableWidgetsSet(false);
}


void dlgorder::on_btnPresent_clicked()
{
    int index = dishIndexFromListWidget();
    if (index < 0)
        return;
    if (DlgMessage::Msg(tr("Confirm present")) != QDialog::Accepted)
        return;
    OD_Dish *d = m_ord->dish(index);
    d->f_price = 0.1;
    d->m_saved = false;
    m_ord->countAmounts();
}

void dlgorder::on_btnOrange_clicked()
{
    return;
    QList<int> dishes;
    int flag = 0;
    for (int i = 0; i < m_ord->m_dishes.count(); i++) {
        if (m_ord->dish(i)->f_stateId != DISH_STATE_NORMAL) {
            continue;
        }
        if (m_ord->dish(i)->flag14 > 0) {
            flag = m_ord->dish(i)->flag14;
            dishes.append(m_ord->dish(i)->f_dishId);
        }
    }
    if (Dlg14::set14(dishes, flag)) {
        foreach (int did, dishes) {
            bool exists = false;
            for (int i = 0; i < m_ord->m_dishes.count(); i++) {
                if (m_ord->dish(i)->f_stateId != DISH_STATE_NORMAL) {
                    continue;
                }
                if (did == m_ord->dish(i)->f_dishId) {
                    exists = true;
                }
            }
            if (exists) {
                continue;
            }
            QSqlDrv drv("FFADMIN", "main");
            drv.prepare("select name from me_dishes where id=:id");
            drv.bind(":id", did);
            drv.execSQL();
            drv.next();
            QString name = drv.val().toString();
            drv.close();
            OD_Dish *dish = new OD_Dish();
            dish->f_id = 0;
            dish->f_stateId = DISH_STATE_NORMAL;
            dish->f_dishId = did;
            dish->f_dishName = name;
            dish->f_totalQty = 1;
            dish->f_printedQty = 0;
            dish->f_price = 0;
            dish->f_priceInc = 0;
            dish->f_priceDec = 0;
            dish->f_storeId = 3;
            dish->f_print1 = "kit";
            dish->f_print2 = "local";
            dish->f_lastUser = m_user->id;
            dish->f_lastUserName = m_user->fullName;
            dish->f_paymentMod = 2;
            dish->f_comments = "";
            dish->f_remind = 1;
            dish->f_adgCode = "56.21";
            dish->flag14 = flag;
            dish->setPriceMod(0, 0);
            connect(dish, SIGNAL(removed(int,float)), this, SLOT(printRemovedDish(int,float)));

            int newDishIndex = m_ord->appendDish(dish);
            if (newDishIndex < 0) {
                DlgMessage::Msg(tr("Cannot append dish"));
                return;
            }

            QListWidgetItem *item = new QListWidgetItem(ui->lstOrder);
            item->setSizeHint(QSize(ui->lstOrder->width(), 50));
            ui->lstOrder->addItem(item);
            ui->lstOrder->setItemSelected(item, true);
            item->setData(Qt::UserRole, newDishIndex);
            setButtonsState();

            ui->lstOrder->verticalScrollBar()->setValue(ui->lstOrder->verticalScrollBar()->maximum());
            ui->lstOrder->setCurrentRow(ui->lstOrder->count() - 1);
        }
    }
}

void dlgorder::on_btnCandyCotton_clicked()
{
    SizeMetrics sm(180);
    XmlPrintMaker pm(&sm);

    pm.setFontName("Arial");
    pm.setFontSize(10);
    pm.setFontBold(true);
    int top = 0;

    pm.text(tr("CANDY COTTON"), 0, top);
    top += pm.lastTextHeight();
    pm.text(tr("Order number"), 0, top);
    pm.textRightAlign(m_ord->m_header.f_id, page_width, top);
    top += pm.lastTextHeight() + 1;
    pm.text(tr("Table"), 0, top);
    pm.textRightAlign(m_ord->m_header.f_tableName, page_width, top);
    top += pm.lastTextHeight() + 1;
    pm.text(tr("Staff"), 0, top);
    pm.textRightAlign(m_ord->m_header.f_currStaffName, page_width, top);
    top += pm.lastTextHeight() + 1;
    pm.text(tr("Date"), 0, top);
    pm.textRightAlign(QDateTime::currentDateTime().toString("ddb->MM.yyyy HH:mm:ss"), page_width, top);
    top += pm.lastTextHeight() + 2;
    pm.line(0, top, page_width, top);
    top++;
    pm.line(0, top, page_width, top);
    top += 5;
    pm.line(0, top, page_width, top);
    pm.finishPage();

    ThreadPrinter *tp = new ThreadPrinter("local", sm, pm);
    tp->start();
}

void dlgorder::on_btnDiscount_2_clicked()
{
    QString cardCode;
    if (!DlgInput::getString(cardCode, tr("Enter card code"), this))
        return;
    cardCode.replace(";", "").replace("?", "");
    if (!fGiftCards.contains(cardCode)) {
        message(tr("Invalid gift card"));
        return;
    }
    DbDriver db;
    db.configureDb("10.1.0.2", "maindb", "SYSDBA", "masterkey");
    if (!db.openDB()) {
        message("Cannot connect to main server!");
        return;
    }
    db.prepare("select sum(f_amount) from o_gift_card where f_code=:f_code");
    db.bindValue(":f_code", cardCode);
    db.execSQL();
    if (!db.next()) {
        message(tr("Invalid gift card"));
        return;
    }
    message(tr("Balance:") + "<br>" + db.v_str(0));
}
