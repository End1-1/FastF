#ifndef DLGKINOPARKCALL_H
#define DLGKINOPARKCALL_H

#include <QDialog>

namespace Ui {
class dlgKinoParkCall;
}

class dlgKinoParkCall : public QDialog
{
    Q_OBJECT

public:
    explicit dlgKinoParkCall(bool newOrd, bool callStaff, const QString &tableName, QWidget *parent = 0);
    ~dlgKinoParkCall();

private slots:
    void on_btnClose_clicked();

private:
    Ui::dlgKinoParkCall *ui;
};

#endif // DLGKINOPARKCALL_H
