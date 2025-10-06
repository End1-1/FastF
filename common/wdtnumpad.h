#ifndef WDTNUMPAD_H
#define WDTNUMPAD_H

#include <QWidget>

namespace Ui {
class WdtNumpad;
}

class WdtNumpad : public QWidget
{
    Q_OBJECT
    
public:
    explicit WdtNumpad(QWidget *parent = 0);
    ~WdtNumpad();
    const QString password();
    void setNoPassEcho();
    void configDouble();

private slots:
    void press(const QString &str);
    void on_pushButton_clicked();
    void on_pushButton_5_clicked();
    void on_pushButton_6_clicked();
    void on_pushButton_13_clicked();
    void on_pushButton_2_clicked();
    void on_pushButton_7_clicked();
    void on_pushButton_10_clicked();
    void on_pushButton_14_clicked();
    void on_pushButton_3_clicked();
    void on_pushButton_8_clicked();
    void on_pushButton_11_clicked();
    void on_pushButton_15_clicked();
    void on_btnEnter_clicked();
    void on_pushButton_4_clicked();
    void on_lePassword_returnPressed();

private:
    Ui::WdtNumpad *ui;
    QString m_string;

signals:
    void key(const QString &str);
    void keyEnter();
    void cancel();
};

#endif // WDTNUMPAD_H
