#ifndef DLGREGISTERCARD_H
#define DLGREGISTERCARD_H

#include <QDialog>
#include <QMap>

namespace Ui {
class DlgRegisterCard;
}

class DlgRegisterCard : public QDialog
{
    Q_OBJECT

public:
    explicit DlgRegisterCard(bool v, QWidget *parent = nullptr);
    ~DlgRegisterCard();
    bool view = false;

private slots:
    void on_leSearch_textChanged(const QString &arg1);

    void on_btnExit_clicked();

    void on_kbd_clicked(const QModelIndex &index);

    void on_btnClear_clicked();

    void on_tbl_clicked(const QModelIndex &index);

private:
    Ui::DlgRegisterCard *ui;
    QMap<int,QStringList> m_letters;
};

#endif // DLGREGISTERCARD_H
