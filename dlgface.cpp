#include "dlgface.h"
#include "ui_dlgface.h"
#include "ff_user.h"
#include "dlgmessage.h"
#include "dlgconnection.h"
#include "ff_settingsdrv.h"
#include <QPainter>
#include "tableordersocket.h"
#include "dlgreports.h"
#include "dlggetpassword.h"
#include "dlgchangepass.h"
#include "dlglist.h"
#include "dlgtimelimit.h"
#include "cnfmaindb.h"
#include "ff_correcttime.h"
#include "logthread.h"
#include "msqldatabase.h"
#include "dbdriver.h"
#include "dlgkinoparkcall.h"
#include "dlgconfigmobile.h"
#include "cnfapp.h"
#include <windows.h>
#include <QWindow>

#define TIMER_TIMEOUT 5000
#define UPDATE_TIME_DEVIDER 30
#define UPDATE_TIME_ONLINE 5

DlgFace::DlgFace(QWidget *parent) :
    #ifdef QT_DEBUG
    QDialog(parent, Qt::FramelessWindowHint),
    #else
    QDialog(parent, Qt::FramelessWindowHint),
    #endif
    ui(new Ui::DlgFace)
{
    ui->setupUi(this);
    ui->lbTime->setText("--");
    ui->lbOrders->setText("0/0");
    showFullScreen();
    QSqlDatabase db = QSqlDB::dbByName("main");
    if (!db.password().length())
        return;
    FF_SettingsDrv::init();
    ui->lstReminder->setItemDelegate(new ReadyDishDelegate());
    ui->lstReminder->setVisible(FF_SettingsDrv::value(SD_REMINDER).toInt() == 1);
    ui->tblHall->setButtons(ui->btnUp, ui->btnDown);
    qApp->processEvents();
    m_filterUsedTables = false;
    m_filterHall = FF_SettingsDrv::value(SD_DEFAULT_HALL_ID).toInt();
    m_hallDrv = nullptr;
    initFace();
    connect(&m_timer, SIGNAL(timeout()), this, SLOT(timer()));
    m_timeout = 0;
    m_timer.start(TIMER_TIMEOUT);
    activateWindow();
    raise();
//    int i = 0;
//    while (1) {
//        qApp->processEvents();
//        qDebug() << "CURRENT TABLE ORDER" << i;
//        TableOrderSocket *t = new TableOrderSocket(++i % 20);
//        connect(t, SIGNAL(err(QString)), this, SLOT(toErrText(QString)));
//        connect(t, SIGNAL(tableLocked(int)), this, SLOT(toTableLocketText(int)));
//        t->begin();
//    }
}

DlgFace::~DlgFace()
{
    delete ui;
}

void DlgFace::toErrText(const QString &msg)
{
    sender()->deleteLater();
    qDebug() << msg;
}

void DlgFace::toTableLocketText(int tableId)
{
    sender()->deleteLater();
    qDebug() << "Locked" << tableId;
}

void DlgFace::toError(const QString &msg)
{
    DlgMessage::Msg(msg);
    sender()->deleteLater();
}

void DlgFace::toTableLocked(int tableId)
{
    TableOrderSocket *to = static_cast<TableOrderSocket*>(sender());
    QString pass;
    if (!DlgGetPassword::password(m_hallDrv->table(tableId)->name, pass, true, this)) {
        to->deleteLater();
        return;
    }
    pass.replace(";", "");
    pass.replace("?", "");
    if (pass.length() < 4) {
        to->deleteLater();
        return;
    }

    m_timer.stop();
    FF_User *user = new FF_User("main");
    user->setCredentails("", pass);
    if (user->auth1()) {
        if (user->roleRead(ROLE_EDIT_ORDER)) {
            dlgorder *order = new dlgorder(this);
            connect(to, SIGNAL(socketDisconnected()), order, SLOT(toDisconnected()));
            if (order->setData(user, m_hallDrv, tableId, "", to))
                order->exec();
            else
                DlgMessage::Msg(tr("Cannot open table"));
            delete order;
            timer();
        } else
            DlgMessage::Msg(tr("User have not access to edit order"));
    } else {
        DlgMessage::Msg(tr("Invalid password"));
    }

    delete user;
    to->deleteLater();
    m_timer.start(TIMER_TIMEOUT);
}

void DlgFace::timer()
{
    if (!__cnfmaindb.fOk) {
        return;
    }
    m_timeout++;
    if (__cnfmaindb.fServerMode.toInt() > 0) {
        if ((m_timeout % UPDATE_TIME_ONLINE) == 0) {
            onlineUp();
        }
    }
    if (!(m_timeout % UPDATE_TIME_DEVIDER))
        correctTime();
    ui->lbTime->setText(DbDriver::serverDateTime().toString("dd.MM.yyyy HH:mm"));
    m_hallDrv->refresh();
    ui->lbOrders->setText(m_hallDrv->total());
    loadReadyDishes();
    for (int i = 0; i < ui->tblHall->rowCount(); i++)
        for (int j = 0; j < ui->tblHall->columnCount(); j++)
            ui->tblHall->update(ui->tblHall->model()->index(i, j));

}

void DlgFace::sqlError(const QString &msg)
{
    DlgMessage::Msg(msg);
}

void DlgFace::on_btnExit_clicked()
{
    accept();
}

void DlgFace::configHallGrid()
{
    m_hallDrv->configGrid(ui->tblHall, new HallItemDelegate(m_hallDrv));
    ui->btnLockStations->setVisible(FF_SettingsDrv::value(SD_BTN_LOCKSTATIONS).toBool());
}

void DlgFace::initFace()
{
    if (!m_hallDrv) {
        m_hallDrv = new FF_HallDrv();
        connect(m_hallDrv, SIGNAL(errorMsg(QString)), this, SLOT(sqlError(QString)));
    }
    m_hallDrv->filter(m_filterHall, m_filterUsedTables);
    configHallGrid();
    loadReadyDishes();
    if (FF_SettingsDrv::value(SD_UPDATE_TIME_FROM_SERVER).toInt()) {
        QPalette p = ui->lbTime->palette();
        p.setColor(QPalette::WindowText, QColor(Qt::blue));
        ui->lbTime->setPalette(p);
    }
    correctTime();
}

void DlgFace::loadReadyDishes()
{
    ui->lstReminder->clear();
    QSqlDrv d("FASTFF", "main");
    d.execSQL("select dr.id, t.name, e.fname || ' ' || e.lname, m.name, dr.qty "
              "from o_dishes_reminder dr, h_table t, employes e, me_dishes m "
              "where dr.table_id=t.id and dr.staff_id=e.id and dr.dish_id=m.id "
              "and dr.date_ready is not null and dr.date_done is null "
              "order by dr.date_ready");
    while (d.next()) {
        QListWidgetItem *item = new QListWidgetItem(ui->lstReminder);
        item->setSizeHint(QSize(150, 50));
        item->setData(Qt::UserRole, d.val());
        item->setData(Qt::UserRole + 1, d.val());
        item->setData(Qt::UserRole + 2, d.val());
        item->setData(Qt::UserRole + 3, d.val());
        item->setData(Qt::UserRole + 4, d.val());
        ui->lstReminder->addItem(item);
    }
}

void DlgFace::correctTime()
{
    QStringList dbParams;
    if (FF_SettingsDrv::value(SD_UPDATE_TIME_SERVER).toString().length()) {
        dbParams = FF_SettingsDrv::value(SD_UPDATE_TIME_SERVER).toString().split(",");
    } else {
        dbParams.append(__cnfmaindb.fHost);
        dbParams.append(__cnfmaindb.fDatabase);
        dbParams.append(__cnfmaindb.fUser);
        dbParams.append(__cnfmaindb.fPassword);
    }
    FF_CorrectTime *ff = new FF_CorrectTime(FF_SettingsDrv::value(SD_UPDATE_TIME_FROM_SERVER).toInt(), dbParams);
    ff->start();
}

void DlgFace::onlineUp()
{
    if (__cnfapp.onlineReportAddress().length() == 0) {
        return;
    }
    MSqlDatabase db;
    QMap<QString, QVariant> v;
    DatabaseResult dr;
    db.select(QString("select date_cash, sum(amount) as amount, count(id) as qty from o_order where (date_cash between '%1' and '%2' and state_id=2) or (state_id=1) group by 1")
              .arg(QDate::currentDate().addDays(-15).toString(DATE_FORMAT))
              .arg(QDate::currentDate().toString(DATE_FORMAT)), v, dr);

    QNet *n = new QNet();
    connect(n, SIGNAL(getResponse(QString,bool)), this, SLOT(onlineUpReply(QString,bool)));
    n->URL = __cnfapp.onlineReportAddress();
    n->addData("cafe", __cnfapp.onlineReportCafeId());
    n->addData("cafe", __cnfapp.onlineReportCafeId());
    n->addData("s", __cnfapp.onlineReportPassword());
    for (int i = 0; i < dr.rowCount(); i++) {
        n->addData("data[" + dr.value(i, "DATE_CASH").toDate().toString("yyyy-MM-dd") + "][amount]", dr.value(i, "AMOUNT").toString());
        n->addData("data[" + dr.value(i, "DATE_CASH").toDate().toString("yyyy-MM-dd") + "][qty]", dr.value(i, "QTY").toString());
    }
    n->goSSL();
}

HallItemDelegate::HallItemDelegate(FF_HallDrv *hallDrv)
{
    m_hallDrv = hallDrv;
}

void HallItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if (!index.isValid())
        return;

    int tableId = index.data(Qt::UserRole).toInt();
    if (!tableId)
        return;
    FF_HallDrv::Table *t = m_hallDrv->table(tableId);
    if (!t) {
        return;
    }

    painter->save();

    QFont font(qApp->font());

    QBrush brush(Qt::SolidPattern);
    QColor bgColor = QColor::fromRgb(230, 250, 255);
    if (t->orderId.length())
       bgColor = QColor::fromRgb(150, 255, 150);
    if (t->printed)
       bgColor = QColor::fromRgb(250, 250, 150);
    if (t->lock.length())
       bgColor = QColor::fromRgb(255, 120, 120);
    brush.setColor(bgColor);
    painter->setBrush(brush);

    QPen pen(QColor::fromRgb(100, 200, 200));
    painter->setPen(pen);

    QRect r = option.rect;
    r.adjust(2, 2, -2, -2);
    painter->drawRoundedRect(r, 5, 5);
    r.adjust(2, 2, -2, -2);

    QRect tableRect = r;
    tableRect.adjust(2, 2,-1, 0);
    pen.setStyle(Qt::SolidLine);
    pen.setColor(Qt::white);
    painter->setPen(pen);
    font.setPointSize(16);
    font.setBold(true);
    int tableNameDelta = QFontMetrics(font).height();
    painter->setFont(font);
    painter->drawText(tableRect, t->name);
    pen.setColor(QColor::fromRgb(50, 50, 100));
    painter->setPen(pen);
    tableRect.adjust(-1, -1, 0, 0);
    painter->drawText(tableRect, t->name);

    QRect rSignal = option.rect;
    rSignal.adjust((rSignal.right() - rSignal.left()) - 30,  (rSignal.bottom() - rSignal.top() - 30), -3, -3);

    if (t->orderId.length()) {
       QRect openTimeRect = r;
       font.setPointSize(8);
       font.setBold(false);
       openTimeRect.adjust(openTimeRect.width() - QFontMetrics(font).width(t->dateOpen) - 2, 2, -2, -2);
       painter->setFont(font);
       painter->drawText(openTimeRect, t->dateOpen);

       QRect amountRect = r;
       amountRect.adjust(2, tableNameDelta, 0, 0);
       font.setPointSize(12);
       painter->setFont(font);
       pen.setColor(Qt::white);
       painter->setPen(pen);
       painter->drawText(amountRect, QString::number(t->amount, 'f', 0));
       amountRect.adjust(-1, -1, 0, 0);
       pen.setColor(QColor::fromRgb(80, 80, 100));
       painter->setPen(pen);
       painter->drawText(amountRect, QString::number(t->amount, 'f', 0));

       QString info = t->orderComment +", " + t->reserved;
       if (info.length()) {
           QRect comment = r;
           comment.adjust(comment.width() - QFontMetrics(font).width(info) - 2, tableNameDelta, 0, 0);
           painter->drawText(comment, info);
       }

       tableNameDelta += QFontMetrics(font).height();

       QRect staffRect = r;
       staffRect.adjust(2, tableNameDelta, 0, 0);
       font.setPointSize(10);
       font.setBold(false);
       painter->setFont(font);
       painter->drawText(staffRect, t->staffName);

       bool isNew = m_hallDrv->tableFlag(*t, TFLAG_NEW);
       if (isNew) {
           brush.setColor(Qt::white);
           painter->setBrush(brush);
           painter->drawEllipse(rSignal);
       }

       bool isCheckout = m_hallDrv->tableFlag(*t, TFLAG_CHECKOUT);
       if (isCheckout) {
           brush.setColor(Qt::red);
           painter->setBrush(brush);
           painter->drawEllipse(rSignal);
       }
    }

    painter->restore();
}

void DlgFace::on_tblHall_clicked(const QModelIndex &index)
{
    if (!index.isValid())
        return;

    int tableId = index.data(Qt::UserRole).toInt();
    if (!tableId)
        return;

    TableOrderSocket *to = new TableOrderSocket(tableId, this);
    connect(to, SIGNAL(err(QString)), this, SLOT(toError(QString)));
    connect(to, SIGNAL(tableLocked(int)), this, SLOT(toTableLocked(int)));
    to->begin();
}

void DlgFace::on_btnSettings_clicked()
{
    QSqlDatabase db = QSqlDB::dbByName("main");
    if (__cnfmaindb.fPassword.length() > 0) {
        QString pass = "";
        if (!DlgGetPassword::password(tr("Settings password"), pass, true, this))
            return;
        if (pass != __cnfmaindb.fPassword) {
            DlgMessage::Msg(tr("Invalid password"));
            return;
        }
    }

    DlgConnection *d = new DlgConnection(this);
    if (d->exec() == QDialog::Accepted) {
        db.setDatabaseName(d->path());
        db.setUserName(d->user());
        db.setPassword(d->pass());
        initFace();
    }
    delete d;
}

void DlgFace::on_btnReport_clicked()
{
    QString pass;
    if (!DlgGetPassword::password(tr("Reports"), pass, true, this))
        return;

    m_timer.stop();
    FF_User *user = new FF_User("main");
    user->setCredentails("", pass);
    if (user->auth1() && user->roleRead(ROLE_WAITER_REPORTS)) {
        dlgreports *d = new dlgreports(user, m_hallDrv, this);
        d->exec();
        delete d;
    } else
        DlgMessage::Msg(tr("Access denied"));

    delete user;
    m_timer.start(TIMER_TIMEOUT);
}

void DlgFace::on_btnChangePass_clicked()
{
    QString pass;
    if (!DlgGetPassword::password(tr("Change password"), pass, true, this))
        return;

    m_timer.stop();
    FF_User *user = new FF_User("main");
    user->setCredentails("", pass);
    if (user->auth1() && user->roleRead(ROLE_CHANGE_PASSWORD)) {
        DlgChangePass *d = new DlgChangePass(user, this);
        d->exec();
        delete d;
    } else
        DlgMessage::Msg(tr("Invalid password"));

    delete user;
    m_timer.start(TIMER_TIMEOUT);
}

void DlgFace::on_btnFilter_clicked()
{
    QVariant out;
    if (!DlgList::value(m_hallDrv->getHallMap(), out, this))
        return;
    m_filterHall = out.toInt();
    m_hallDrv->filter(m_filterHall, m_filterUsedTables);
    configHallGrid();
}


void ReadyDishDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if (!index.isValid())
        return;

    QFont font(qApp->font());

    painter->save();

    QRect r = option.rect;
    r.adjust(3, 3, -3, -3);
    QBrush brush(QColor::fromRgb(170, 170, 250));
    painter->setBrush(brush);
    QPen pen(QColor::fromRgb(200, 100, 100));
    painter->setPen(pen);
    painter->drawRoundedRect(r, 10, 10);

    pen.setStyle(Qt::SolidLine);
    pen.setColor(Qt::white);
    painter->setPen(pen);
    font.setPointSize(20);
    painter->setFont(font);
    QRect rectTable = r;
    rectTable.adjust(4, 4, -4, -4);
    painter->drawText(rectTable, index.data(Qt::UserRole + 1).toString());
    pen.setColor(QColor::fromRgb(50, 50, 100));
    painter->setPen(pen);
    rectTable.adjust(-1, -1, 0, 0);
    painter->drawText(rectTable, index.data(Qt::UserRole + 1).toString());
    int left = QFontMetrics(font).width(index.data(Qt::UserRole + 1).toString()) + 5;

    QRect rectStaff = r;
    rectStaff.adjust(left, 2, -2, -2);
    font.setPointSize(12);
    painter->setFont(font);
    painter->drawText(rectStaff, index.data(Qt::UserRole + 2).toString());

    QRect rectDish = r;
    rectDish.adjust(left, 20, -2, -2);
    painter->drawText(rectDish, index.data(Qt::UserRole + 3).toString() + " : " + index.data(Qt::UserRole + 4).toString());

    painter->restore();
}

void DlgFace::on_lstReminder_clicked(const QModelIndex &index)
{
    if (!index.isValid())
        return;

    if (DlgMessage::Msg(tr("Remove") + "\n"
                        + index.data(Qt::UserRole + 1).toString() + "\n"
                        + index.data(Qt::UserRole + 2).toString() + "\n"
                        + index.data(Qt::UserRole + 3).toString() + "\n"
                        + QString::number(index.data(Qt::UserRole + 4).toFloat(), 'f', 2)) != QDialog::Accepted)
        return;

    QSqlDrv d("FASTFF", "main");
    d.prepare("update o_dishes_reminder set state_id=4, date_done=current_timestamp where id=:id");
    d.bind(":id", index.data(Qt::UserRole));
    d.execSQL();

    QListWidgetItem *item = ui->lstReminder->item(index.row());
    delete item;
}

void DlgFace::on_btnFilterUsedTables_clicked()
{
    m_filterUsedTables = !m_filterUsedTables;
    m_hallDrv->filter(m_filterHall, m_filterUsedTables);
    configHallGrid();
}

void DlgFace::on_btnLockStations_clicked()
{
    dlgTimeLimit *d = new dlgTimeLimit(this);
    if (d->exec() == QDialog::Accepted) {
        QDateTime t = QDateTime::currentDateTime();
        t = t.addSecs((dlgTimeLimit::hour * 3600) + (dlgTimeLimit::min * 60));
        QSqlDrv *q = new QSqlDrv("PROGRAM", MAIN);
        q->prepare("insert into messanger (type_id, dst, message) values (:type_id, :dst, :message)");
        q->bind(":type_id", 5);
        q->bind(":dst", -1);
        q->bind(":message", t.toString("dd.MM.yyyy HH:mm:ss"));
        q->execSQL();
        q->close();
        delete q;
    }
    delete d;
}

void DlgFace::onlineUpReply(const QString &data, bool isError)
{
    Q_UNUSED(data);
    Q_UNUSED(isError);
    sender()->deleteLater();
}


void DlgFace::on_btnConfigMobile_clicked()
{
    DlgConfigMobile d;
    d.exec();
}
