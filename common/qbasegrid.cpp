#include "qbasegrid.h"
#include "ui_qbasegrid.h"
#include "qdlgfilter.h"
#include "xlsxall.h"
#include <QSqlRecord>
#include <QScrollBar>
#include <QSqlField>
#include <QMessageBox>
#include <QAbstractButton>
#include <QLibrary>
#include <QSaveFile>
#include <QDir>
#include <QFileDialog>
#include "printpreview.h"

QBaseGrid::QBaseGrid(QWidget *parent) :
    QBaseSqlWindow(parent),
    ui(new Ui::QBaseGrid)
{
    ui->setupUi(this);
    CustomHeader *h = new CustomHeader(Qt::Horizontal, this);
    connect(h, SIGNAL(sectionClicked(int)), SLOT(slotGridHeaderClicked(int)));
    connect(h, SIGNAL(sectionResized(int,int,int)), SLOT(slotGridSectionResized(int, int, int)));
    connect(ui->gridTotal->horizontalScrollBar(), SIGNAL(valueChanged(int)), SLOT(slotScrollBarValueChanged(int)));
    ui->grid->setHorizontalHeader(h);
    ui->gridTotal->setRowCount(1);

    QAbstractButton *btn = ui->grid->findChild<QAbstractButton*>();
    btn->disconnect();
    connect(btn, SIGNAL(clicked()), this, SLOT(slotSelectAll()));
    ui->grid->installEventFilter(this);
    ui->grid->setItemDelegate(new ItemDelegate());
}

QBaseGrid::~QBaseGrid()
{
    delete ui;
}

void QBaseGrid::brindToFrom()
{
    setWindowState((windowState() & ~Qt::WindowMinimized) | Qt::WindowActive);
    activateWindow();
    setFocus();
}

void QBaseGrid::activateToolbarButtons(const QList<QAction *> &actions)
{
    MdiWindow::activateToolbarButtons(actions);
    if (!actions.count())
        return;
    actions.at(ACTION_NEW)->setVisible(true);
    actions.at(ACTION_EDIT)->setVisible(true);
    actions.at(ACTION_PRINT_PREVIEW)->setVisible(true);
    actions.at(ACTION_PRINT)->setVisible(true);
    actions.at(ACTION_EXTPORT_TO_EXCEL)->setVisible(true);
    actions.at(ACTION_REFRESH)->setVisible(true);
}

void QBaseGrid::actionNew()
{

}

void QBaseGrid::actionEdit()
{

}

void QBaseGrid::actionBeforeSelect()
{
    actionSelect();
}

void QBaseGrid::actionSelect()
{
    ui->grid->setSortingEnabled(false);
    ui->grid->clearContents();
    ui->gridTotal->clearContents();

    int colCount = groupFieldsCount();
    ui->grid->setColumnCount(colCount);
    ui->gridTotal->setColumnCount(colCount);
    ui->gridTotal->setRowCount(1);
    setCaptions();
    m_totalRowValues.clear();
    for (QStringList::const_iterator it = m_totalRowTitles.begin(); it != m_totalRowTitles.end(); it++)
        m_totalRowValues.insert(columnIndex(*it), 0);

    if (!m_sqlDriver->prepare(m_sqlString + whereClause()))
        return;
    if (!m_sqlDriver->execSQL())
        return;

    int row = 0, col = m_sqlDriver->m_query->record().count();
    for (int i = 0; i < col; i++) {
        m_fields[fieldIndexByName(m_sqlDriver->m_query->record().fieldName(i))]->dataType = m_sqlDriver->m_query->record().field(i).type();
    }

    ui->grid->setRowCount(500);
    while (m_sqlDriver->m_query->next()) {
        for (int i = 0; i < col; i++) {
            QTableWidgetItem *item = new QTableWidgetItem();
            QVariant v = m_sqlDriver->m_query->value(i);
            switch (v.type()) {
            case QVariant::Double:
                item->setData(Qt::DisplayRole, dts(v.toDouble()));
                break;
            default:
                item->setData(Qt::DisplayRole, v);
                break;
            }
            ui->grid->setItem(row, i, item);
        }
        for (QMap<int, double>::iterator it = m_totalRowValues.begin(); it != m_totalRowValues.end(); it++) {
            it.value() += m_sqlDriver->m_query->value(it.key()).toDouble();
        }
        row++;
        if (row >= ui->grid->rowCount())
            ui->grid->setRowCount(row + 500);
    }
    m_sqlDriver->close();
    ui->grid->setRowCount(row);
    ui->gridTotal->setVerticalHeaderLabels(QStringList() << QString("%1").arg(row));
    for (QMap<int, double>::const_iterator it = m_totalRowValues.begin(); it != m_totalRowValues.end(); it++)
        ui->gridTotal->setItem(0, it.key(), new QTableWidgetItem(dts(it.value())));
}

void QBaseGrid::actionRefresh()
{
    actionBeforeSelect();
}

bool QBaseGrid::actionFilter()
{
   bool r = false;
   QDlgFilter *d = new QDlgFilter(m_fields, this);
   if (d->exec() == QDialog::Accepted) {
        for (QList<SqlField*>::iterator it = m_fields.begin(); it != m_fields.end(); it++) {
            if ((*it)->m_filter) {
                (*it)->m_filter->listNames = (*it)->m_filter->tempNames;
                (*it)->m_filter->listValues = (*it)->m_filter->tempValues;
            }
        }
        r = true;
        actionBeforeSelect();
   }
   delete d;
   return r;
}

void QBaseGrid::actionExcel(QString fileName)
{
    if (fileName.isEmpty()) {
        fileName = QFileDialog::getSaveFileName(this, "", "", "*.xlsx");
        if (fileName.isEmpty()) {
            return;
        }
    }
    exportToExcel(fileName);
}

void QBaseGrid::actionMultiSelect(QToolBar *tb)
{
    MdiWindow::actionMultiSelect(tb);
    ui->grid->selectionModel()->clear();
    if (m_multiSelect)
        ui->grid->setSelectionMode(QAbstractItemView::MultiSelection);
    else
        ui->grid->setSelectionMode(QAbstractItemView::SingleSelection);
}

void QBaseGrid::actionPrintPreview()
{
    SizeMetrics sm;
    XmlPrintMaker xpm(&sm);
    getPrintData(xpm);
    PrintPreview *pp = new PrintPreview(this);
    pp->m_lastPrinter = m_lastPrinter;
    pp->setPaintData(xpm, 0);
    pp->show();
}

void QBaseGrid::actionPrint()
{
    QPrinter prn;
    QPrinter::Margins m;
    m.left = 5;
    m.right = 5;
    m.top = 5;
    m.bottom = 5;
    if (m_lastPrinter.length())
        prn.setPrinterName(m_lastPrinter);
    prn.setPageSize(QPrinter::A4);
    prn.setOrientation(QPrinter::Portrait);
    prn.setMargins(m);
    QPrintDialog pd(&prn);
    if (m_lastPrinter.length())
        pd.printer()->setPrinterName(m_lastPrinter);
    if (pd.exec() != QDialog::Accepted)
        return;

    m_lastPrinter = prn.printerName();
    SizeMetrics sm(prn.resolution());
    XmlPrintMaker xpm(&sm);
    getPrintData(xpm);

    if (xpm.pageWidth() > XmlPrintMaker::m_pageWidthProtrate)
        prn.setOrientation(QPrinter::Landscape);

    QPainter painter(&prn);
    XmlPrinter xPrn(&painter, &sm);

    for (int i = 0, count = xpm.pageCount(); i < count; i++) {
        if (i)
            prn.newPage();
        xPrn.render(xpm.page(i));
    }
}

void QBaseGrid::search()
{
    if (!m_findRec.what.length())
        return;


    ui->grid->selectionModel()->clear();
    bool found = false;
    for (int r = m_findRec.lastRow; r < ui->grid->rowCount(); r++) {
        int firstCol = m_findRec.lastCol;
        if (r > m_findRec.lastRow)
            firstCol = 0;
        for (int c = firstCol; c < ui->grid->columnCount(); c++) {
            QTableWidgetItem *item = ui->grid->item(r, c);
            if (!item) {
                continue;
            }
            QString cellText = item->data(Qt::DisplayRole).toString();
            if (cellText.contains(m_findRec.what, Qt::CaseInsensitive)) {
                m_findRec.lastCol = c + 1;
                m_findRec.lastRow = r;
                if (m_findRec.lastCol > ui->grid->columnCount() - 1) {
                    m_findRec.lastCol = 0;
                    m_findRec.lastRow = r + 1;
                }
                ui->grid->setCurrentCell(r, c);
                found = true;
                break;
            }
        }
        if (found)
            break;
    }

    if (m_multiSelect)
        ui->grid->setSelectionMode(QAbstractItemView::MultiSelection);
    else
        ui->grid->setSelectionMode(QAbstractItemView::SingleSelection);

    if (!found)
        QMessageBox::warning(this, tr("Search"), m_findRec.what + "\r\n" + tr("Not found"));
}

QModelIndexList QBaseGrid::selectedRows()
{
    return ui->grid->selectionModel()->selectedRows();
}

QVariant QBaseGrid::itemData(int row, int col, Qt::ItemDataRole role)
{
    QTableWidgetItem *item = ui->grid->item(row, col);
    if (item)
        return item->data(role);
    return QVariant();
}

void QBaseGrid::setItemData(int row, int col, QVariant data, Qt::ItemDataRole role)
{
    QTableWidgetItem *item = ui->grid->item(row, col);
    if (!item)
        return;
    item->setData(role, data);
}

int QBaseGrid::appendRow()
{
    int row = ui->grid->rowCount();
    ui->grid->setRowCount(row + 1);
    for (int i = 0, count = ui->grid->columnCount(); i < count; i++)
        ui->grid->setItem(row, i, new QTableWidgetItem());
    return row;
}

void QBaseGrid::removeRow(int row)
{
    ui->grid->removeRow(row);
    ui->gridTotal->setVerticalHeaderLabels(QStringList() << QString("%1").arg(ui->grid->rowCount()));
}

bool QBaseGrid::eventFilter(QObject *o, QEvent *e)
{

    if (o == ui->grid) {
        switch (e->type()) {
        case QEvent::KeyPress: {
            QKeyEvent *ke = static_cast<QKeyEvent*>(e);
            if (ke->key() == Qt::Key_Control)
                ui->grid->setSelectionMode(QAbstractItemView::MultiSelection);
            break;
        }
        case QEvent::KeyRelease : {
            QKeyEvent *ke = static_cast<QKeyEvent*>(e);
            if (ke->key() == Qt::Key_Control)
                ui->grid->setSelectionMode(QAbstractItemView::SingleSelection);
            break;
        }
        default:
            break;
        }
    }
    return QWidget::eventFilter(o, e);
}

void QBaseGrid::remove_zap(QString &src)
{
    if (src.right(1) == ",")
        src.remove(src.length() - 1, 1);
    if (src == " group by ")
        src = "";
    if (src == " from ")
        src = "";
}

void QBaseGrid::remove_and(QString &src)
{
    if (src.right(3) == "and")
        src.remove(src.length() - 4, 4);
    if (src == " where ")
        src = "";
}

int QBaseGrid::groupFieldsCount()
{
    int result = 0;
    for (QList<SqlField*>::const_iterator it = m_fields.begin(); it != m_fields.end(); it++) {
        if ((*it)->isIncluded())
            result++;
    }
    return result;
}

void QBaseGrid::setCaptions()
{
    CustomHeader *h = static_cast<CustomHeader*>(ui->grid->horizontalHeader());
    h->m_captions.clear();
    for (int i = 0; i < m_fields.count(); i++) {
        if (m_fields.at(i)->isIncluded()) {
            h->m_captions << m_fields.at(i)->title;
            ui->grid->setHorizontalHeaderItem(h->m_captions.count() - 1, new QTableWidgetItem());
            int col = h->columnIndex(m_fields.at(i)->title);
            ui->grid->setColumnWidth(col, m_fields.at(i)->width);
            ui->gridTotal->setColumnWidth(col, m_fields.at(i)->width);
        }
    }
}

void QBaseGrid::setCaptions(const QStringList &captions, const QList<int> widths)
{
    CustomHeader *h = static_cast<CustomHeader*>(ui->grid->horizontalHeader());
    h->m_captions = captions;
    ui->grid->setColumnCount(captions.count());
    ui->gridTotal->setColumnCount(captions.count());
    for (int i = 0; i < widths.count(); i++) {
        ui->grid->setHorizontalHeaderItem(i, new QTableWidgetItem());
        ui->grid->setColumnWidth(i, widths.at(i));
        ui->gridTotal->setColumnWidth(i, widths.at(i));
    }
}

void QBaseGrid::setColCaption(int col, const QString &caption)
{
    CustomHeader *h = static_cast<CustomHeader*>(ui->grid->horizontalHeader());
    h->m_captions[col] = caption;
}

QTableWidget *QBaseGrid::tableWidget(int which)
{
    switch (which) {
    case 0:
        return ui->grid;
    case 1:
        return ui->gridTotal;
    }

    return ui->grid;
}

int QBaseGrid::columnIndex(const QString &name)
{
    CustomHeader *h = static_cast<CustomHeader *>(ui->grid->horizontalHeader());
    return h->columnIndex(name);
}

int QBaseGrid::columnWidth(int columnIndex)
{
    return ui->grid->columnWidth(columnIndex);
}

int QBaseGrid::columnsCount()
{
    return ui->grid->columnCount();
}

void QBaseGrid::setColumnWidth(int column, int width)
{
    ui->grid->setColumnWidth(column, width);
    ui->gridTotal->setColumnWidth(column, width);
}

void QBaseGrid::setHeaderHeight(int height)
{
    QTableWidgetItem *item = ui->grid->horizontalHeaderItem(0);
    if (item)
        item->setSizeHint(QSize(ui->grid->horizontalHeader()->defaultSectionSize(), height));
}

int QBaseGrid::selectedRows(QSet<int> &rows)
{
    return QBaseSqlWindow::selectedRows(ui->grid, rows);
}

QVariant QBaseGrid::cellValue(int row, int col)
{
    QTableWidgetItem *i = ui->grid->item(row, col);
    if (i)
        return i->data(Qt::DisplayRole);
    else
        return QVariant();
}

void QBaseGrid::setCellValue(int row, int col, const QVariant &value)
{
    ui->grid->item(row, col)->setData(Qt::DisplayRole, value);
}

void QBaseGrid::setItemDelegate(QItemDelegate *d)
{
    ui->grid->setItemDelegate(d);
}

void QBaseGrid::createSimpleIdNameTable(const QString &tableName)
{
    SqlField *fId = new SqlField("ID", tr("Id"), 100, 0, QVariant::Int, 0, false);
    m_fields.append(fId);

    SqlField *fName = new SqlField("NAME", tr("Name"), 400, 0, QVariant::String, 0, false);
    m_fields.append(fName);

    m_sqlString = "select id, name from " + tableName + " order by name";
    actionBeforeSelect();
}

bool QBaseGrid::confirmDeletion()
{
    return QMessageBox::warning(this, tr("Warning"), tr("Confirm deletion"), QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes;
}

SqlField *QBaseGrid::addField(const QString &fieldName, const QString &fieldTitle, int width, QVariant::Type type)
{
    SqlField *f = new SqlField(fieldName, fieldTitle, width, 0, type, 0, false);
    m_fields.append(f);
    return f;
}

void QBaseGrid::getPrintData(XmlPrintMaker &x)
{
    x.setFontName(qApp->font().family());
    x.setFontSize(9);

    int top = 5, left = 0, rowHeight = 6;
    double colScale = 0.25;
    x.text(windowTitle() + " / " + QSystem::objectName(), 0, top);
    top += x.lastTextHeight() + 3;

    int gridWidth = 0;
    for (int i = 0, count = ui->grid->columnCount(); i < count; i++)
        gridWidth += ui->grid->columnWidth(i);
    gridWidth *= colScale;
    if (gridWidth > XmlPrintMaker::m_pageWidthProtrate)
        x.setPageHeight(XmlPrintMaker::m_PageHeightLandscape - 20);
    x.line(0, top, gridWidth, top);
    x.line(0, top, 0, top + rowHeight);
    x.setFontBold(true);
    CustomHeader *h = static_cast<CustomHeader*>(ui->grid->horizontalHeader());
    for (int i = 0, count = h->m_captions.count(); i < count; i++) {
        if (!ui->grid->columnWidth(i))
            continue;
        x.text(h->caption(i), left + 1, top);
        left += ui->grid->columnWidth(i) * colScale;
        x.line(left, top, left, top + rowHeight);
    }
    x.line(0, top, gridWidth, top);

    x.setFontBold(false);
    for (int i = 0, rowCount = ui->grid->rowCount(); i < rowCount; i++) {
        x.line(0, top, gridWidth, top);
        x.line(0, top, 0, top + rowHeight);
        top += rowHeight;
        left = 0;
        for (int j = 0, colCount = ui->grid->columnCount(); j < colCount; j++) {
            if (!ui->grid->columnWidth(j))
                continue;
            x.text(ui->grid->item(i, j)->data(Qt::DisplayRole).toString(), left + 1, top);
            left += ui->grid->columnWidth(j) * colScale;
            x.line(left, top, left, top + rowHeight);
        }
        x.checkForNewPage(top);
    }
    for (int i = 0, rowCount = ui->gridTotal->rowCount(); i < rowCount; i++) {
        x.line(0, top, gridWidth, top);
        x.line(0, top, 0, top + rowHeight);
        top += rowHeight;
        left = 0;
        for (int j = 0, colCount = ui->gridTotal->columnCount(); j < colCount; j++) {
            if (!ui->gridTotal->columnWidth(j))
                continue;
            if (ui->gridTotal->item(i, j))
                x.text(ui->gridTotal->item(i, j)->data(Qt::DisplayRole).toString(), left + 1, top);
            left += ui->gridTotal->columnWidth(j) * colScale;
            x.line(left, top, left, top + rowHeight);
        }
        x.checkForNewPage(top);
    }

    x.line(0, top, gridWidth, top);
    x.line(0, top + rowHeight, gridWidth, top + rowHeight);

    x.setFontSize(8);
    x.setFontItalic(true);
    top += 10;
    x.text(QString("%1: %2, %3, %4").arg(tr("Printed")).arg(QDateTime::currentDateTime().toString(DATETIME_FORMAT))
           .arg(getHostUsername()).arg(___ff_user->fullName), 0, top);
    x.finishPage();
}

void QBaseGrid::exportToExcel(const QString &fileName)
{

    int fXlsxFitToPage = 0;
    QString fXlsxPageOrientation = xls_page_orientation_portrait;
    int fXlsxPageSize = xls_page_size_a4;
    int colCount = ui->grid->columnCount();
    int rowCount = ui->grid->rowCount();
    if (colCount == 0 || rowCount == 0) {
        QMessageBox::information(this, "Error", tr("Empty report!"));
        return;
    }
    XlsxDocument d;
    XlsxSheet *s = d.workbook()->addSheet("Sheet1");
    s->setupPage(fXlsxPageSize, fXlsxFitToPage, fXlsxPageOrientation);
    /* HEADER */
    QColor color = QColor::fromRgb(200, 200, 250);
    QFont headerFont(qApp->font());
    headerFont.setBold(true);
    d.style()->addFont("header", headerFont);
    d.style()->addBackgrounFill("header", color);
    d.style()->addHAlignment("header", xls_alignment_center);
    d.style()->addBorder("header", XlsxBorder());
    CustomHeader *h = static_cast<CustomHeader*>(ui->grid->horizontalHeader());
    for (int i = 0; i < colCount; i++) {
        s->addCell(1, i + 1, h->m_captions[i], d.style()->styleNum("header"));
        s->setColumnWidth(i + 1, ui->grid->columnWidth(i) / 7);
    }
    /* BODY */
    QMap<int, QString> bgFill;
    QMap<int, QString> bgFillb;
    QFont bodyFont(qApp->font());
    d.style()->addFont("body", bodyFont);
    d.style()->addBackgrounFill("body", QColor(Qt::white));
    d.style()->addVAlignment("body", xls_alignment_center);
    d.style()->addBorder("body", XlsxBorder());
    bgFill[QColor(Qt::white).rgb()] = "body";
    bodyFont.setBold(true);
    d.style()->addFont("body_b", bodyFont);
    d.style()->addBackgrounFill("body_b", QColor(Qt::white));
    d.style()->addVAlignment("body_b", xls_alignment_center);
    d.style()->addBorder("body_b", XlsxBorder());
    bgFillb[QColor(Qt::white).rgb()] = "body_b";
    for (int j = 0; j < rowCount; j++) {
        for (int i = 0; i < colCount; i++) {
            int bgColor = QColor(Qt::white).rgb();
            if (!bgFill.contains(bgColor)) {
                bodyFont.setBold(false);
                d.style()->addFont(QString::number(bgColor), bodyFont);
                d.style()->addBackgrounFill(QString::number(bgColor), QColor::fromRgb(bgColor));
                bgFill[bgColor] = QString::number(bgColor);
            }
            if (!bgFill.contains(bgColor)) {
                bodyFont.setBold(true);
                d.style()->addFont(QString::number(bgColor), bodyFont);
                d.style()->addBackgrounFill(QString::number(bgColor), QColor::fromRgb(bgColor));
                bgFillb[bgColor] = QString::number(bgColor);
            }
            QString bgStyle = bgFill[bgColor];
            if (ui->grid->item(j, i)->data(Qt::FontRole).value<QFont>().bold()) {
                bgStyle = bgFillb[bgColor];
            }
            s->addCell(j + 2, i + 1, ui->grid->item(j, i)->data(Qt::EditRole), d.style()->styleNum(bgStyle));
        }
    }
    // /* MERGE cells */
    // QMap<int, QList<int> > skiprow, skipcol;
    // for (int r = 0; r < rowCount; r++) {
    //     for (int c = 0; c < colCount; c++) {
    //         if (ui->mTableView->columnSpan(r, c) > 1 || ui->mTableView->rowSpan(r, c) > 1) {
    //             int rs = -1, cs = -1;
    //             if (ui->mTableView->columnSpan(r, c) > 1 && skipcol[r].contains(c) == false) {
    //                 cs = ui->mTableView->columnSpan(r, c) - 1;
    //                 for (int i = c + 1; i < c + cs + 1; i++) {
    //                     skipcol[r].append(i);
    //                 }
    //             }
    //             if (ui->mTableView->rowSpan(r, c) > 1 && skiprow[c].contains(r) == false) {
    //                 rs = ui->mTableView->rowSpan(r, c) - 1;
    //                 for (int i = r + 1; i < r + rs + 1; i++) {
    //                     skiprow[c].append(i);
    //                 }
    //             }
    //             if (rs == -1 && cs == -1) {
    //                 continue;
    //             }
    //             rs = rs < 0 ? 0 : rs;
    //             cs = cs < 0 ? 0 : cs;
    //             s->setSpan(r + 2, c + 1, r + 2 + rs, c + 1 + cs);
    //         }
    //     }
    // }
    /* TOTALS ROWS */
    //    if (ui->tblTotal->isVisible()) {
    //        QFont totalFont(qApp->font());
    //        totalFont.setBold(true);
    //        d.style()->addFont("footer", headerFont);
    //        d.style()->addBorder("footer", XlsxBorder());
    //        color = QColor::fromRgb(193, 206, 221);
    //        d.style()->addBackgrounFill("footer", color);
    //        //d.style()->addHAlignment("footer", xls_alignment_right);
    //        for (int i = 0; i < colCount; i++) {
    //            s->addCell(1 + fModel->rowCount() + 1, i + 1, ui->tblTotal->getData(0, i), d.style()->styleNum("footer"));
    //        }
    //    }
    QString err;
    if (!d.save(fileName, err)) {
        if (!err.isEmpty()) {
            QMessageBox::critical(this, "Error", err);
        }
    }

    // QLibrary lib(QDir::currentPath() + "/excel.dll");
    // if (!lib.load())
    // {
    //     QMessageBox::critical(this, "Program error", "Could not load library " + QDir::currentPath() + "/excel.dll");
    //     return;
    // }

    // Excel_Create excel_create = (Excel_Create)(lib.resolve("Excel_Create"));
    // Excel_Show excel_show = (Excel_Show)(lib.resolve("Excel_Show"));
    // Workbook_Add workbook_add = (Workbook_Add)(lib.resolve("Workbook_Add"));
    // Sheet_Select sheet_select = (Sheet_Select)(lib.resolve("Sheet_Select"));
    // Sheet_Set_Cell_Value sheet_set_cell_value = (Sheet_Set_Cell_Value)(lib.resolve("Sheet_Set_Cell_Value"));
    // Sheet_Set_Col_Width sheet_set_col_width = (Sheet_Set_Col_Width)(lib.resolve("Sheet_Set_Col_Width"));
    // Clear clear = (Clear)(lib.resolve("Clear"));
    // SaveToFile save = (SaveToFile)(lib.resolve("SaveToFile"));

    // int *excel = excel_create();
    // int *workbook = workbook_add(excel);
    // int *sheet = sheet_select(workbook, 1);

    // for (int i = 0; i < ui->grid->columnCount(); i++)
    // {
    //     CustomHeader *h = static_cast<CustomHeader *>( ui->grid->horizontalHeader());
    //     sheet_set_cell_value(sheet, i + 1, 1, h->caption(i).toStdWString().c_str());
    //     sheet_set_col_width(sheet, i + 1, ui->grid->columnWidth(i) / 7);
    // }

    // for (int i = 0; i < ui->grid->rowCount(); i++)
    //     for (int j =  0; j < ui->grid->columnCount(); j++)
    //         sheet_set_cell_value(sheet, j + 1, i + 2, ui->grid->item(i, j)->text().toStdWString().c_str());

    // int footerRow = ui->gridTotal->rowCount() + ui->grid->rowCount() + 2;
    // for (int i = 0; i < ui->grid->rowCount(); i++)
    //     for (int j =  0; j < ui->gridTotal->columnCount(); j++) {
    //         QTableWidgetItem *item = ui->gridTotal->item(i, j);
    //         if (item)
    //             sheet_set_cell_value(sheet, j + 1, i + footerRow, item->text().toStdWString().c_str());
    //     }

    // excel_show(excel);
    // if (fileName.length())
    //     save(workbook, fileName.toStdWString().c_str());
    // clear(sheet);
    // clear(workbook);
    // clear(excel);
}

void QBaseGrid::slotGridHeaderClicked(int index)
{
    ui->grid->sortByColumn(index);
    m_findRec.lastCol = 0;
    m_findRec.lastRow = 0;
}

void QBaseGrid::slotGridSectionResized(int index, int oldSize, int newSize)
{
    Q_UNUSED(oldSize)
    ui->gridTotal->setColumnWidth(index, newSize);
}

void QBaseGrid::slotScrollBarValueChanged(int newValue)
{
    ui->grid->horizontalScrollBar()->setValue(newValue);
}

void QBaseGrid::slotSelectAll()
{
    QAbstractButton *btn = ui->grid->findChild<QAbstractButton*>();
    btn->disconnect();
    connect(btn, SIGNAL(clicked()), SLOT(slotDeselectAll()));
    ui->grid->selectAll();
}

void QBaseGrid::slotDeselectAll()
{
    QAbstractButton *btn = ui->grid->findChild<QAbstractButton*>();
    btn->disconnect();
    connect(btn, SIGNAL(clicked()), SLOT(slotSelectAll()));
    ui->grid->clearSelection();
    ui->grid->setCurrentCell(-1, -1);
}


QBaseGrid::CustomHeader::CustomHeader(Qt::Orientation orientation, QWidget *parent) :
    QHeaderView(orientation, parent)
{
    setVisible(true);
    setSectionsClickable(true);
}

QString QBaseGrid::CustomHeader::caption(const int &index) const
{
    if (index > m_captions.count() - 1)
        return "";
    return m_captions.at(index);
}

int QBaseGrid::CustomHeader::columnIndex(const QString &name)
{
    return m_captions.indexOf(name);
}

void QBaseGrid::CustomHeader::paintSection(QPainter *painter, const QRect &rect, int logicalIndex) const
{
    if (!m_captions.count())
        return;
    if (!rect.width())
        return;
    QPen pen(Qt::white);
    painter->setPen(pen);
    QRect trect = rect;
    trect.adjust(0, 0, -1, 0);
    painter->drawRect(trect);
    QTextOption to;
    to.setAlignment(Qt::AlignCenter);
    to.setWrapMode(QTextOption::WrapAtWordBoundaryOrAnywhere);
    trect = rect;
    trect.adjust(2, 2, -2, 2);
    painter->drawText(trect, caption(logicalIndex), to);
    trect.adjust(-1, -1, -1, -1);
    pen.setColor(Qt::black);
    painter->setPen(pen);
    painter->drawText(trect, caption(logicalIndex), to);
}

void QBaseGrid::on_grid_doubleClicked(const QModelIndex &index)
{
    Q_UNUSED(index)
    actionEdit();
}


void QBaseGrid::ItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if (!index.isValid())
        return;

    painter->save();

    QPen pen(Qt::SolidLine);
    pen.setColor(Qt::black);

    QBrush brush(Qt::white);
    brush.setStyle(Qt::SolidPattern);
    if (option.state & QStyle::State_Selected) {
        brush.setColor(Qt::blue);
        pen.setColor(Qt::white);
    }

    QPen emptyPen(Qt::NoPen);
    painter->setPen(emptyPen);
    painter->setBrush(brush);
    painter->drawRect(option.rect);
    painter->setPen(pen);

    QString text;
    switch (index.data(Qt::DisplayRole).type()) {
    case QVariant::Double:
        text = QString::number(index.data(Qt::DisplayRole).toFloat(), 'f', 2);
        //QLocale::system().toString(index.data(Qt::DisplayRole).toDouble(), 'f');
        break;
    case QVariant::Int:
        text = QString::number(index.data(Qt::DisplayRole).toInt());
        break;
    case QVariant::Date:
        text = index.data(Qt::DisplayRole).toDate().toString(DATE_FORMAT);
        break;
    case QVariant::DateTime:
        text = index.data(Qt::DisplayRole).toDateTime().toString(DATETIME_FORMAT);
        break;
    case QVariant::Time:
        text = index.data(Qt::DisplayRole).toTime().toString(TIME_FORMAT);
        break;
    default:
        text = index.data(Qt::DisplayRole).toString();
        break;
    }

    QRect rText = option.rect;
    rText.adjust(3, 2, -2, -2);
    painter->drawText(rText, text);

    painter->restore();
}
