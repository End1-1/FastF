#include "dlgregistercard.h"
#include "ui_dlgregistercard.h"
#include "database.h"
#include "dlgmessage.h"
#include "dlggetpassword.h"
#include "qsqldrv.h"
#include <QPainter>
#include <QItemDelegate>

#define U(X) QString::fromUtf8(X)

class QLetterDelegate : public QItemDelegate {
protected:
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const {
        painter->drawText(option.rect, index.data().toString(), QTextOption(Qt::AlignCenter));
    }
};

DlgRegisterCard::DlgRegisterCard(bool v, QWidget *parent) :
    QDialog(parent, Qt::FramelessWindowHint),
    ui(new Ui::DlgRegisterCard)
{
    ui->setupUi(this);
    showFullScreen();
    view = v;

    QStringList row1, row2, row3, row4, row5;
    row1 << U("1") << U("2") << U("3") << U("4") << U("5") << U("6") << U("7") << U("8") << U("9") << U("0") << U("=") << U(".") << U(" ");
    row2 << U("Է") << U("Թ") << U("Փ") << U("Ձ") << U("Ջ") << U("Ւ") << U("և") << U("Ր") << U("Չ") << U("Ճ") << U("-") << U("Ժ") << U(" ");
    row3 << U("Ք") << U("Ո") << U("Ե") << U("Ռ") << U("Տ") << U("Տ") << U("Ը") << U("Ւ") << U("Ի") << U("Օ") << U("Պ") << U("Խ") << U("Ծ");
    row4 << U("Ա") << U("Ս") << U("Դ") << U("Ֆ") << U("Գ") << U("Հ") << U("Յ") << U("Կ") << U("Լ") << U(":") << U("Շ") << U(" ") << U(" ");
    row5 << U("Զ") << U("Ղ") << U("Ց") << U("Վ") << U("Բ") << U("Ն") << U("Մ") << U("")  << U("")  << U("?") << U(" ") << U(" ") << U(" ");
    m_letters.insert(0, row1);
    m_letters.insert(1, row2);
    m_letters.insert(2, row3);
    m_letters.insert(3, row4);
    m_letters.insert(4, row5);
    int col = 0, row = 0;
    for (QMap<int,QStringList>::const_iterator it = m_letters.constBegin(); it != m_letters.constEnd(); it++) {
        col = 0;
        ui->kbd->setRowCount(it.key() + 1);
        ui->kbd->setColumnCount(it.value().count() > ui->kbd->columnCount() ? it.value().count() : ui->kbd->columnCount());
        for (QStringList::const_iterator jt = it.value().begin(); jt != it.value().end(); jt++)
            ui->kbd->setItem(it.key(), col++, new QTableWidgetItem(*jt));
        row++;
    }
    ui->kbd->setMinimumHeight((ui->kbd->rowCount() * ui->kbd->verticalHeader()->defaultSectionSize()) + 4);
    ui->kbd->setItemDelegate(new QLetterDelegate());
    ui->kbd->setVisible(!v);

    ui->tbl->setColumnWidth(0, 0);
    ui->tbl->setColumnWidth(1, 200);
    ui->tbl->setColumnWidth(2, 400);
    Database db("QIBASE");
    if (!db.open("10.1.0.4", "maindb", "SYSDBA", "Inter_OneStep")){
       DlgMessage::Msg(tr("Cannot connect to database"));
       return;
    }
    QString w;
    if (view) {
        QSqlDrv m_sqlDrv("", "main");
        if (!m_sqlDrv.prepare("select department_id from sys_app "))
            return;
        if (!m_sqlDrv.execSQL())
            return;
        m_sqlDrv.next();
        QString department = m_sqlDrv.m_query->value(0).toString();
        w = " and e.id in (select user_Id from attendance where dateout is null and department_id=" + department + ") ";
    }
    db.exec("SELECT e.id, gr.name, e.lname || ' ' || e.fname  as fullname "
            "FROM employes e "
            "LEFT JOIN EMPLOYES_GROUP gr ON gr.id=e.GROUP_ID "
            "WHERE e.STATE_ID =1 " + w +
            "ORDER BY 2, 3");
    while (db.next()) {
        int r = ui->tbl->rowCount();
        ui->tbl->setRowCount(r + 1);
        ui->tbl->setItem(r, 0, new QTableWidgetItem(db.string("id")));
        ui->tbl->setItem(r, 1, new QTableWidgetItem(db.string("name")));
        ui->tbl->setItem(r, 2, new QTableWidgetItem(db.string("fullname")));
    }
}

DlgRegisterCard::~DlgRegisterCard()
{
    delete ui;
}

void DlgRegisterCard::on_leSearch_textChanged(const QString &arg1)
{
    for (int r = 0; r < ui->tbl->rowCount(); r++) {
        bool rh = true;
        for (int c = 0; c < ui->tbl->columnCount(); c++) {
            if (ui->tbl->item(r, c)->data(Qt::EditRole).toString().contains(arg1, Qt::CaseInsensitive)) {
              rh = false;
              break;
            }
        }
        ui->tbl->setRowHidden(r, rh);
    }
}

void DlgRegisterCard::on_btnExit_clicked()
{
    accept();
}

void DlgRegisterCard::on_kbd_clicked(const QModelIndex &index)
{
    if (!index.isValid())
        return;

    ui->leSearch->setText(ui->leSearch->text() + index.data().toString());
}

void DlgRegisterCard::on_btnClear_clicked()
{
    ui->leSearch->clear();
}

void DlgRegisterCard::on_tbl_clicked(const QModelIndex &index)
{
    if (view) {
        return;
    }
    if (!index.isValid())
        return;
    QString pass;
    if (!DlgGetPassword::password(ui->tbl->item(index.row(), 2)->data(Qt::DisplayRole).toString(), pass, true, this))
        return;

    if (pass.length() < 4) {
        DlgMessage::Msg(tr("The password is too short"));
        return;
    }

    Database db("QIBASE");
    if (!db.open("10.1.0.4", "maindb", "SYSDBA", "Inter_OneStep")){
       DlgMessage::Msg(tr("Cannot connect to database"));
       return;
    }
    db[":acard"] = pass;
    db.exec("update employes set acard=null where acard=:acard");
    db[":id"] = ui->tbl->item(index.row(), 0)->data(Qt::DisplayRole);
    db[":acard"] = pass;
    db.exec("update employes set acard=:acard where id=:id");
    DlgMessage::Msg(tr("Done"));
}
