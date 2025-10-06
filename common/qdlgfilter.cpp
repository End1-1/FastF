#include "qdlgfilter.h"
#include "ui_qdlgfilter.h"
#include "qdlgcache.h"
#include <QLabel>
#include <QDateEdit>
#include <QPushButton>

QDlgFilter::QDlgFilter(QList<SqlField*> &fields, QWidget *parent) :
    QDlgBase(parent),
    ui(new Ui::QDlgFilter),
    m_fields(fields)
{
    ui->setupUi(this);
    int layoutRow = 0;
    for (QList<SqlField*>::const_iterator it = fields.begin(); it != fields.end(); it++)
        if ((*it)->m_filter && ((*it)->m_filter->m_filterType != Filter::ftNone))
            addRow(layoutRow++, (*it)->m_filter);
    QPushButton *btnClear = new QPushButton(tr("Clear"), this);
    connect(btnClear, SIGNAL(clicked()), this, SLOT(slotClearFilter()));
    ui->widget->layout()->addWidget(btnClear);
}

QDlgFilter::~QDlgFilter()
{
    delete ui;
}

void QDlgFilter::addRow(int row, Filter *filter)
{
    connect(filter, SIGNAL(viewCache(QString&)), SLOT(viewCache(QString&)));
    ui->grid->addWidget(new QLabel(filter->caption), row, 0);
    ui->grid->addWidget(filter->widget(this), row, 1);
    ui->grid->addWidget(filter->button(this), row, 2);
    ui->grid->addWidget(filter->checkbox(this), row, 3);

    filter->tempNames = filter->listNames;
    filter->tempValues = filter->listValues;
    filter->updateWidget();
}

void QDlgFilter::btnOK()
{
    accept();
}

void QDlgFilter::viewCache(QString &name)
{
    Filter *f = qobject_cast<Filter*>(sender());
    QDlgCache *d = new QDlgCache(name, f->listValues, qobject_cast<QWidget*>(this));
    if (d->exec() == QDialog::Accepted) {
        d->getValues(f->tempNames, f->tempValues);
        f->updateWidget();
    }
    delete d;
}

void QDlgFilter::slotClearFilter()
{
    for (QList<SqlField*>::iterator it = m_fields.begin(); it != m_fields.end(); it++) {
        if ((*it)->m_filter) {
            if ((*it)->m_filter->m_filterType != Filter::ftEdit)
                continue;
            (*it)->m_filter->tempNames = "";
            (*it)->m_filter->tempValues = "";
            (*it)->m_filter->updateWidget();
        }
    }
}
