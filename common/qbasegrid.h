#ifndef QBASEGRID_H
#define QBASEGRID_H

#include "qsystem.h"
#include "qbasesqldriver.h"
#include "qbasesqlwindow.h"
#include <QHeaderView>
#include <QPainter>
#include <QTextOption>
#include <QAction>
#include <QMouseEvent>
#include <QItemDelegate>
#include "printing.h"

namespace Ui {
class QBaseGrid;
}

class QBaseGrid : public QBaseSqlWindow, public QBaseSqlDriver
{
    Q_OBJECT

public:
    class CustomHeader : public QHeaderView {
    public:
        QStringList m_captions;
        CustomHeader(Qt::Orientation orientation, QWidget *parent = 0);
        QString caption(const int &index) const;
        int columnIndex(const QString &name);
    protected:
        void paintSection(QPainter * painter, const QRect & rect, int logicalIndex ) const;
    };

    class ItemDelegate : public QItemDelegate {
    protected:
        virtual void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    };

public:
    explicit QBaseGrid(QWidget *parent = 0);
    ~QBaseGrid();
    void brindToFrom();
    virtual void activateToolbarButtons(const QList<QAction *> &actions);
    virtual void actionNew();
    virtual void actionEdit();
    virtual void actionBeforeSelect();
    virtual void actionSelect();
    virtual void actionRefresh();
    virtual bool actionFilter();
    virtual void actionExcel(QString fileName = "");
    virtual void actionMultiSelect(QToolBar *tb);
    virtual void actionPrintPreview();
    virtual void actionPrint();

private:
    Ui::QBaseGrid *ui;
    QString m_lastPrinter;
    void search();

protected:
    QString m_editDlgTable;
    QModelIndexList selectedRows();
    QVariant itemData(int row, int col, Qt::ItemDataRole role = Qt::DisplayRole);
    QStringList m_totalRowTitles;
    QMap<int, double> m_totalRowValues;
    void setItemData(int row, int col, QVariant data, Qt::ItemDataRole role = Qt::DisplayRole);
    int appendRow();
    void removeRow(int row);
    virtual bool eventFilter(QObject *o, QEvent *e);
    void remove_zap(QString &src);
    void remove_and(QString &src);
    int groupFieldsCount();
    void setCaptions();
    void setCaptions(const QStringList &captions, const QList<int> widths);
    void setColCaption(int col, const QString &caption);
    QTableWidget *tableWidget(int which);
    int columnIndex(const QString &name);
    int columnWidth(int columnIndex);
    int columnsCount();
    void setColumnWidth(int column, int width);
    void setHeaderHeight(int height);
    int selectedRows(QSet<int> &rows);
    QVariant cellValue(int row, int col);
    void setCellValue(int row, int col, const QVariant &value);
    void setItemDelegate(QItemDelegate *d);
    void createSimpleIdNameTable(const QString &tableName);
    bool confirmDeletion();
    SqlField *addField(const QString &fieldName, const QString &fieldTitle, int width, QVariant::Type type);
    virtual void getPrintData(XmlPrintMaker &x);
    void exportToExcel(const QString &fileName);

private slots:
    virtual void slotGridHeaderClicked(int index);
    void slotGridSectionResized(int index, int oldSize, int newSize);
    void slotScrollBarValueChanged(int newValue);
    void slotSelectAll();
    void slotDeselectAll();
    virtual void on_grid_doubleClicked(const QModelIndex &index);
};

#endif // QBASEGRID_H
