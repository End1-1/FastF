#include "dlgtimelimit.h"
#include "ui_dlgtimelimit.h"
#include <QDateTime>

int dlgTimeLimit::hour = 0;
int dlgTimeLimit::min = 0;

dlgTimeLimit::dlgTimeLimit(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::dlgTimeLimit)
{
    ui->setupUi(this);
    countTime();
}

dlgTimeLimit::~dlgTimeLimit()
{
    delete ui;
}

void dlgTimeLimit::on_pushButton_clicked()
{
    hour++;
    countTime();
}

void dlgTimeLimit::countTime()
{
    ui->leHour->setText(QString::number(hour));
    ui->leMins->setText(QString::number(min));
    QDateTime t = QDateTime::currentDateTime();
    t = t.addSecs((hour * 3600) + (min * 6));
    ui->leTime->setText(t.toString("dd.MM.yyyy HH:mm"));
}

void dlgTimeLimit::on_pushButton_2_clicked()
{
    hour = hour > -1 ? hour - 1 : hour;
    countTime();
}

void dlgTimeLimit::on_pushButton_3_clicked()
{
    min += 5;
    countTime();
}

void dlgTimeLimit::on_pushButton_4_clicked()
{
    min = min > 0 ? min - 5 : min ;
    countTime();
}

void dlgTimeLimit::on_pushButton_5_clicked()
{
    accept();
}
