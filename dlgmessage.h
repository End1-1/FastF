#ifndef DLGMESSAGE_H
#define DLGMESSAGE_H

#include <QDialog>

#define msg(x) DlgMessage::Msg(x)

namespace Ui {
class DlgMessage;
}

class DlgMessage : public QDialog
{
    Q_OBJECT

public:
    explicit DlgMessage(QWidget *parent = 0);
    ~DlgMessage();

    static int Msg(const QString &message, bool merci = false);

private slots:
    void on_btnClose_clicked();
    void on_btnCancel_clicked();

private:
    Ui::DlgMessage *ui;
};

#endif // DLGMESSAGE_H
