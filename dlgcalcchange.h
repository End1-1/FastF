#ifndef DLGCALCCHANGE_H
#define DLGCALCCHANGE_H

#include <QDialog>

namespace Ui {
class DlgCalcChange;
}

class DlgCalcChange : public QDialog
{
    Q_OBJECT

public:
    explicit DlgCalcChange(float amount, QWidget *parent = 0);
    ~DlgCalcChange();

private slots:
    void btnNumClicked(const QString &text);
    void on_pushButton_9_clicked();
    void on_pushButton_12_clicked();
    void on_pushButton_19_clicked();
    void on_pushButton_23_clicked();
    void on_pushButton_10_clicked();
    void on_pushButton_11_clicked();
    void on_pushButton_13_clicked();
    void on_pushButton_17_clicked();
    void on_pushButton_18_clicked();
    void on_pushButton_14_clicked();
    void on_pushButton_15_clicked();
    void on_pushButton_16_clicked();
    void on_btn1_clicked();
    void on_btn2_clicked();
    void on_btn3_clicked();
    void on_btn4_clicked();
    void on_btn6_clicked();

private:
    Ui::DlgCalcChange *ui;
    float m_amount;
    void calcChange();
    void calcNearAmount();

signals:
    void btnClicked(const QString &text);
};

#endif // DLGCALCCHANGE_H
