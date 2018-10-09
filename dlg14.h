#ifndef DLG14_H
#define DLG14_H

#include <QDialog>
#include <QTableWidgetItem>

namespace Ui {
class Dlg14;
}

class Dlg14 : public QDialog
{
    Q_OBJECT

public:
    explicit Dlg14(QWidget *parent = 0);
    ~Dlg14();
    static bool set14(QList<int> &dishes, int &flag);
private slots:
    void on_btnCancel_clicked();

    void on_tblData_itemClicked(QTableWidgetItem *item);

    void on_btnOne_clicked();

    void on_btnTwo_clicked();

private:
    Ui::Dlg14 *ui;
};

#endif // DLG14_H
