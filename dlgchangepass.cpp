#include "dlgchangepass.h"
#include "ui_dlgchangepass.h"
#include "qsqldrv.h"
#include "dlggetpassword.h"
#include "dlgmessage.h"
#include <QCryptographicHash>

DlgChangePass::DlgChangePass(FF_User *user, QWidget *parent) :
    QDialog(parent, Qt::FramelessWindowHint),
    ui(new Ui::DlgChangePass),
    m_user(user)
{
    ui->setupUi(this);
    showFullScreen();
    ui->tblEmployes->setButtons(ui->btnUp, ui->btnDown);
    m_sqlDrv = new QSqlDrv(m_user->fullName, "main");
    getEmployesList();
}

DlgChangePass::~DlgChangePass()
{
    delete m_sqlDrv;
    delete ui;
}

void DlgChangePass::getEmployesList()
{
    if (!m_sqlDrv->prepare("select id, fname || ' ' || lname from employes where ((group_id in (3,14,16,70)) or (group_id in (2,11) and id=:id)) and state_id=1 order by 2"))
        return;
    m_sqlDrv->bind(":id", m_user->id);
    if (!m_sqlDrv->execSQL())
        return;
    m_sqlDrv->fillTableWidget2(ui->tblEmployes, 1, 0);
    m_sqlDrv->close();
}

void DlgChangePass::on_btnClose_clicked()
{
    accept();
}

void DlgChangePass::on_tblEmployes_clicked(const QModelIndex &index)
{
    if (!index.isValid())
        return;
    int id = index.data(Qt::UserRole).toInt();
    if (!id)
        return;
    QString pass;
    if (!DlgGetPassword::password(index.data(Qt::DisplayRole).toString(), pass, true, this))
        return;

    if (pass.length() < 4) {
        DlgMessage::Msg(tr("The password is too short"));
        return;
    }

    pass = QString(QCryptographicHash::hash(pass.toLatin1(), QCryptographicHash::Md5).toHex());
    if (!m_sqlDrv->prepare("update employes set password2=null where password2=:password2"))
        return;
    m_sqlDrv->bind(":password2", pass);
    if (!m_sqlDrv->execSQL())
        return;
    if (!m_sqlDrv->prepare("update employes set password2=:password2 where id=:id"))
        return;
    m_sqlDrv->bind(":password2", pass);
    m_sqlDrv->bind(":id", id);
    if (!m_sqlDrv->execSQL())
        return;
    m_sqlDrv->close();
    DlgMessage::Msg(tr("Password was changed"));
}
