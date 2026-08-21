#include "dlgqty.h"
#include "ui_dlgqty.h"
#include <QPushButton>

DlgQty::DlgQty(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DlgQty),
    m_max(0)
{
    ui->setupUi(this);
}

DlgQty::~DlgQty()
{
    delete ui;
}

bool DlgQty::qty(double &d, QWidget *parent)
{
    return qty(d, 0, QString(), parent);
}

bool DlgQty::qty(double &d, double max, const QString &caption, QWidget *parent)
{
    DlgQty *dq = new DlgQty(parent);
    dq->m_max = max;
    if(!caption.isEmpty()) {
        dq->setWindowTitle(caption);
    }
    bool result = dq->exec() == QDialog::Accepted;
    if(result) {
        d = dq->ui->leText->text().toDouble();
    }
    delete dq;
    return result;
}

void DlgQty::applyMaxLimit()
{
    if(m_max <= 0) {
        return;
    }
    const double v = ui->leText->text().toDouble();
    if(v > m_max) {
        ui->leText->setText(QString::number(qRound(m_max)));
    }
}

void DlgQty::on_pushButton_clicked()
{
    setText(sender());
}

void DlgQty::setText(QObject *o)
{
    QPushButton *b = static_cast<QPushButton*>(o);
    ui->leText->setText(ui->leText->text() + b->text());
    applyMaxLimit();
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
    applyMaxLimit();
    const double v = ui->leText->text().toDouble();
    if(v < 0.001) {
        return;
    }
    if(m_max > 0) {
        if(v > m_max) {
            ui->leText->setText(QString::number(qRound(m_max)));
        }
    } else if(v > 99) {
        return;
    }
    accept();
}
