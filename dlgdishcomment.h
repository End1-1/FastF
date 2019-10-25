#ifndef DLGDISHCOMMENT_H
#define DLGDISHCOMMENT_H

#include <QDialog>
#include <QItemDelegate>
#include <QPainter>
#include "qsqldrv.h"

#define U(X) QString::fromUtf8(X)

namespace Ui {
class DlgDishComment;
}

class DlgDishComment : public QDialog
{
    Q_OBJECT
    
public:

    class QLetterDelegate : public QItemDelegate {
    protected:
        void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    };

    explicit DlgDishComment(QWidget *parent = 0);
    ~DlgDishComment();
    QString result();
    void setSingleSelection();
    QMap<int,QStringList> m_letters;
    
private slots:
    void on_pushButton_2_clicked();
    void on_pushButton_clicked();
    void on_pushButton_3_clicked();
    void on_pushButton_4_clicked();
    void on_tableWidget_clicked(const QModelIndex &index);
    void on_pushButton_5_clicked();
    void on_tblLetter_clicked(const QModelIndex &index);
    void on_pushButton_6_clicked();

private:
    Ui::DlgDishComment *ui;
    QSqlDrv *m_sqlDrv;
};

#endif // DLGDISHCOMMENT_H
