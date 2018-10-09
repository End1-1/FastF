#include "dlggatreader.h"
#include "ui_dlggatreader.h"

DlgGatReader::DlgGatReader(QWidget *parent) :
    QDialog(parent, Qt::FramelessWindowHint),
    ui(new Ui::DlgGatReader)
{
    ui->setupUi(this);
    m_gat = new GATWriter();
    connect(m_gat, SIGNAL(longTimeout()), this, SLOT(cancelCode()));
    connect(m_gat, SIGNAL(readCode(QString)), this, SLOT(readCode(QString)));
}

DlgGatReader::~DlgGatReader()
{
    m_gat->deleteLater();
    delete ui;
}

void DlgGatReader::readCode(const QString &code)
{
    m_cardCode = code;
    accept();
}

void DlgGatReader::cancelCode()
{
    reject();
}

void DlgGatReader::on_btnCancel_clicked()
{
    reject();
}
