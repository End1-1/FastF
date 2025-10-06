#ifndef QDLGCACHE_H
#define QDLGCACHE_H

#include <QDialog>
#include <QTableWidgetItem>

namespace Ui {
class QDlgCache;
}

class QDlgCache : public QDialog
{
    Q_OBJECT

public:
    explicit QDlgCache(const QString &cacheName, const QString &selection, QWidget *parent = 0);
    ~QDlgCache();
    void getValues(QString &names, QString &values);

private slots:
    void on_btnCancel_clicked();
    void on_btnClear_clicked();
    void on_btnOk_clicked();
    void on_listWidget_clicked(const QModelIndex &index);

private:
    Ui::QDlgCache *ui;
    QList<QTableWidgetItem*> data;
};

#endif // QDLGCACHE_H
