#ifndef QDLGBASE_H
#define QDLGBASE_H

#include "qbasesqldriver.h"
#include "qsystem.h"
#include <QDialog>
#include <QMessageBox>
#include <QVariant>


class QDlgBase : public QDialog, public QBaseSqlDriver
{
    Q_OBJECT

private:

public:
    explicit QDlgBase(QWidget *parent = 0);
    ~QDlgBase();

signals:

public slots:
    virtual void btnOK() = 0;

protected:

};

#endif // QDLGBASE_H
