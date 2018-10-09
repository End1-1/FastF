#include "dlg14.h"
#include "ui_dlg14.h"
#include "qsqldrv.h"

#define result_1 1
#define result_05 2

Dlg14::Dlg14(QWidget *parent) :
    QDialog(parent, Qt::FramelessWindowHint),
    ui(new Ui::Dlg14)
{
    ui->setupUi(this);
    showFullScreen();
    qApp->processEvents();
    ui->tblData->setColumnCount(4);
    QSqlDrv drv("FFADMIN", "main");
    if (!drv.openDB()) {
        qDebug() << "Cannot open db. Dlg14";
        return;
    }
    drv.prepare("select id, name, type_id from me_dishes where type_id in (154, 155,156,157)");
    drv.execSQL();
    QMap<int, int> rows;
    while (drv.next()) {
        int id = drv.val().toInt();
        QString name = drv.val().toString();
        int type_id = drv.val().toInt();
        int c = type_id - 154;
        if (c > 3) {
            c -= 3;
        }
        if (!rows.contains(c)) {
            rows[c] = 0;
        }
        int r = rows[c];
        if (r + 1 > ui->tblData->rowCount()) {
            ui->tblData->setRowCount(r + 1);
        }
        QTableWidgetItem *item = new QTableWidgetItem(name);
        item->setData(Qt::UserRole, id);
        ui->tblData->setItem(r, c, item);
        rows[c] = r + 1;
    }
    drv.close();
}

Dlg14::~Dlg14()
{
    delete ui;
}

bool Dlg14::set14(QList<int> &dishes, int &flag)
{
    bool result;
    Dlg14 *d = new Dlg14();
    if (flag > 0) {
        d->ui->btnOne->setEnabled(flag == 1);
        d->ui->btnTwo->setEnabled(flag == 2);
    }

    QItemSelection is;
    foreach (int did, dishes) {
        for (int r = 0; r < d->ui->tblData->rowCount(); r++) {
            for (int c = 0; c < d->ui->tblData->columnCount(); c++) {
                QTableWidgetItem *item = d->ui->tblData->item(r, c);
                if (!item) {
                    continue;
                }
                if (did == item->data(Qt::UserRole).toInt()) {
                    item->setData(Qt::UserRole + 1, 1);
                    d->ui->tblData->selectionModel()->select(d->ui->tblData->model()->index(r, c), QItemSelectionModel::Select);
                    is.merge(d->ui->tblData->selectionModel()->selection(), QItemSelectionModel::Select);
                }
            }
        }
    }
    d->ui->tblData->selectionModel()->clear();
    d->ui->tblData->selectionModel()->select(is, QItemSelectionModel::Select);

    flag = d->exec();
    result = flag != 0;
    if (result) {
        QModelIndexList mil = d->ui->tblData->selectionModel()->selectedIndexes();
        if (mil.count() == 0) {
            result = false;
        } else {
            dishes.clear();
            for (int i = 0; i < mil.count(); i++) {
                if (mil.at(i).data(Qt::UserRole).toInt() == 0) {
                    continue;
                }
                dishes.append(mil.at(i).data(Qt::UserRole).toInt());
            }
        }
    }
    delete d;
    return result;
}

void Dlg14::on_btnCancel_clicked()
{
    reject();
}

void Dlg14::on_tblData_itemClicked(QTableWidgetItem *item)
{
    if (!item) {
        return;
    }
    QModelIndexList m = ui->tblData->selectionModel()->selectedIndexes();
    /*int i = 0;
    while (m.count() > 4) {
        if (m.at(i).data(Qt::UserRole + 1).toInt() > 0) {
            i++;
            continue;
        }
        m.removeAt(i);
    }
    */
    ui->tblData->selectionModel()->clear();
    QItemSelection is;
    for (int i = 0; i < m.count(); i++) {
        ui->tblData->selectionModel()->select(m.at(i), QItemSelectionModel::Select);
        is.merge(ui->tblData->selectionModel()->selection(), QItemSelectionModel::Select);
    }
    ui->tblData->selectionModel()->select(is, QItemSelectionModel::Select);
}

void Dlg14::on_btnOne_clicked()
{
    done(result_1);
}

void Dlg14::on_btnTwo_clicked()
{
    done(result_05);
}
