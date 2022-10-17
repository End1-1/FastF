#ifndef DLGCONFIGMOBILE_H
#define DLGCONFIGMOBILE_H

#include <QDialog>

namespace Ui {
class DlgConfigMobile;
}

class DlgConfigMobile : public QDialog
{
    Q_OBJECT

public:
    explicit DlgConfigMobile(QWidget *parent = nullptr);
    ~DlgConfigMobile();

private slots:
    void on_btnClose_clicked();

private:
    Ui::DlgConfigMobile *ui;
};

#endif // DLGCONFIGMOBILE_H
