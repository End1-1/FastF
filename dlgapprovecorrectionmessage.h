#ifndef DLGAPPROVECORRECTIONMESSAGE_H
#define DLGAPPROVECORRECTIONMESSAGE_H

#include <QDialog>

namespace Ui {
class DlgApproveCorrectionMessage;
}

class DlgApproveCorrectionMessage : public QDialog
{
    Q_OBJECT

public:
    explicit DlgApproveCorrectionMessage(const QJsonArray &ar, bool approve, QWidget *parent = nullptr);
    ~DlgApproveCorrectionMessage();

private slots:
    void on_btnClose_clicked();

private:
    Ui::DlgApproveCorrectionMessage *ui;
};

#endif // DLGAPPROVECORRECTIONMESSAGE_H
