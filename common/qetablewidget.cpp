#include "qetablewidget.h"
#include <QScrollBar>
#include <QHeaderView>

QETableWidget::QETableWidget(QWidget *parent) :
    QTableWidget(parent)
{
}

void QETableWidget::setButtons(QPushButton *btnUp, QPushButton *btnDown)
{
    connect(btnUp, SIGNAL(clicked()), this, SLOT(pageUp()));
    connect(btnDown, SIGNAL(clicked()), this, SLOT(pageDown()));
}

void QETableWidget::pageUp()
{
    scrollPage(-1);
}

void QETableWidget::pageDown()
{
    scrollPage(1);
}

void QETableWidget::scrollPage(int direction)
{
    int rows = (width() / verticalHeader()->defaultSectionSize()) - 2;
    if (rows < 1) {
        rows = 1;
    }
    verticalScrollBar()->setValue(verticalScrollBar()->value() + (rows * direction));
}
