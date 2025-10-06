#ifndef QBASESQLDIALOG_H
#define QBASESQLDIALOG_H

#include "mdiwindow.h"
#include <QMdiArea>
#include <QMdiSubWindow>
#include "qsqldrv.h"

class QBaseSqlWindow : public MdiWindow
{
    Q_OBJECT
public:
    explicit QBaseSqlWindow(QWidget *parent = 0);
    ~QBaseSqlWindow();
    static void init(QToolBar *btn, QToolBar *wnd);
    static void setUsername(const QString &username);
    void setWidgetVisible(QWidget *w, bool visible = false);

public:
    static QToolBar *m_toolBarBtn;
    static QToolBar *m_toolBarWnd;
    static QString m_username;
    QSqlDrv *m_sqlDrv;
    int selectedRows(QTableWidget *t, QSet<int> &rows);

};

#endif // QBASESQLDIALOG_H
