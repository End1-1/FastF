#include "configmobilewidget.h"
#include "ui_configmobilewidget.h"

ConfigMobileWidget::ConfigMobileWidget(const QPixmap &pix, const QString &name, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ConfigMobileWidget)
{
    ui->setupUi(this);
    ui->lbImage->setPixmap(pix);
    ui->lbName->setText(name);
}

ConfigMobileWidget::~ConfigMobileWidget()
{
    delete ui;
}
