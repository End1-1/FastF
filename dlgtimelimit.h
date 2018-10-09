#ifndef DLGTIMELIMIT_H
#define DLGTIMELIMIT_H

#include <QDialog>

namespace Ui {
class dlgTimeLimit;
}

class dlgTimeLimit : public QDialog
{
    Q_OBJECT

public:
    static int hour;
    static int min;
    explicit dlgTimeLimit(QWidget *parent = 0);
    ~dlgTimeLimit();

private slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();

    void on_pushButton_4_clicked();

    void on_pushButton_5_clicked();

private:
    Ui::dlgTimeLimit *ui;
    void countTime();
};

#endif // DLGTIMELIMIT_H
