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
    void on_btnMinus_clicked();
    void on_btnPlus_clicked();
    void on_btnReject_clicked();
    void on_btnOk_clicked();
    void on_btnRemoveReason_clicked();
    void on_btnClearWindow_clicked();
    void on_btnKeyboard_clicked();
    void on_tblLetter_itemClicked(QTableWidgetItem *item);
    void parseRequestForCorrection(const QString &response, bool result);
    void parseRequestResponse(const QString &response, bool result);
    void on_btnCheckResponse_clicked();

private:
    Ui::DlgCorrection *ui;
    OD_Dish *fDish;
    OD_Drv *m_ord;
    int fRequestId;
    double fQty;
    void setWaitMode();
    void setQtyRemove();
    void printRemoved(double qty);
    QMap<int, QStringList> fLetters;
    void requestForCorrection(double qty);
    void checkResponse(int id);
};

#endif // DLGCORRECTION_H
