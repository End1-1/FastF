#include "dlgcalcchange.h"
#include "ui_dlgcalcchange.h"
#include <time.h>

DlgCalcChange::DlgCalcChange(float amount, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DlgCalcChange),
    m_amount(amount)
{
    ui->setupUi(this);
    ui->leAmount->setText(QString::number(m_amount, 'f', 0));
    calcChange();
    calcNearAmount();
    connect(this, SIGNAL(btnClicked(QString)), this, SLOT(btnNumClicked(QString)));
    showFullScreen();
}

DlgCalcChange::~DlgCalcChange()
{
    delete ui;
}

void DlgCalcChange::btnNumClicked(const QString &text)
{
    if (ui->leMoney->text() == "0") {
        if (text == "0")
            return;
        ui->leMoney->setText(text);
        return;
    }
    ui->leMoney->setText(ui->leMoney->text() + text);
    calcChange();
}

void DlgCalcChange::on_pushButton_9_clicked()
{
    emit btnClicked(qobject_cast<QPushButton*>(sender())->text());
}

void DlgCalcChange::calcChange()
{
    ui->leChange->setText(QString::number(ui->leMoney->text().toDouble() - m_amount, 'f', 0));
}

void DlgCalcChange::calcNearAmount()
{
    /*
     * < 1000 -> 1000, 5000, 10000, 20000
     * < 5000 -> 5000, 10000, 20000,
     * < 10000 -> 10000, 20000
     * < 20000 -> 20000,
     * > 20000 ->
     *

     float banknotes[7] = {1000, 2000, 3000, 4000, 5000, 10000, 20000};
     float amount_left = m_amount;
     while (amount_left > 0) {

     }
     */
}

void DlgCalcChange::on_pushButton_12_clicked()
{
    ui->leMoney->setText("0");
    calcChange();
}

void DlgCalcChange::on_pushButton_19_clicked()
{
    emit btnClicked(qobject_cast<QPushButton*>(sender())->text());
}

void DlgCalcChange::on_pushButton_23_clicked()
{
    emit btnClicked(qobject_cast<QPushButton*>(sender())->text());
}

void DlgCalcChange::on_pushButton_10_clicked()
{
    emit btnClicked(qobject_cast<QPushButton*>(sender())->text());
}

void DlgCalcChange::on_pushButton_11_clicked()
{
    emit btnClicked(qobject_cast<QPushButton*>(sender())->text());
}

void DlgCalcChange::on_pushButton_13_clicked()
{
    emit btnClicked(qobject_cast<QPushButton*>(sender())->text());
}

void DlgCalcChange::on_pushButton_17_clicked()
{
    emit btnClicked(qobject_cast<QPushButton*>(sender())->text());
}

void DlgCalcChange::on_pushButton_18_clicked()
{
    emit btnClicked(qobject_cast<QPushButton*>(sender())->text());
}

void DlgCalcChange::on_pushButton_14_clicked()
{
    emit btnClicked(qobject_cast<QPushButton*>(sender())->text());
}

void DlgCalcChange::on_pushButton_15_clicked()
{
    emit btnClicked(qobject_cast<QPushButton*>(sender())->text());
}

void DlgCalcChange::on_pushButton_16_clicked()
{
    accept();
}

void DlgCalcChange::on_btn1_clicked()
{
    ui->leMoney->setText(qobject_cast<QPushButton*>(sender())->text());
    calcChange();
}

void DlgCalcChange::on_btn2_clicked()
{
    ui->leMoney->setText(qobject_cast<QPushButton*>(sender())->text());
    calcChange();
}

void DlgCalcChange::on_btn3_clicked()
{
    ui->leMoney->setText(qobject_cast<QPushButton*>(sender())->text());
    calcChange();
}

void DlgCalcChange::on_btn4_clicked()
{
    ui->leMoney->setText(qobject_cast<QPushButton*>(sender())->text());
    calcChange();
}

void DlgCalcChange::on_btn6_clicked()
{
    srand(time(0));
    float r = 0;
    while (r < m_amount + 20000)
        r += rand() * 100;
    ui->leMoney->setText(QString::number(r, 'f', 0));
    calcChange();
}
