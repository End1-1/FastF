#ifndef QDLGFILTER_H
#define QDLGFILTER_H

#include "qdlgbase.h"

namespace Ui {
class QDlgFilter;
}

class QDlgFilter : public QDlgBase
{
    Q_OBJECT

public:

    explicit QDlgFilter(QList<SqlField*> &fields, QWidget *parent = 0);
    ~QDlgFilter();

private:
    Ui::QDlgFilter *ui;
    QList<SqlField*> &m_fields;
    void addRow(int row, Filter *filter);
    virtual void btnOK();

private slots:
    void viewCache(QString &name);
    void slotClearFilter();
};

#endif // QDLGFILTER_H
