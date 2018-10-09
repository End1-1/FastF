#ifndef DLGGATREADER_H
#define DLGGATREADER_H

#include <QDialog>
#include "gatwriter.h"

namespace Ui {
class DlgGatReader;
}

class DlgGatReader : public QDialog
{
    Q_OBJECT

public:
    explicit DlgGatReader(QWidget *parent = 0);
    ~DlgGatReader();
    QString m_cardCode;

private slots:
    void readCode(const QString &code);
    void cancelCode();
    void on_btnCancel_clicked();

private:
    Ui::DlgGatReader *ui;
    GATWriter *m_gat;
};

#endif // DLGGATREADER_H
