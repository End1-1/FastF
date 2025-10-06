#ifndef DLGINPUT_H
#define DLGINPUT_H

#include <QDialog>

namespace Ui {
class DlgInput;
}

class DlgInput : public QDialog
{
    Q_OBJECT
    
public:
    explicit DlgInput(QWidget *parent = 0);
    ~DlgInput();
    QString text();
    static bool getDouble(double &value, const QString &caption, const double &max = 0, QWidget *parent = 0);
    static bool getString(QString &value, const QString &caption, QWidget *parent);
    void setWindowTitle(const QString &title);

private:
    bool isValueValid();
    
private slots:
    void on_pushButton_2_clicked();
    void on_pushButton_clicked();
    void on_leValue_returnPressed();

private:
    Ui::DlgInput *ui;
};

#endif // DLGINPUT_H
