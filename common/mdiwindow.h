#ifndef MDIWINDOW_H
#define MDIWINDOW_H

#include <QMainWindow>
#include <QAction>
#include <QToolBar>
#include <QMessageBox>
#include <QSettings>

#define ACTION_NEW 0
#define ACTION_EDIT 1
#define ACTION_SAVE 2
#define ACTION_MULTIPLE_EDIT 3
#define ACTION_DELETE 4
#define ACTION_FIND 5
#define ACTION_REFRESH 6
#define ACTION_FILTER 7
#define ACTION_PRINT_PREVIEW 9
#define ACTION_PRINT 10
#define ACTION_EXTPORT_TO_EXCEL 11
#define ACTION_SEARCH 12

#define ACTION_SALES_SETTINGS 50
#define ACTION_COPY 51
#define ACTION_PASTE 52
#define ACTION_RECIPE 53
#define ACTION_MENU 54
#define ACTION_MAGIC 55
#define ACTION_VIEW_PRICES 56
#define ACTION_SET_PASSWORD 57
#define ACTION_CLEAR_PASSWORD 58
#define ACTION_ACCESS_CONTROL 59
#define ACTION_GROUP_SALARY 60
#define ACTION_UP 61
#define ACTION_DOWN 62
#define ACTION_HISTORY 63
#define ACTION_COSTUM 64
#define ACTION_REP_OPTION 65
#define ACTION_CORRECT_STORE 66
#define ACTION_IMG 67

class MdiWindow : public QWidget
{
    Q_OBJECT

    struct FindRec {
        QString what;
        int lastRow;
        int lastCol;
    };

public:
    explicit MdiWindow(QWidget *parent = 0);
    virtual void actionNew();
    virtual void actionEdit();
    virtual void actionDelete();
    virtual void actionSave();
    virtual void actionBeforeSelect();
    virtual void actionSelect();
    virtual void actionRefresh();
    virtual bool actionFilter();
    virtual void actionPrintPreview();
    virtual void actionPrint();
    virtual void actionExcel(QString fileName = "");
    virtual void actionSearch();
    virtual void actionSearchNext();
    virtual void actionMultiSelect(QToolBar *tb);
    virtual void actionCostum(int action);
    virtual void activateToolbarButtons(const QList<QAction *> &actions);
    virtual void setToolbarButtons(QToolBar *tb);
    virtual void setToolbarButtonVisible(QToolBar *tb);
    virtual void search();
    bool getYesNo(const QString &text);

protected:
    FindRec m_findRec;
    bool m_multiSelect;
    QList<QAction*> m_toolBarActions;
    QStringList m_actions;
    void setActionVisible(const QString &action, bool visible);
    void setToolbar(QToolBar *tb);
    QSettings *m_settings;

private:
    QToolBar *m_toolBar;

};

#endif // MDIWINDOW_H
