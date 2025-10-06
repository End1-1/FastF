#ifndef FBTNOKCANCEL_H
#define FBTNOKCANCEL_H

#include <QWidget>

namespace Ui {
class FBtnOkCancel;
}

class FBtnOkCancel : public QWidget
{
    Q_OBJECT

public:
    explicit FBtnOkCancel(QWidget *parent = 0);
    ~FBtnOkCancel();

private slots:
    void on_btnCancel_clicked();
    void on_btnOK_clicked();

private:
    Ui::FBtnOkCancel *ui;
};

#endif // FBTNOKCANCEL_H
