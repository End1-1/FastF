#ifndef DLGLIST_H
#define DLGLIST_H

#include <QDialog>
#include <QVariant>

namespace Ui {
class DlgList;
}

class DlgList : public QDialog
{
    Q_OBJECT

public:
    explicit DlgList(QWidget *parent = 0);
    ~DlgList();
    static bool value(const QMap<QString, QVariant> data, QVariant &out, QWidget *parent = 0);

private slots:
    void on_btnCancel_clicked();
    void on_btnOK_clicked();

private:
    Ui::DlgList *ui;
    QVariant m_result;
};

#endif // DLGLIST_H
