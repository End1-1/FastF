#ifndef DLGQTY_H
#define DLGQTY_H

#include <QDialog>

namespace Ui {
class DlgQty;
}

class DlgQty : public QDialog
{
    Q_OBJECT

public:
    explicit DlgQty(QWidget *parent = nullptr);
    ~DlgQty();
    static bool qty(double &d, QWidget *parent = nullptr);

private slots:
    void on_pushButton_clicked();
    void on_pushButton_11_clicked();
    void on_pushButton_2_clicked();
    void on_pushButton_3_clicked();
    void on_pushButton_5_clicked();
    void on_pushButton_4_clicked();
    void on_pushButton_6_clicked();
    void on_pushButton_8_clicked();
    void on_pushButton_9_clicked();
    void on_pushButton_7_clicked();
    void on_pushButton_10_clicked();
    void on_pushButton_12_clicked();

private:
    Ui::DlgQty *ui;
    void setText(QObject *o);
};

#endif // DLGQTY_H
