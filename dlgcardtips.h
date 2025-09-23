#ifndef DLGCARDTIPS_H
#define DLGCARDTIPS_H

#include <QDialog>

namespace Ui {
class DlgCardTips;
}

class DlgCardTips : public QDialog
{
    Q_OBJECT

public:
    explicit DlgCardTips(const QString &order, QWidget *parent = nullptr);
    ~DlgCardTips();

private:
    Ui::DlgCardTips *ui;
    QString fOrder;

private slots:
    void cancel();
    void enter();
};

#endif // DLGCARDTIPS_H
