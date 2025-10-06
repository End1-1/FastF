#include "dlgmaindbsettings.h"
#include "ui_dlgmaindbsettings.h"
#include "core.h"
#include <QFile>
#include <QDir>

#ifdef WAITER_EXE
#include "../core_gui/dlgmessagebox.h"
#else
#include <QMessageBox>
#endif

dlgMainDBSettings::dlgMainDBSettings(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::dlgMainDBSettings)
{
    ui->setupUi(this);
    ui->lnServerAddress->setText(Connection::serverUrl());
    ui->lnServerUsername->setText(Connection::user());
    ui->lnServerPassword->setText(Connection::password());
    ui->lnSettingsPassword->setText(Connection::settingsPassword());
    ui->cbWaiterConfiguration->addItem(Connection::waiterSettings());
    ui->cbWaiterConfiguration->setCurrentIndex(0);
    ui->gridLayout->setColumnMinimumWidth(2, 500);
#ifdef MANAGER_EXE
    ui->btnGetConfigurationList->setIcon(QIcon(":/res/reload.png"));
#endif
}

dlgMainDBSettings::~dlgMainDBSettings()
{
    delete ui;
}

void dlgMainDBSettings::takeConfiguration(const QString &qName, const QXmlAttributes &atts)
{
    if (qName == "config") {
        ui->cbWaiterConfiguration->addItem(atts.value("value"));
        return;
    }
}

void dlgMainDBSettings::endOfTakeConfiguration(int code, const QString &message)
{
    if (code) {
#ifdef WAITER_EXE
        DlgMessageBox::show(message, 1);
#else
        QMessageBox::critical(this, tr("Error"), message);
#endif
    }
}

void dlgMainDBSettings::on_btnCancel_clicked()
{
    reject();
}

void dlgMainDBSettings::on_btnSave_clicked()
{
    Connection::setServerUrl(ui->lnServerAddress->text());
    Connection::setUser(ui->lnServerUsername->text());
    Connection::setPassword(ui->lnServerPassword->text());
    Connection::setSettingsPassword(ui->lnSettingsPassword->text());
    Connection::setWaiterSettings(ui->cbWaiterConfiguration->currentText());
    Connection::writeMainDatabaseFile();
    QMessageBox::information(this, tr("Information"), tr("Saved"));
}

void dlgMainDBSettings::on_btnGetConfigurationList_clicked()
{
    ui->cbWaiterConfiguration->clear();
    BaseNetwork *b = new BaseNetwork();
    b->appendData("sysop", "LOADSETTINGS");
    b->appendData("username", ui->lnServerUsername->text());
    b->appendData("password", ui->lnServerPassword->text());
    connect(b, SIGNAL(takeElement(QString,QXmlAttributes)), SLOT(takeConfiguration(QString,QXmlAttributes)));
    connect(b, SIGNAL(endOfQuery(int,QString)), SLOT(endOfTakeConfiguration(int,QString)));
    b->post();
}
