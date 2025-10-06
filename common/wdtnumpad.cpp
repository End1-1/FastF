#include "wdtnumpad.h"
#include "ui_wdtnumpad.h"
#include <QDoubleValidator>

WdtNumpad::WdtNumpad(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WdtNumpad)
{
    ui->setupUi(this);
    connect(this, SIGNAL(key(QString)), SLOT(press(QString)));
    ui->btnEnter->setIcon(QIcon(":/res/ok.png"));
    ui->pushButton_4->setIcon(QIcon(":/res/cancel.png"));
}

WdtNumpad::~WdtNumpad()
{
    delete ui;
}

const QString WdtNumpad::password()
{
    return ui->lePassword->text();
}

void WdtNumpad::setNoPassEcho()
{
    ui->lePassword->setEchoMode(QLineEdit::Normal);
}

void WdtNumpad::configDouble()
{
    setNoPassEcho();
    ui->lePassword->setValidator(new QDoubleValidator(0, 999999999, 0));
}

void WdtNumpad::press(const QString &str)
{
    m_string += str;
    ui->lePassword->setText(m_string);
}

void WdtNumpad::on_pushButton_clicked()
{
    emit key("1");
}

void WdtNumpad::on_pushButton_5_clicked()
{
    emit key("2");
}

void WdtNumpad::on_pushButton_6_clicked()
{
    emit key("3");
}

void WdtNumpad::on_pushButton_13_clicked()
{
    emit key("4");
}

void WdtNumpad::on_pushButton_2_clicked()
{
    emit key("5");
}

void WdtNumpad::on_pushButton_7_clicked()
{
    emit key("6");
}

void WdtNumpad::on_pushButton_10_clicked()
{
    emit key("7");
}

void WdtNumpad::on_pushButton_14_clicked()
{
    emit key("8");
}

void WdtNumpad::on_pushButton_3_clicked()
{
    emit key("9");
}

void WdtNumpad::on_pushButton_8_clicked()
{
    emit key("0");
}

void WdtNumpad::on_pushButton_11_clicked()
{
    m_string = "";
    ui->lePassword->clear();
}

void WdtNumpad::on_pushButton_15_clicked()
{
    m_string = m_string.mid(0, m_string.length() - 1);
    ui->lePassword->setText(m_string);
}

void WdtNumpad::on_btnEnter_clicked()
{
    emit keyEnter();
}

void WdtNumpad::on_pushButton_4_clicked()
{
    emit cancel();
}

void WdtNumpad::on_lePassword_returnPressed()
{
    on_btnEnter_clicked();
}
