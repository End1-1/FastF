#include "dlgqty.h"
#include "ui_dlgqty.h"

DlgQty::DlgQty(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DlgQty)
{
    ui->setupUi(this);
}

DlgQty::~DlgQty()
{
    delete ui;
}

bool DlgQty::qty(double &d, QWidget *parent)
{
    DlgQty *dq = new DlgQty(parent);
    bool result = dq->exec() == QDialog::Accepted;
    if (result) {
        d = dq->ui->leText->text().toDouble();
    }
    return result;
}

void DlgQty::on_pushButton_clicked()
{
    setText(sender());
}

void DlgQty::setText(QObject *o)
{
    QPushButton *b = static_cast<QPushButton*>(o);
    ui->leText->setText(ui->leText->text() + b->text());
}

void DlgQty::on_pushButton_11_clicked()
{
    reject();
}

void DlgQty::on_pushButton_2_clicked()
{
    setText(sender());
}

void DlgQty::on_pushButton_3_clicked()
{
    setText(sender());
}

void DlgQty::on_pushButton_5_clicked()
{
    setText(sender());
}

void DlgQty::on_pushButton_4_clicked()
{
    setText(sender());
}

void DlgQty::on_pushButton_6_clicked()
{
    setText(sender());
}

void DlgQty::on_pushButton_8_clicked()
{
    setText(sender());
}

void DlgQty::on_pushButton_9_clicked()
{
    setText(sender());
}

void DlgQty::on_pushButton_7_clicked()
{
    setText(sender());
}

void DlgQty::on_pushButton_10_clicked()
{
    setText(sender());
}

void DlgQty::on_pushButton_12_clicked()
{
    if (ui->leText->text().toDouble() < 0.001) {
        return;
    }
    if (ui->leText->text().toDouble() > 99) {
        return;
    }
    accept();
}
