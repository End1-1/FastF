#include "qbasesqlwindow.h"

QToolBar *QBaseSqlWindow::m_toolBarBtn;
QToolBar *QBaseSqlWindow::m_toolBarWnd;
QString QBaseSqlWindow::m_username;

QBaseSqlWindow::QBaseSqlWindow(QWidget *parent) :
    MdiWindow(parent)
{
    m_sqlDrv = new QSqlDrv(m_username, "clientdb");
    setToolbar(m_toolBarBtn);
}

QBaseSqlWindow::~QBaseSqlWindow()
{
    delete m_sqlDrv;
}

void QBaseSqlWindow::init(QToolBar *btn, QToolBar *wnd)
{
    m_toolBarBtn = btn;
    m_toolBarWnd = wnd;
}

void QBaseSqlWindow::setUsername(const QString &username)
{
    m_username = username;
}

void QBaseSqlWindow::setWidgetVisible(QWidget *w, bool visible)
{
    w->setVisible(visible);
}

int QBaseSqlWindow::selectedRows(QTableWidget *t, QSet<int> &rows)
{
    QModelIndexList l = t->selectionModel()->selectedIndexes();
    for (QModelIndexList::const_iterator it = l.begin(); it != l.end(); it++)
        rows.insert(it->row());
    return rows.count();
}
