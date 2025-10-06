#include "dlginput.h"
#include "ui_dlginput.h"
#include <QDoubleValidator>

DlgInput::DlgInput(QWidget *parent) :
    QDialog(parent, Qt::FramelessWindowHint),
    ui(new Ui::DlgInput)
{
    ui->setupUi(this);
    QFont font(qApp->font());
    font.setPointSize(22);
    ui->leValue->setFont(font);
    ui->lbCaption->setFont(font);
    ui->lbCaption->setText("");
    ui->pushButton->setIcon(QIcon(":/res/ok.png"));
    ui->pushButton_2->setIcon(QIcon(":/res/cancel.png"));
}

DlgInput::~DlgInput()
{
    delete ui;
}

QString DlgInput::text()
{
    return ui->leValue->text();
}

bool DlgInput::getDouble(double &value, const QString &caption, const double &max, QWidget *parent)
{
    bool result = false;
    value = 0;
    DlgInput *dlg = new DlgInput(parent);
    dlg->ui->leValue->setEchoMode(QLineEdit::Normal);
    dlg->ui->leValue->setValidator(new QDoubleValidator(1, max, 2, dlg));
    dlg->ui->lbCaption->setText(caption);
    if (dlg->exec() == QDialog::Accepted) {
        result = true;
        value = dlg->ui->leValue->text().toDouble();
    }
    delete dlg;
    return result;
}

bool DlgInput::getString(QString &value, const QString &caption, QWidget *parent)
{
    bool result = false;
    value = "";
    DlgInput *d = new DlgInput(parent);
    d->setWindowTitle(caption);
    if (d->exec() == QDialog::Accepted) {
        value = d->text();
        result = true;
    }
    delete d;
    return result;
}

void DlgInput::setWindowTitle(const QString &title)
{
    ui->lbCaption->setText(title);
    QDialog::setWindowTitle(title);
}

bool DlgInput::isValueValid()
{
    return true;//return ui->leValue->validator()->validate(ui->leValue->text(), 0) == Accepted;
}

void DlgInput::on_pushButton_2_clicked()
{
    reject();
}

void DlgInput::on_pushButton_clicked()
{
    ui->leValue->setText(ui->leValue->text().trimmed());
    if (!ui->leValue->text().length())
        return;
    accept();
}

void DlgInput::on_leValue_returnPressed()
{
    if (isValueValid())
        on_pushButton_clicked();
}
