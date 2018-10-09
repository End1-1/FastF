#ifndef DLGREMOVEREASON_H
#define DLGREMOVEREASON_H

#include <QDialog>

namespace Ui {
class DlgRemoveReason;
}

class DlgRemoveReason : public QDialog
{
    Q_OBJECT

public:
    explicit DlgRemoveReason(QMap<int, QString> values, QWidget *parent = 0);
    ~DlgRemoveReason();
    int m_reasonId = 0;
    QString m_reasonName = "";

private slots:
    void on_btnOK_clicked();

    void on_btnUp_clicked();

    void on_btnDown_clicked();

private:
    Ui::DlgRemoveReason *ui;
};

#endif // DLGREMOVEREASON_H
