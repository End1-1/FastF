#include "dlgsplash.h"
#include "ui_dlgsplash.h"
#include <QMovie>
#include "qsystem.h"

DlgSplash::DlgSplash(QWidget *parent) :
    QDialog(parent, Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint),
    ui(new Ui::DlgSplash)
{
    ui->setupUi(this);
    ui->lbVersion->setText(getVersionString(qApp->applicationFilePath()));
    /*
    QMovie *m = new QMovie(":/res/loading.gif");
    ui->lbMovie->setMovie(m);
    m->start();
    */
}

DlgSplash::~DlgSplash()
{
    delete ui;
}

void DlgSplash::setText(const QString &text)
{
    ui->label->setText(text);
    qApp->processEvents();
}
