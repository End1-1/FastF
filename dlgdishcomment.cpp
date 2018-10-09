#include "dlgdishcomment.h"
#include "ui_dlgdishcomment.h"
#include <QScrollBar>

#define U(X) QString::fromUtf8(X)

DlgDishComment::DlgDishComment(QWidget *parent) :
    QDialog(parent, Qt::FramelessWindowHint),
    ui(new Ui::DlgDishComment)
{
    /*
    if (!comments.count()) {
        DlgMessageBox::show(tr("No comments"), 1, this);
        return;
    }
    */

    m_sqlDrv = new QSqlDrv("FASTF", "main");

    ui->setupUi(this);
    showFullScreen();
    qApp->processEvents();

    QFont font(qApp->font());
    font.setBold(true);
    font.setPointSize(12);
    ui->tblLetter->setFont(font);

    QStringList row1, row2, row3, row4, row5;
    row1 << U("1") << U("2") << U("3") << U("4") << U("5") << U("6") << U("7") << U("8") << U("9") << U("0") << U("=") << U(".") << U(" ");
    row2 << U("Է") << U("Թ") << U("Փ") << U("Ձ") << U("Ջ") << U("Ւ") << U("և") << U("Ր") << U("Չ") << U("Ճ") << U("-") << U("Ժ") << U(" ");
    row3 << U("Ք") << U("Ո") << U("Ե") << U("Ռ") << U("Տ") << U("Տ") << U("Ը") << U("Ւ") << U("Ի") << U("Օ") << U("Պ") << U("Խ") << U("Ծ");
    row4 << U("Ա") << U("Ս") << U("Դ") << U("Ֆ") << U("Գ") << U("Հ") << U("Յ") << U("Կ") << U("Լ") << U(":") << U("Շ") << U(" ") << U(" ");
    row5 << U("Զ") << U("Ղ") << U("Ց") << U("Վ") << U("Բ") << U("Ն") << U("Մ") << U("") << U("") << U("?") << U(" ") << U(" ") << U(" ");
    m_letters.insert(0, row1);
    m_letters.insert(1, row2);
    m_letters.insert(2, row3);
    m_letters.insert(3, row4);
    m_letters.insert(4, row5);

    QStringList comments;
    m_sqlDrv->prepare("select name from me_dishes_comment order by name");
    m_sqlDrv->execSQL();
    while (m_sqlDrv->m_query->next())
        comments.append(m_sqlDrv->valStr("NAME"));
    m_sqlDrv->close();

    int colWidth = ui->tableWidget->horizontalHeader()->defaultSectionSize();
    int colCount = (ui->tableWidget->width() - 5) / colWidth;
    int rowCount = (comments.count() / colCount);
    if (!rowCount)
        rowCount++;
    if (comments.count() / colCount)
        rowCount++;

    ui->tableWidget->setColumnCount(colCount);
    ui->tableWidget->setRowCount(rowCount);

    int col = 0, row = 0;
    for (QStringList::const_iterator it = comments.begin(); it != comments.end(); it++) {
        ui->tableWidget->setItem(row, col++, new QTableWidgetItem(*it));
        if (col >= colCount) {
            row++;
            col = 0;
        }
    }

    for (QMap<int,QStringList>::const_iterator it = m_letters.begin(); it != m_letters.end(); it++) {
        col = 0;
        ui->tblLetter->setRowCount(it.key() + 1);
        ui->tblLetter->setColumnCount(it.value().count() > ui->tblLetter->columnCount() ? it.value().count() : ui->tblLetter->columnCount());
        for (QStringList::const_iterator jt = it.value().begin(); jt != it.value().end(); jt++)
            ui->tblLetter->setItem(it.key(), col++, new QTableWidgetItem(*jt));
        row++;
    }
    ui->tblLetter->setMinimumHeight((ui->tblLetter->rowCount() * ui->tblLetter->verticalHeader()->defaultSectionSize()) + 4);
    ui->tblLetter->setItemDelegate(new QLetterDelegate());
}

DlgDishComment::~DlgDishComment()
{
    delete ui;
    delete m_sqlDrv;
}

QString DlgDishComment::result()
{
    return ui->leComment->text();
}

void DlgDishComment::setSingleSelection()
{
    ui->tableWidget->setSelectionMode(QAbstractItemView::SingleSelection);
}

void DlgDishComment::on_pushButton_2_clicked()
{
    reject();
}

void DlgDishComment::on_pushButton_clicked()
{
    accept();
}

void DlgDishComment::on_pushButton_3_clicked()
{
    ui->tableWidget->verticalScrollBar()->setValue(ui->tableWidget->verticalScrollBar()->value() + 6);
}

void DlgDishComment::on_pushButton_4_clicked()
{
    ui->tableWidget->verticalScrollBar()->setValue(ui->tableWidget->verticalScrollBar()->value() - 6);
}

void DlgDishComment::on_tableWidget_clicked(const QModelIndex &index)
{
    if (!index.isValid())
        return;
    if (!index.data().toString().length())
        return;

    if (ui->leComment->text().length())
        ui->leComment->setText(ui->leComment->text() + ", ");
    ui->leComment->setText(ui->leComment->text() + index.data().toString());
}

void DlgDishComment::on_pushButton_5_clicked()
{
    ui->leComment->clear();
}


void DlgDishComment::QLetterDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    painter->drawText(option.rect, index.data().toString(), QTextOption(Qt::AlignCenter));
}

void DlgDishComment::on_tblLetter_clicked(const QModelIndex &index)
{
    if (!index.isValid())
        return;

    ui->leComment->setText(ui->leComment->text() + index.data().toString());
}

void DlgDishComment::on_pushButton_6_clicked()
{
    if (!ui->leComment->text().length())
        return;

    ui->leComment->setText(ui->leComment->text().remove(ui->leComment->text().length() - 1, 1));
}
