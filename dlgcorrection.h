#ifndef DLGCORRECTION_H
#define DLGCORRECTION_H

#include <QDialog>
#include <QMap>
#include <QTableWidgetItem>

namespace Ui {
class DlgCorrection;
}

class OD_Dish;
class OD_Drv;

class DlgCorrection : public QDialog
{
    Q_OBJECT

public:
    explicit DlgCorrection(OD_Drv *drv, OD_Dish *d, QWidget *parent = nullptr);
    ~DlgCorrection();

private slots:
    void on_btnReject_clicked();
    void on_btnOk_clicked();
    void on_btnRemoveReason_clicked();
    void on_btnClearWindow_clicked();
    void on_btnKeyboard_clicked();
    void on_tblLetter_itemClicked(QTableWidgetItem *item);
    void on_btnCheckResponse_clicked();
    void on_btnLossYes_clicked();
    void on_btnLossNo_clicked();

private:
    Ui::DlgCorrection *ui;
    OD_Dish *fDish;
    OD_Drv *m_ord;
    int fRequestId;
    void setEnabledWidget(bool v);
    void setWaitMode();
    void printRemoved(double qty);
    QMap<int, QStringList> fLetters;
    void requestForCorrection(double qty);
};

#endif // DLGCORRECTION_H
