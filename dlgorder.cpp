#include "dlgorder.h"
#include "ui_dlgorder.h"
#include "dlgmessage.h"
#include "dlgtableformovement.h"
#include "ff_settingsdrv.h"
#include "dlginput.h"
#include "ff_discountdrv.h"
#include "dlggetpassword.h"
#include "dlgdishcomment.h"
#include "cnfmaindb.h"
#include "dlgreportfilter.h"
#include "dlglist.h"
#include "dlgcorrection.h"
#include "mdefined.h"
#include "dlgpayment.h"
#include "dlgqty.h"
#include "dlgcalcchange.h"
#include "cnfmaindb.h"
#include "htable.h"
#include "logthread.h"
#include "dlgcardtips.h"
#include <QException>
#include <QScrollBar>
#include <QDir>
#include <QLibrary>
#include <QPainter>
#include <QInputDialog>
#include <QCryptographicHash>

#define total_row_count 8

#define RTableName 0
#define RStaffName 1
#define ROrderId 2
#define RCounted 3
#define RService 4
#define RDiscount 5
#define RTotal 6
#define RComment 7

static QStringList fGiftCards;

dlgorder::dlgorder(QWidget *parent) :
    FastfDialog(parent),
    ui(new Ui::dlgorder)
{
    ui->setupUi(this);
    ui->tblTotal->setColumnWidth(0, 150);
    ui->tblTotal->setRowCount(total_row_count);
    ui->tblTotal->setMaximumHeight((total_row_count * ui->tblTotal->verticalHeader()->defaultSectionSize()) + 5);

    for(int i = 0; i < total_row_count; i++)
        for(int j = 0; j < 2; j++)
            ui->tblTotal->setItem(i, j, new QTableWidgetItem());

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
    connect(&mTimer, &QTimer::timeout, this, [this]() {
        ui->leCmd->setFocus();
    });
    mTimer.start(1500);
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
    ui->wdtDishQty->setEnabled(false);
    ui->lstOrder->setEnabled(false);
    setButtonsState();
}

bool dlgorder::setData(FF_User *user, FF_HallDrv *hallDrv, int tableId, QString orderId)
{
    qApp->processEvents();
    m_hallDrv = hallDrv;
    m_user = user;
    bool isClosed = false;

    if(!tableId) {
        isClosed = true;
        m_ord->openDB();
        m_ord->prepare("select table_id from o_order where id=:id");
        m_ord->bindValue(":id", orderId);
        m_ord->execSQL();

        if(m_ord->next())
            tableId = m_ord->v_int(0);

        m_ord->closeDB();
    }

    if(!tableId) {
        DlgMessage::Msg(tr("Program error"));
        return false;
    }

    double defaultPriceInc;

    if(FF_SettingsDrv::tableOption1(tableId, defaultPriceInc))
        m_ord->mfDefaultPriceInc = defaultPriceInc;

    connect(m_ord, SIGNAL(dishRepaint(int)), this, SLOT(dishRepaint(int)));
    m_ord->m_header.f_currStaffId = user->id;
    m_ord->m_header.f_currStaffName = user->fullName;
    m_ord->m_header.f_tableId = tableId;

    if(hallDrv->table(tableId)) {
        m_ord->m_header.f_tableName = hallDrv->table(tableId)->name;
    }

    m_ord->m_header.f_amount_inc_value = m_ord->mfDefaultPriceInc;
    m_ord->m_header.f_amount = 0.0;
    m_ord->m_header.f_amount_inc = 0.0;
    m_ord->m_header.f_amount_dec = 0.0;
    m_ord->m_header.f_amount_dec_value = 0.0;
    m_ord->m_header.f_dateCash = DbDriver::serverDate();
    m_ord->m_header.f_comment = "";
    m_ord->m_header.f_taxPrint = 0;
    m_ord->m_header.f_payment = 0;

    if(!m_ord->openTable(orderId, isClosed, user->id))
        return false;

    LogThread::logOrderThread(m_ord->m_header.f_currStaffName, m_ord->m_header.f_id, "", tr("Open table "), m_ord->m_header.f_tableName);
    connect(m_ord, SIGNAL(counted(QMap<QString, QString>)), this, SLOT(orderCounted(QMap<QString, QString>)));
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

    if(m_user->id != m_ord->m_header.f_staffId)
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
    ui->btnRequestOrderRemove->setEnabled(m_user->roleRead(ROLE_ORDER_CHANGE_STAFF));
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

void dlgorder::toLockError(const QString &msg)
{
    sender()->deleteLater();
    DlgMessage::Msg(msg);
}

void dlgorder::toTableLockedMove(int index, const QString &tableName, int tableId)
{
    moveOrderDish(index, tableId, tableName);
}

void dlgorder::message(const QString &msg)
{
    DlgMessage::Msg(msg);
}

void dlgorder::orderCounted(const QMap<QString, QString>& values)
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

    for(int i = 0; i < ui->lstOrder->count(); i++) {
        QModelIndex item = ui->lstOrder->model()->index(i, 0);

        if(item.data(Qt::UserRole).toInt() == dishIndex) {
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

    for(QMap<QString, int>::const_iterator it = m_dishesDrv->m_proxyTypes.begin(); it != m_dishesDrv->m_proxyTypes.end(); it++) {
        QTableWidgetItem *i = new QTableWidgetItem(it.key());
        i->setData(Qt::UserRole, it.value());
        ui->tblDishGroup->setItem(r, c++, i);

        if(c >= ui->tblDishGroup->columnCount()) {
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

    for(int i = 0; i < colCount; i++)
        ui->tblDishes->setColumnWidth(i, ui->tblDishes->horizontalHeader()->defaultSectionSize() + colDelta);

    if(!typeId)
        return;

    m_dishesDrv->filterDishes(typeId);
    rowCount = m_dishesDrv->m_proxyDishes.count() / colCount;
    ui->tblDishes->setRowCount(rowCount);

    if(m_dishesDrv->m_proxyDishes.count() % colCount)
        rowCount++;

    ui->tblDishes->setRowCount(rowCount);
    int r = 0, c = 0;

    for(QMap<QString, int>::const_iterator it = m_dishesDrv->m_proxyDishes.begin(); it != m_dishesDrv->m_proxyDishes.end(); it++) {
        QTableWidgetItem *i = new QTableWidgetItem(it.key());
        i->setData(Qt::UserRole, it.value() + 1);
        ui->tblDishes->setItem(r, c, i);
        c++;

        if(c == ui->tblDishes->columnCount()) {
            c = 0;
            r++;
        }
    }
}

int dlgorder::dishIndexFromListWidget()
{
    int row = ui->lstOrder->currentRow();

    if(row < 0) {
        return -1;
    }

    return ui->lstOrder->item(row)->data(Qt::UserRole).toInt();
}

int dlgorder::listIndexOfDish(OD_Dish *d)
{
    for(int i = 0; i < ui->lstOrder->count(); i++) {
        if(d == ui->lstOrder->item(i)->data(Qt::UserRole).value<OD_Dish*>()) {
            return i;
        }
    }

    return -1;
}

OD_Dish* dlgorder::dishFromListWidget()
{
    int index = dishIndexFromListWidget();

    if(index < 0) {
        return nullptr;
    }

    return m_ord->dish(index);
}

void dlgorder::setButtonsState()
{
    ui->lstOrder->viewport()->update();
    bool btnPrint = false;
    bool btnPay = true;
    bool btnRemoveOrder = true;
    bool btnMoveOrder = true;
    bool btnDiscount = true;
    bool btnMoveDish = true;
    bool btnRemoveDish = true;

    for(int i = 0 ; i < m_ord->m_dishes.count(); i++) {
        if(m_ord->dish(i)->f_stateId != DISH_STATE_NORMAL)
            continue;

        if(m_ord->dish(i)->f_totalQty > m_ord->dish(i)->f_printedQty) {
            btnPrint = true;
            btnPay = false;
            break;
        }
    }

    if(btnPrint)
        if(FF_SettingsDrv::value(SD_SERIVCE_CHECK_PAY) == 1) {
            btnPay = true;
            btnPrint = false;
        }

    btnPrint = btnPrint && !m_flagCashMode;
    btnRemoveOrder = m_user->roleRead(ROLE_REMOVE_ORDER);

    if(m_flagCashMode)
        btnRemoveOrder = btnRemoveOrder && m_user->roleRead(ROLE_REMOVE_ORDER_FROM_CASH);

    if(m_ord->m_header.f_printQty > 0) {
        btnRemoveOrder = btnRemoveOrder && m_user->roleRead(ROLE_REMOVE_ORDER_AFTER_CHECKOUT);
        //btnRemoveDish &= m_user->roleRead(ROLE_REMOVE_ORDER_AFTER_CHECKOUT);
        btnRemoveDish &= !m_flagCashMode;
    }

    btnRemoveDish &= m_user->roleRead(ROLE_ORDER_REMOVE_PRINTED_QTY);
    btnMoveOrder = m_ord->m_header.f_amount > 0.01;
    btnMoveOrder = btnMoveOrder && !m_flagCashMode && m_user->roleRead(ROLE_MOVE_ORDER);

    if(m_ord->m_header.f_printQty > 0)
        btnMoveOrder = btnMoveOrder && m_user->roleRead(ROLE_REMOVE_ORDER_AFTER_CHECKOUT);

    btnMoveDish = m_user->roleRead(ROLE_MOVE_DISH);
    btnMoveDish = btnMoveDish && !m_flagCashMode && !(m_ord->m_header.f_printQty > 0) && !m_user->roleRead(ROLE_REMOVE_ORDER_AFTER_CHECKOUT);
    btnDiscount = btnDiscount && (m_ord->m_header.f_amount_dec_value < 0.01);

    if(m_ord->m_header.f_printQty > 0) {
        btnDiscount &= m_user->roleRead(ROLE_ORDER_DISCOUNT_AFTER_CHECKOUT);
    }

    btnDiscount = btnDiscount && !m_flagCashMode;
    ui->btnTotalOrders->setEnabled(m_user->roleRead(ROLE_W_VIEW_SALES_AMOUNT));
    ui->btnPrint->setEnabled(btnPrint);
    ui->btnPayment->setEnabled(btnPay);
    ui->btnMoveOrder->setEnabled(btnMoveOrder);
    ui->btnMoveDish->setEnabled(btnMoveDish);
    ui->btnDiscount->setEnabled(btnDiscount);

    if(m_ord->m_header.f_printQty > 0)
        ui->tblDishes->setEnabled(m_user->roleRead(ROLE_REMOVE_ORDER) && m_user->roleRead(ROLE_REMOVE_ORDER_AFTER_CHECKOUT));

    ui->wdtOrderService->setEnabled(m_ord->m_header.f_requestremoveal != 1);
    bool enableControls = !(m_ord->m_header.f_printQty > 0) && !m_flagCashMode && !m_user->roleRead(ROLE_REMOVE_ORDER_AFTER_CHECKOUT);
    enableControls = enableControls && (m_ord->m_header.f_requestremoveal != 1);
    ui->wdtDishQty->setEnabled(enableControls);
    ui->tblDishes->setEnabled(enableControls);
    ui->wdtDishBtn->setEnabled(enableControls && !m_flagCashMode);
    ui->wdtDishGroup->setEnabled(enableControls);
    ui->btnPlugins->setEnabled(m_user->roleRead(ROLE_W_SPESIAL_ACTIONS));
    ui->wdtHdm->setEnabled(m_ord->m_header.f_requestremoveal != 1);
    ui->btnRmDish->setEnabled(btnRemoveDish);

    if(m_ord->m_header.f_printQty > 0) {
        ui->wdtDishQty->setEnabled(false);
        ui->btnRmDish->setEnabled(false);
    }

    ui->btnPayment->setEnabled(ui->btnPayment->isEnabled() && m_ord->m_header.f_requestremoveal != 1);
}

void dlgorder::moveDish(int index, int dtid, const QString &dtname)
{
    int tableId = m_ord->m_header.f_tableId;
    QString tableName = m_ord->m_header.f_tableName;

    if(dtid == 0) {
        if(DlgTableForMovement::getTable(tableId, this, m_hallDrv) != QDialog::Accepted) {
            return;
        }

        dtid = tableId;
        HTable t;

        if(!t.tryLock(tableId)) {
            message(tr("Could not lock table"));
            return;
        }

        toTableLockedMove(index, tableName, dtid);
        t.tryLock(dtid);
    } else {
        tableId = dtid;
        tableName = dtname;
        moveOrderDish(index, tableId, tableName);
    }
}

void dlgorder::insertDiscount()
{
    if(m_ord->m_header.f_printQty > 0) {
        return;
    }

    QString cardCode;

    if(!DlgInput::getString(cardCode, tr("Enter card code"), this))
        return;

    cardCode.replace(";", "").replace("?", "");

    if(fGiftCards.contains(cardCode)) {
        giftCard(cardCode);
        return;
    }

    QMap<QString, QVariant> output;
    FF_DiscountDrv *d = new FF_DiscountDrv();
    bool ok = false;

    if(d->checkCode(m_ord->m_header.f_currStaffId, cardCode, output)) {
        if(!m_ord->discount(output["ID"].toInt(), output["VALUE_DATA"].toDouble()))
            DlgMessage::Msg(tr("Discount failed due program error"));

        m_ord->mfDefaultPriceDec = m_ord->m_header.f_amount_dec_value;

        for(int i = 0; i < m_ord->m_dishes.count(); i++)
            m_ord->m_dishes[i]->setPriceMod(m_ord->mfDefaultPriceInc, m_ord->mfDefaultPriceDec);

        m_ord->countAmounts();
        LogThread::logOrderThread(m_ord->m_header.f_currStaffName, m_ord->m_header.f_id, "", tr("Discount"), QString("%1, %2%")
                                  .arg(output["ID"].toString())
                                  .arg(output["VALUE_DATA"].toString()));
        ok = true;
    }

    if(!ok) {
        DlgMessage::Msg(tr("Discount failed"));
    }

    delete d;
}

void dlgorder::beforeClose()
{
    m_hallDrv->setFlag(m_ord->m_header.f_tableId, TFLAG_NEW, '0');
    m_ord->disconnect();
    m_ord->openDB();

    if(!m_flagCashMode) {
        m_ord->m_header.f_dateCash = FF_SettingsDrv::cashDate();
        m_ord->m_header.f_dateClose = QDateTime::currentDateTime();
        m_ord->m_header.m_saved = false;

        if(m_ord->m_header.f_stateId == ORDER_STATE_OPEN) {
            if(!m_ord->m_dishes.count()) {
                m_ord->m_header.f_stateId = ORDER_STATE_EMTPY1;
            } else {
                bool empty = true;

                for(int i = 0; i < m_ord->m_dishes.count(); i++) {
                    OD_Dish *d = m_ord->dish(i);

                    if(d->f_stateId == DISH_STATE_NORMAL) {
                        if(d->f_totalQty < 0.01 && d->f_printedQty < 0.01) {
                            d->f_stateId = DISH_STATE_REMOVED_NORMAL;
                            d->m_saved = false;
                            continue;
                        }

                        empty = false;
                        break;
                    }
                }

                if(empty) {
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
    if(!fGiftCards.contains(code)) {
        message(tr("Invalid gift card"));
        return;
    }

    DbDriver db;
    db.configureDb("10.1.0.2", "maindb", "SYSDBA", "masterkey");

    if(!db.openDB()) {
        message("Cannot connect to main server!");
        return;
    }

    db.prepare("select sum(f_amount) from o_gift_card where f_code=:f_code");
    db.bindValue(":f_code", code);
    db.execSQL();

    if(!db.next()) {
        message(tr("Invalid gift card"));
        return;
    }

    double total = db.v_dbl(0);
    m_ord->prepare("select * from o_gift_card where f_order=:f_order");
    m_ord->bindValue(":f_order", m_ord->m_header.f_id);
    m_ord->execSQL();

    if(m_ord->next()) {
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

void dlgorder::moveOrderDish(int index, int tableId, QString tableName)
{
    if(tableId == 0) {
        msg("Program error dlgorder::moveOrderDish. Table id = 0");
        return;
    }

    if(!m_ord->openDB()) {
        DlgMessage::Msg(tr("Connection error"));
        return;
    }

    if(!m_ord->prepare("select order_id, name from h_table where id=:id")) {
        DlgMessage::Msg(tr("SQL error"));
        return;
    }

    m_ord->bindValue(":id", tableId);

    if(!m_ord->execSQL()) {
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

    if(m_ord->next()) {
        dstOrdId = m_ord->v_str(0);
        dstTableName = m_ord->v_str(1);
    }

    if(dstOrdId.length()) {
        /* Check destination table for receipt */
        m_ord->prepare("select print_qty from o_order where id=:id");
        m_ord->bindValue(":id", dstOrdId);

        if(!m_ord->execSQL()) {
            DlgMessage::Msg(tr("SQL error"));
            return;
        }

        if(m_ord->next() && m_ord->v_int(0)) {
            m_ord->closeDB();
            DlgMessage::Msg(tr("Cannot to move the order on the table, which is printed receipt."));
            return;
        }

        /* Merge orders */
        dstTableState = tr("Not empty: ") + dstOrdId;
        m_ord->prepare("update o_dishes set order_id=:order_id, moved_from=:moved_from where id=:id");

        if(index < 0) {
            for(int i = ui->lstOrder->count() - 1; i > -1; i--) {
                int idx = ui->lstOrder->item(i)->data(Qt::UserRole).toInt();
                OD_Dish *d = m_ord->dish(idx);
                m_ord->bindValue(":order_id", dstOrdId);
                m_ord->bindValue(":moved_from", m_ord->m_header.f_id);
                m_ord->bindValue(":id", d->f_id);
                m_ord->execSQL();
                delete ui->lstOrder->item(i);
                m_ord->m_dishes.removeAt(idx);
                LogThread::logOrderThread(m_ord->m_header.f_currStaffName, m_ord->m_header.f_id, QString::number(d->f_id), tr("Move dish"), dstOrdId);
                LogThread::logOrderThread(m_ord->m_header.f_currStaffName, dstOrdId, QString::number(d->f_id), tr("Move dish"), m_ord->m_header.f_id);
            }
        } else {
            int idx = ui->lstOrder->item(index)->data(Qt::UserRole).toInt();
            OD_Dish *d = m_ord->dish(idx);
            m_ord->bindValue(":order_id", dstOrdId);
            m_ord->bindValue(":moved_from", m_ord->m_header.f_id);
            m_ord->bindValue(":id", d->f_id);
            m_ord->execSQL();
            delete ui->lstOrder->item(index);
            m_ord->m_dishes.removeAt(idx);
            LogThread::logOrderThread(m_ord->m_header.f_currStaffName, m_ord->m_header.f_id, QString::number(d->f_id), tr("Move dish"), dstOrdId);
            LogThread::logOrderThread(m_ord->m_header.f_currStaffName, dstOrdId, QString::number(d->f_id), tr("Move dish"), m_ord->m_header.f_id);
        }

        /* Update destination table status, set to merged or no */
        if(ui->lstOrder->count() == 0) {
            m_ord->prepare("update o_order set state_id=:state_id where id=:id");
            m_ord->bindValue(":state_id", ORDER_STATE_MERGE);
            m_ord->bindValue(":id", m_ord->m_header.f_id);

            if(!m_ord->execSQL()) {
                DlgMessage::Msg(tr("SQL error. Movement error, please check source and destination tables"));
                return;
            }

            m_ord->prepare("update h_table set order_id='' where id=:id");
            m_ord->bindValue(":id", m_ord->m_header.f_tableId);

            if(!m_ord->execSQL()) {
                DlgMessage::Msg(tr("SQL error. Movement error, please check source and destination tables"));
                return;
            }
        }

        m_ord->prepare("update o_order set amount = (select sum(qty*price) from o_dishes where state_id=1 and order_id=:order_id) where id=:id");
        m_ord->bindValue(":order_id", dstOrdId);
        m_ord->bindValue(":id", dstOrdId);
        m_ord->execSQL();
        m_ord->prepare("update o_order set amount_inc = amount * amount_inc_value where id=:id");
        m_ord->bindValue(":id", dstOrdId);
        m_ord->execSQL();
        m_ord->prepare("update o_order set amount = amount + amount_inc where id=:id");
        m_ord->bindValue(":id", dstOrdId);
        m_ord->execSQL();
        m_ord->closeDB();

        if(ui->lstOrder->count() > 0) {
            makeDishesList();
            m_ord->countAmounts();
        } else {
            on_btnExit_clicked();
            return;
        }
    } else {
        //Create new order and repeat this function
        int new_id = m_ord->m_dbDrv.genId("GEN_O_ORDER_ID");

        if(!new_id) {
            DlgMessage::Msg(tr("SQL error. Movement error, please check source and destination tables"));
            return;
        }

        QString newStrId = QString("%1%2").arg(m_ord->mfOrderIdPrefix).arg(new_id);
        m_ord->prepare("insert into o_order (id) values (:id)");
        m_ord->bindValue(":id", newStrId);

        if(!m_ord->execSQL()) {
            DlgMessage::Msg(tr("SQL error. Movement error, please check source and destination tables"));
            return;
        }

        m_ord->prepare("update o_order set state_id=:state_id, table_id=:table_id, "
                       "date_open=:date_open, date_close=:date_close, "
                       "date_cash=:date_cash, staff_id=:staff_id, print_qty=:print_qty, "
                       "amount=:amount, amount_inc=:amount_inc, amount_dec=:amount_dec, "
                       "amount_inc_value=:amount_inc_value, amount_dec_value=:amount_dec_value, payment=:payment, taxprint=:taxprint, "
                       "comment=:comment where id=:id");
        m_ord->bindValue(":state_id", ORDER_STATE_OPEN);
        m_ord->bindValue(":table_id", tableId);
        m_ord->bindValue(":date_open", QDateTime::currentDateTime());
        m_ord->bindValue(":date_close", QDateTime::currentDateTime());
        m_ord->bindValue(":date_cash", DbDriver::serverDate());
        m_ord->bindValue(":staff_id", m_ord->m_header.f_staffId);
        m_ord->bindValue(":print_qty", 0);
        m_ord->bindValue(":payment", 0);
        m_ord->bindValue(":taxprint", 0);
        m_ord->bindValue(":amount", 0);
        m_ord->bindValue(":amount_inc", 0);
        m_ord->bindValue(":amount_dec", 0);
        m_ord->bindValue(":amount_inc_value", m_ord->m_header.f_amount_inc_value);
        m_ord->bindValue(":amount_dec_value", m_ord->m_header.f_amount_dec_value);
        m_ord->bindValue(":comment", "");
        m_ord->bindValue(":id", newStrId);

        if(!m_ord->execSQL()) {
            DlgMessage::Msg(tr("SQL error. Movement error, please check source and destination tables"));
            return;
        }

        m_ord->prepare("update h_table set order_id=:order_id where id=:id");
        m_ord->bindValue(":order_id", newStrId);
        m_ord->bindValue(":id", tableId);

        if(!m_ord->execSQL()) {
            DlgMessage::Msg(tr("SQL error. Movement error, please check source and destination tables"));
            return;
        }

        m_ord->closeDB();
        moveOrderDish(index, tableId, tableName);
    }
}

int dlgorder::addNewDish(int dishIndex)
{
    if(m_dishesDrv->prop(dishIndex, "F_ADDBYMANAGER").toInt() > 0) {
        if(!m_user->roleRead(ROLE_W_PRESENT)) {
            message(tr("Access denied!"));
            return -1;
        }
    }

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
    dish->f_remind = m_dishesDrv->prop(dishIndex, "REMIND").toInt();
    dish->f_adgCode = m_dishesDrv->prop(dishIndex, "ADGCODE").toString();
    dish->setPriceMod(m_ord->mfDefaultPriceInc, m_ord->mfDefaultPriceDec);

    if(FF_SettingsDrv::value(SD_AUTODISCOUNT_V1).toString().length()) {
        QStringList params = FF_SettingsDrv::value(SD_AUTODISCOUNT_V1).toString().split(";", Qt::SkipEmptyParts);
        QTime time = QTime::currentTime();
        QTime time1 = QTime::fromString(params.at(0), TIME_FORMAT);
        QTime time2 = QTime::fromString(params.at(1), TIME_FORMAT);
        int store = params.at(2).toInt();
        float value = params.at(3).toFloat();

        if((time > time1) && (time < time2)) {
            if(dish->f_storeId == store && dish->f_paymentMod == 1)
                dish->f_priceDec = value;
        }
    }

    int newDishIndex = m_ord->appendDish(dish);

    if(newDishIndex < 0) {
        DlgMessage::Msg(tr("Cannot append dish"));
        return -1;
    }

    LogThread::logOrderThread(m_ord->m_header.f_currStaffName, m_ord->m_header.f_id, QString::number(dish->f_id), tr("New dish "), dish->f_dishName + ": 1");
    QListWidgetItem *item = new QListWidgetItem(ui->lstOrder);
    item->setSizeHint(QSize(ui->lstOrder->width(), 50));
    ui->lstOrder->addItem(item);
    ui->lstOrder->setCurrentItem(item);
    item->setData(Qt::UserRole, newDishIndex);
    setButtonsState();
    ui->lstOrder->verticalScrollBar()->setValue(ui->lstOrder->verticalScrollBar()->maximum());
    ui->lstOrder->setCurrentRow(ui->lstOrder->count() - 1);
    return newDishIndex;
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
    if(!index.isValid())
        return;

    int groupId = index.data(Qt::UserRole).toInt();
    buildTypes(m_currentMenu, groupId);
}

QDishTableItemDelegate::QDishTableItemDelegate(const FF_DishesDrv &data) :
    QItemDelegate(),
    m_data(data)
{
}

void QDishTableItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if(!index.isValid()) {
        QItemDelegate::paint(painter, option, index);
        return;
    }

    int dataIndex = index.data(Qt::UserRole).toInt() - 1;

    if(dataIndex < 0)
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

    if(m_data.prop(dataIndex, "CANCELREQUEST").toInt() > 0) {
        brush.setColor(QColor::fromRgbF(200, 200, 200, 1));
    }

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
    rectPrice.adjust(rectPrice.width() - fm.horizontalAdvance(price) - 5, rectPrice.height() - fm.height(), 2, 2);
    //painter->drawRect(rectPrice);
    painter->drawText(rectPrice, price);
}

void dlgorder::on_tblDishes_clicked(const QModelIndex &index)
{
    if(!index.isValid())
        return;

    int dishIndex = index.data(Qt::UserRole).toInt() - 1;

    if(dishIndex < 0)
        return;

    if(m_dishesDrv->prop(dishIndex, "QR").toInt() > 0) {
        msg(tr("Available only with Emark code"));
        return;
    }

    if(m_dishesDrv->prop(dishIndex, "F_ADDBYMANAGER").toInt() > 0) {
        if(!m_user->roleRead(ROLE_W_PRESENT)) {
            msg(tr("Available for manager"));
            return;
        }
    }

    addNewDish(dishIndex);
}

QOrderItemDelegate::QOrderItemDelegate(const QList<OD_Dish*>& data) :
    QItemDelegate(),
    m_data(data)
{
}

void QOrderItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if(!index.isValid()) {
        QItemDelegate::paint(painter, option, index);
        return;
    }

    int dishIndex = index.data(Qt::UserRole).toInt();

    if(dishIndex > m_data.count() - 1)
        return;

    const OD_Dish *dish = m_data[dishIndex];
    painter->save();
    QPen pen(QColor::fromRgb(152, 200, 239));
    painter->setPen(pen);
    QBrush brush(Qt::SolidPattern);
    brush.setColor(Qt::white);

    if(option.state & QStyle::State_Selected)
        brush.setColor(Qt::yellow);

    if(dish->f_cancelrequest > 0) {
        brush.setColor(QColor::fromRgb(200, 100, 100));
    }

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

    if(dish->f_stateId != DISH_STATE_NORMAL) {
        font.setStrikeOut(true);
    }

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
    QRect emarkRect = option.rect;
    emarkRect.adjust(emarkRect.right() - 116, 10, -72, -10);

    if(dish->f_emarks.isEmpty() == false) {
        painter->drawImage(emarkRect, QImage(":/res/qr_code.png"));
    }

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
    setQty(1);
}

void dlgorder::on_btn2_clicked()
{
    setQty(2);
}

void dlgorder::on_btn3_clicked()
{
    setQty(3);
}

void dlgorder::on_btn4_clicked()
{
    setQty(4);
}

void dlgorder::on_btnPrint_clicked()
{
    m_ord->m_print.printService(FF_SettingsDrv::value(SD_REMINDER).toInt(), m_ord->mfObjectName, m_ord->m_dishes, m_ord->m_header, m_ord->m_dbDrv);

    for(int i = 0; i < ui->lstOrder->count(); i++)
        dishRepaint(i);

    LogThread::logOrderThread(m_ord->m_header.f_currStaffName, m_ord->m_header.f_id, "", tr("Print service check"), "");
}

void dlgorder::on_btnPayment_clicked()
{
    if(!DlgPayment::payment(m_user, m_ord, this)) {
        setButtonsState();
        return;
    }

    LogThread::logOrderThread(m_ord->m_header.f_currStaffName, m_ord->m_header.f_id, "", tr("Close order"), "");
    accept();
}

void dlgorder::on_btnDuplicateDish_clicked()
{
    int currDish = dishIndexFromListWidget();

    if(currDish < 0)
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
    ui->lstOrder->setCurrentItem(item);
    setButtonsState();
    ui->lstOrder->verticalScrollBar()->setValue(ui->lstOrder->verticalScrollBar()->maximum());
}

void dlgorder::on_btnExit_clicked()
{
    LogThread::logOrderThread(m_ord->m_header.f_currStaffName, m_ord->m_header.f_id, "", tr("Quit"), "");
    accept();
}

void dlgorder::on_btnMoveOrder_clicked()
{
    if(m_ord->m_header.f_amount_dec_value > 0.001) {
        message(tr("Cannot move the discounted order"));
        return;
    }

    moveDish(-1, 0, "");
}

void dlgorder::makeDishesList()
{
    ui->lstOrder->clear();

    for(int i = 0; i < m_ord->m_dishes.count(); i++) {
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

    for(QList<DishesData::GroupOfGroup>::const_iterator it = m_dishesDrv->m_groupOfGroup.begin(); it != m_dishesDrv->m_groupOfGroup.end(); it++) {
        if(m_currentMenu == it->menu_id) {
            QTableWidgetItem *item = new QTableWidgetItem(it->name);
            item->setData(Qt::UserRole, it->id);
            ui->tblGroupOfGroup->setItem(r, c++, item);

            if(c > 1) {
                c = 0;
                r++;
            }

            if(r > 3)
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

    if(!qd.prepare("select count(id), sum (amount) from o_order "
                   "where state_id=:state_id and date_cash=:date_cash and staff_id=:staff_id"))
        return;

    qd.m_query->bindValue(":state_id", ORDER_STATE_CLOSED);
    qd.m_query->bindValue(":date_cash", FF_SettingsDrv::cashDate());
    qd.m_query->bindValue(":staff_id", m_user->id);

    if(!qd.execSQL())
        return;

    int qty = 0;
    double amount = 0;

    if(qd.m_query->next()) {
        qty = qd.m_query->value(0).toInt();
        amount = qd.m_query->value(1).toDouble();
    }

    DlgMessage::Msg(QString("%1 / %2").arg(qty).arg(amount));
}

void dlgorder::on_btnChangeStaff_clicked()
{
    QString newStaffPwd;

    if(!DlgGetPassword::password(tr("Password of new staff"), newStaffPwd, true, this))
        return;

    QSqlDrv d(m_user->fullName, "main");

    if(!d.prepare("select id, fname || ' ' || lname as name from employes where password2=:password2")) {
        DlgMessage::Msg(tr("SQL error"));
        return;
    }

    d.bind(":password2", QString(QCryptographicHash::hash(newStaffPwd.toLatin1(), QCryptographicHash::Md5).toHex()));

    if(!d.execSQL()) {
        DlgMessage::Msg(tr("SQL error"));
        return;
    }

    if(!d.m_query->next()) {
        DlgMessage::Msg(tr("Invalid password"));
        return;
    }

    m_ord->m_header.f_staffId = d.m_query->value(0).toInt();
    m_ord->m_header.f_staffName = d.m_query->value(1).toString();
    m_ord->m_header.m_saved = false;
    m_ord->saveAll();
    DlgMessage::Msg(tr("New owner of order") + "\n" + m_ord->m_header.f_staffName);
}

void dlgorder::on_btnMoveDish_clicked()
{
    if(m_ord->m_header.f_amount_dec_value > 0.001) {
        message(tr("Cannot move the discounted order"));
        return;
    }

    int i = ui->lstOrder->currentRow();

    if(i < 0)
        return;

    moveDish(i, 0, "");
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

    if(dishIndex < 0)
        return;

    //QStringList comments = m_orderDrv->m_dishes[dishIndex].comment.split(",");
    DlgDishComment *d = new DlgDishComment(this);

    if(d->exec() == QDialog::Accepted) {
        m_ord->m_dishes[dishIndex]->f_comments = d->result();
        m_ord->m_dishes[dishIndex]->m_saved = false;
        ui->lstOrder->repaint();
    }

    delete d;
}

void dlgorder::on_tblDishGroup_clicked(const QModelIndex &index)
{
    if(!index.isValid())
        return;

    if(!index.data(Qt::UserRole).toInt())
        return;

    buildDishes(index.data(Qt::UserRole).toInt());
}

void dlgorder::on_btnPlugins_clicked()
{
    QMap<QString, QVariant> data;
    QDir d(QSystem::appPath() + "/orderplugins/");
    QStringList files = d.entryList();

    for(QStringList::const_iterator i = files.begin(); i != files.end(); i++) {
        if((*i) == "." || (*i) == "..")
            continue;

        QLibrary l(QSystem::appPath() + "/orderplugins/" + (*i));

        if(!l.load())
            continue;

        typedef QString(*caption)();
        caption c = (caption) l.resolve("caption");

        if(!c) {
            l.unload();
            continue;
        }

        data[c()] = QSystem::appPath() + "/orderplugins/" + (*i);
        l.unload();
    }

    QVariant out;

    if(!DlgList::value(data, out, this))
        return;

    QLibrary l(out.toString());

    if(!l.load()) {
        DlgMessage::Msg(tr("Could not load library") + "\n" + out.toString());
        return;
    }

    typedef bool (*exec)(const QMap<QString, QString>& filter, OD_Drv * o, QString & out);
    exec e = (exec) l.resolve("exec");

    if(!e) {
        DlgMessage::Msg(tr("Could not find entry point for") + "\nexec(QWidget *parent, FF_OrderDrv *o, QString &out)");
        l.unload();
        return;
    }

    QString msg;
    QMap<QString, QString> filter;
    typedef bool (*filterDate)();
    filterDate fd = (filterDate)l.resolve("filterDate");

    if(fd) {
        DlgReportFilter *dr = new DlgReportFilter(filter, this);

        if(dr->exec() != QDialog::Accepted) {
            delete dr;
            l.unload();
            return;
        }
    }

    if(!e(filter, m_ord, msg)) {
        DlgMessage::Msg(msg);
    }

    makeDishesList();

    for(int i = 0; i < m_ord->m_dishes.count(); i++)
        if(m_ord->m_dishes[i]->f_stateId == 1)
            m_ord->m_dishes[i]->m_saved = false;

    m_ord->openDB();
    m_ord->saveAll();
    m_ord->closeDB();
    setButtonsState();
    DlgMessage::Msg(tr("Complete") + "\n" + msg);
}

void dlgorder::on_btnCalculator_clicked()
{
    DlgCalcChange *d = new DlgCalcChange(m_ord->m_header.f_amount, this);
    d->exec();
    delete d;
}

void dlgorder::on_lstOrder_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous)
{
    OD_Dish *d;

    if(previous) {
        d = m_ord->dish(previous->data(Qt::UserRole).toInt());
        d->disconnect();
    }

    if(current) {
        d = m_ord->dish(current->data(Qt::UserRole).toInt());
        connect(d, SIGNAL(update(int)), this, SLOT(dishRepaint(int)));

        if(d->f_price > 0.001)
            ui->btnPresent->setEnabled(FF_DishesDrv::canPresent(d->f_dishId) && m_user->roleRead(ROLE_W_PRESENT));
    }
}

void dlgorder::on_btnPresent_clicked()
{
    int index = dishIndexFromListWidget();

    if(index < 0)
        return;

    if(DlgMessage::Msg(tr("Confirm present")) != QDialog::Accepted)
        return;

    OD_Dish *d = m_ord->dish(index);
    d->f_price = 0.1f;
    d->m_saved = false;
    m_ord->countAmounts();
}

void dlgorder::on_btnDiscount_2_clicked()
{
    DlgCardTips d(m_ord->m_header.f_id);

    if(d.exec() == QDialog::Accepted) {
    }
}

void dlgorder::on_btnPluse05_clicked()
{
    incQty(0.5);
}

void dlgorder::on_btnMinus05_clicked()
{
    decQty(0.5);
}

void dlgorder::setQty(double qty)
{
    if(OD_Dish *d = dishFromListWidget()) {
        if(!d->f_emarks.isEmpty()) {
            msg(tr("Cannot change quantity of dish thats contains emark"));
            return;
        }

        if(d->f_printedQty > qty) {
            return;
        }

        if(d->f_remind > 0 && d->f_printedQty > 0.001) {
            return;
        }

        d->f_totalQty = qty;
        d->m_saved = false;
        LogThread::logOrderThread(m_ord->m_header.f_currStaffName, m_ord->m_header.f_id, QString::number(d->f_id), tr("Set qty to"), d->f_dishName + ": " +  double_str(qty));
        m_ord->countAmounts();
        setButtonsState();
    }
}

void dlgorder::decQty(double qty)
{
    int index = dishIndexFromListWidget();

    if(index < 0) {
        return;
    }

    if(OD_Dish *d = m_ord->dish(index)) {
        if(d->f_printedQty > d->f_totalQty - qty) {
            return;
        }

        if(d->f_totalQty - qty < 0.5) {
            if(d->f_printedQty > 0.01) {
                message(tr("Use removal tool or call to manager."));
                return;
            } else {
                d->f_stateId = DISH_STATE_REMOVED_NORMAL;
                d->f_emarks = "";
                d->m_saved = false;
                m_ord->saveAll();
                ui->lstOrder->viewport()->update();
                LogThread::logOrderThread(m_ord->m_header.f_currStaffName, m_ord->m_header.f_id,
                                          QString::number(d->f_id), tr("Removed without print "),
                                          d->f_dishName + ": " + double_str(d->f_totalQty));
                return;
            }
        }

        d->f_totalQty -= qty;
        d->m_saved = false;
        QString msg = tr("Decrease qty by");

        if(d->f_totalQty < 0.001) {
            msg = tr("Remove, because qty is zero. Decrease by");
            d->f_stateId = DISH_STATE_REMOVED_NORMAL;
            QListWidgetItem *item = ui->lstOrder->currentItem();
            delete item;
        }

        LogThread::logOrderThread(m_ord->m_header.f_currStaffName, m_ord->m_header.f_id, QString::number(d->f_id), msg, d->f_dishName + ": -" + double_str(qty) + " -> " + double_str(d->f_totalQty));
        m_ord->countAmounts();
        setButtonsState();
    }
}

void dlgorder::incQty(double qty)
{
    if(OD_Dish *d = dishFromListWidget()) {
        if(!d->f_emarks.isEmpty()) {
            msg(tr("Cannot change quantity of dish thats contains emark"));
            return;
        }

        if(d->f_remind > 0) {
            if(d->f_printedQty > 0.001) {
                OD_Dish *nd = d->copy();
                nd->f_printedQty = 0;
                nd->f_totalQty = 1;
                int ni = m_ord->appendDish(nd);
                QListWidgetItem *item = new QListWidgetItem(ui->lstOrder);
                item->setSizeHint(QSize(ui->lstOrder->width(), 50));
                ui->lstOrder->addItem(item);
                ui->lstOrder->setCurrentItem(item);
                item->setData(Qt::UserRole, ni);
                setButtonsState();
                ui->lstOrder->verticalScrollBar()->setValue(ui->lstOrder->verticalScrollBar()->maximum());
                ui->lstOrder->setCurrentRow(ui->lstOrder->count() - 1);
                LogThread::logOrderThread(m_ord->m_header.f_currStaffName, m_ord->m_header.f_id, QString::number(d->f_id), tr("Increase qty by"), d->f_dishName + ": +" + double_str(qty) + " -> " + double_str(d->f_totalQty));
                return;
            }
        }

        d->f_totalQty += qty;
        d->m_saved = false;
        LogThread::logOrderThread(m_ord->m_header.f_currStaffName, m_ord->m_header.f_id, QString::number(d->f_id), tr("Increase qty by"), d->f_dishName + ": +" + double_str(qty) + " -> " + double_str(d->f_totalQty));
        m_ord->countAmounts();
        setButtonsState();
    }
}

void dlgorder::on_btnPlus1_clicked()
{
    incQty(1);
}

void dlgorder::on_btnMinus1_clicked()
{
    decQty(1);
}

void dlgorder::on_btnRmDish_clicked()
{
    if(OD_Dish *d = dishFromListWidget()) {
        if(d->f_printedQty < 0.01) {
            d->f_stateId = DISH_STATE_REMOVED_NORMAL;
            d->f_emarks = "";
            d->m_saved = false;
            m_ord->saveAll();
            LogThread::logOrderThread(m_ord->m_header.f_currStaffName, m_ord->m_header.f_id,
                                      QString::number(d->f_id), tr("Removed without print "),
                                      d->f_dishName + ": " + double_str(d->f_totalQty));
        } else {
            DlgCorrection *dc = new DlgCorrection(m_ord, d, this);

            if(dc->exec() == QDialog::Accepted) {
                if(d->f_totalQty < 0.001 || d->f_stateId != DISH_STATE_NORMAL) {
                    delete ui->lstOrder->item(listIndexOfDish(d));
                }
            }

            dc->deleteLater();
        }
    }

    ui->lstOrder->viewport()->update();
    m_ord->countAmounts();
    setButtonsState();
}

void dlgorder::on_btnAnyQty_clicked()
{
    double qty;

    if(DlgQty::qty(qty, this)) {
        setQty(qty);
    }
}

void dlgorder::on_btnRequestOrderRemove_clicked()
{
    if(m_ord->m_header.f_requestremoveal > 0) {
        return;
    }

    if(msg(tr("Submit an order removal request?")) != QDialog::Accepted) {
        return;
    }

    m_ord->m_header.f_requestremoveal = 1;
    m_ord->m_header.m_saved = false;
    m_ord->saveAll();
    LogThread::logOrderThread(m_ord->m_header.f_currStaffName, m_ord->m_header.f_id, "", tr("Request order removal"), "");
    setButtonsState();
    msg(tr("Your request was sent to the responsible person"));
}

void dlgorder::on_btnEmark_clicked()
{
    if(auto *d = dishFromListWidget()) {
        if(d->f_emarks.isEmpty()) {
            bool ok;
            QString emarks = QInputDialog::getText(this, tr("Emarks"), "", QLineEdit::Normal, "", &ok);

            if(!ok) {
                return;
            }

            if(emarks.isEmpty()) {
                return;
            }

            if(emarks.length() < 29) {
                msg(tr("Invalid emarks"));
                return;
            }

            m_ord->prepare("select id from o_dishes where state_id=1 and emarks=:emarks");
            m_ord->bindValue(":emarks", emarks);
            m_ord->execSQL();

            if(m_ord->next()) {
                msg(tr("Used emarks detected"));
                return;
            }

            d->f_emarks = emarks;
            d->m_saved = false;
            dishRepaint(0);
            m_ord->saveAll();
        } else {
            if(msg(tr("Clear emarks?")) == QDialog::Accepted) {
                d->f_emarks = "";
                dishRepaint(0);
                m_ord->saveAll();
            }
        }
    }
}
void dlgorder::on_leCmd_returnPressed()
{
    QString code = ui->leCmd->text();
    ui->leCmd->clear();
    QString emarks;
    int index = -1;

    if(code.length() == 13 || code.length() == 8) {
        index = m_dishesDrv->indexOfDishByBarcode(code);
    } else  if(code.length() >= 29) {
        QString barcode;

        if(code.mid(0, 6) == "000000") {
            barcode = code.mid(6, 8);
        } else if(code.mid(0, 3) == "010") {
            if(code.mid(0, 8) == "01000000") {
                barcode = code.mid(8, 8);
            } else {
                barcode = code.mid(3, 13);
            }
        } else {
            barcode = code.mid(1, 8);
            index = m_dishesDrv->indexOfDishByBarcode(barcode);

            if(index < 0) {
                barcode.clear();
            }

            if(barcode.isEmpty()) {
                barcode = code.mid(1, 13);
                index = m_dishesDrv->indexOfDishByBarcode(barcode);

                if(index < 0) {
                    barcode.clear();
                }
            }
        }

        if(barcode.isEmpty()) {
            msg(tr("Invalid emarks"));
            return;
        }

        index = m_dishesDrv->indexOfDishByBarcode(barcode);

        if(index < 0) {
            msg(tr("Invalid barcode"));
            return;
        }

        m_ord->prepare("select * from o_dishes where emarks=:emarks and state_id=1");
        m_ord->bindValue(":emarks", code);
        m_ord->execSQL();

        if(m_ord->next()) {
            msg(tr("Used emarks detected"));
            return;
        }

        emarks = code;
    }

    if(index > -1) {
        index = addNewDish(index);
        OD_Dish *d = dishFromListWidget();
        d->f_emarks = emarks;
        d->m_saved = false;
        dishRepaint(0);
        m_ord->saveAll();
    }
}

void dlgorder::on_btnOrderComment_clicked()
{
    DlgDishComment d(this);

    if(d.exec() == QDialog::Accepted) {
        m_ord->m_header.f_comment = d.result();
        m_ord->m_saved = false;
    }
}
