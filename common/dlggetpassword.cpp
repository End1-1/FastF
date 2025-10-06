#include "dlggetpassword.h"
#include "ui_dlggetpassword.h"
#include "wdtnumpad.h"

DlgGetPassword::DlgGetPassword(QWidget *parent) :
    QDialog(parent, Qt::FramelessWindowHint),
    ui(new Ui::DlgGetPassword)
{
    ui->setupUi(this);
    connect(ui->widget, SIGNAL(keyEnter()), SLOT(keyEnter()));
    connect(ui->widget, SIGNAL(cancel()), SLOT(reject()));
    QFont f(qApp->font());
    f.setPointSize(f.pointSize() + 4);
    ui->lbText->setFont(f);
}

DlgGetPassword::~DlgGetPassword()
{
    delete ui;
}

bool DlgGetPassword::password(const QString &caption, QString &out, bool echopass, QWidget *parent)
{
    bool result = false;
    DlgGetPassword *d = new DlgGetPassword(parent);
    if (!echopass) {
        d->ui->widget->setNoPassEcho();
    }
    d->setWindowTitle(caption);
    if (d->exec() == QDialog::Accepted)  {
        out = d->property("password").toString();
        out.replace("?", "");
        out.replace(";", "");
        result = true;
    }
    delete d;
    return result;
}

void DlgGetPassword::setWindowTitle(const QString &title)
{
    ui->lbText->setText(title);
}

void DlgGetPassword::keyEnter()
{
    setProperty("password", (qobject_cast<WdtNumpad*>(ui->widget))->password());
    accept();
}
