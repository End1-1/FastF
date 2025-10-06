#include "qdlgcache.h"
#include "qsqlcache.h"
#include "ui_qdlgcache.h"

QDlgCache::QDlgCache(const QString &cacheName, const QString &selection, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::QDlgCache)
{
    ui->setupUi(this);
    show();
    hide();
    qApp->processEvents();
    QMap<int, QString> &c = QSqlCache().getCache(cacheName);

    QStringList sel = selection.split(",");
    for (QMap<int, QString>::const_iterator i = c.begin(); i != c.end(); i++) {
        QListWidgetItem *l = new QListWidgetItem(ui->listWidget);
        l->setFlags(l->flags() | Qt::ItemIsUserCheckable);
        l->setText(i.value());
        l->setData(Qt::UserRole, i.key());
        if (sel.contains(i.value()))
            l->setCheckState(Qt::Checked);
        else
            l->setCheckState(Qt::Unchecked);
        ui->listWidget->addItem(l);
    }
    ui->listWidget->sortItems();
}

QDlgCache::~QDlgCache() {

}

void QDlgCache::getValues(QString &names, QString &values)
{
    names.clear();
    values.clear();

    QList<QListWidgetItem*> selectedItems;
    for (int i = 0; i < ui->listWidget->count(); i++) {
        QListWidgetItem *l = ui->listWidget->item(i);
        if (l->checkState() == Qt::Checked)
            selectedItems.append(l);
    }

    bool first = true;
    for (QList<QListWidgetItem*>::const_iterator i = selectedItems.begin(); i != selectedItems.end(); i++) {
        if (first)
            first = false;
        else {
            names.append(",");
            values.append(",");
        }
        names.append((*i)->text());
        values.append((*i)->data(Qt::UserRole).toString());
    }
}

void QDlgCache::on_btnCancel_clicked()
{
    reject();
}

void QDlgCache::on_btnClear_clicked()
{
    for (int i = 0; i < ui->listWidget->count(); i++) {
        QListWidgetItem *l = ui->listWidget->item(i);
        l->setCheckState(Qt::Unchecked);
    }
}

void QDlgCache::on_btnOk_clicked()
{
    accept();
}

void QDlgCache::on_listWidget_clicked(const QModelIndex &index)
{
    if (!index.isValid())
        return;
    QListWidgetItem *i = ui->listWidget->item(index.row());
    if (!i)
        return;
    Qt::CheckState cs = i->checkState() == Qt::Checked ? Qt::Unchecked : Qt::Checked;
    i->setCheckState(cs);
}
