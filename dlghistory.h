#ifndef DLGHISTORY_H
#define DLGHISTORY_H

#include <QDialog>

namespace Ui {
class DlgHistory;
}

class DlgHistory : public QDialog
{
    Q_OBJECT

public:
    explicit DlgHistory(QString orderId, const QString &username, QWidget *parent = 0);
    ~DlgHistory();

private slots:
    void on_btnOK_clicked();

private:
    Ui::DlgHistory *ui;
};

#endif // DLGHISTORY_H
