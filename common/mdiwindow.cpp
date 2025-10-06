#include "mdiwindow.h"
#include <QMessageBox>
#include <QPushButton>
#include <QInputDialog>

MdiWindow::MdiWindow(QWidget *parent) :
    QWidget(parent)
{
    m_multiSelect = false;
    m_findRec.lastCol = 0;
    m_findRec.lastRow = 0;
    m_findRec.what = "";
    m_settings = new QSettings("Jazzve", "Cafe4");
}

void MdiWindow::actionNew()
{

}

void MdiWindow::actionEdit()
{

}

void MdiWindow::actionDelete()
{

}

void MdiWindow::actionSave()
{

}

void MdiWindow::actionBeforeSelect()
{

}

void MdiWindow::actionSelect()
{

}

void MdiWindow::actionRefresh()
{

}

bool MdiWindow::actionFilter()
{
    return true;
}

void MdiWindow::actionPrintPreview()
{

}

void MdiWindow::actionPrint()
{

}

void MdiWindow::actionExcel(QString fileName)
{
    Q_UNUSED(fileName)
}

void MdiWindow::actionSearch()
{
    QString txt = QInputDialog::getText(this, tr("Search"), tr("Text"), QLineEdit::Normal, m_findRec.what);
    txt = txt.trimmed();
    if (!txt.length())
        return;
    m_findRec.what = txt;
    m_findRec.lastCol = 0;
    m_findRec.lastRow = 0;
    search();
}

void MdiWindow::actionSearchNext()
{
    if (m_findRec.what.isEmpty()) {
        return;
    }
    search();
}

void MdiWindow::actionMultiSelect(QToolBar *tb)
{
    m_multiSelect = !m_multiSelect;
    QList<QAction*> la = tb->actions();
    for (QList<QAction*>::const_iterator it = la.begin(); it != la.end(); it++) {
        if ((*it)->objectName() == "actionMultiselection") {
            if ((*it)->isVisible()) {
                if (m_multiSelect)
                    (*it)->setIcon(QIcon(":/img/checked.png"));
                else
                    (*it)->setIcon(QIcon(":/img/unchecked.png"));
            }
        }
    }
}

void MdiWindow::actionCostum(int action)
{
    Q_UNUSED(action)
}

void MdiWindow::activateToolbarButtons(const QList<QAction *> &actions)
{
    for (QList<QAction*>::const_iterator it = actions.begin(); it != actions.end(); it++)
        (*it)->setVisible(false);
    m_toolBarActions = actions;
}

void MdiWindow::setToolbarButtons(QToolBar *tb)
{
    QList<QAction*> la = tb->actions();
    for (QList<QAction*>::const_iterator it = la.begin(); it != la.end(); it++)
        (*it)->setVisible(false);
    setToolbarButtonVisible(tb);
}

void MdiWindow::setToolbarButtonVisible(QToolBar *tb)
{
    QList<QAction*> la = tb->actions();
    for (QList<QAction*>::const_iterator it = la.begin(); it != la.end(); it++) {
        if (m_actions.contains((*it)->objectName())) {
                (*it)->setVisible(true);
            if ((*it)->objectName() == "actionMultiselection") {
                if ((*it)->isVisible()) {
                    if (m_multiSelect)
                        (*it)->setIcon(QIcon(":/img/checked.png"));
                    else
                        (*it)->setIcon(QIcon(":/img/unchecked.png"));
                }
            }
        }
    }
}

void MdiWindow::search()
{

}

bool MdiWindow::getYesNo(const QString &text)
{
    QMessageBox mb(this);
    mb.setText(text);
    mb.addButton(new QPushButton(tr("Yes")), QMessageBox::AcceptRole);
    mb.addButton(new QPushButton(tr("No")), QMessageBox::NoRole);
    return mb.exec() == QMessageBox::AcceptRole;
}

void MdiWindow::setActionVisible(const QString &action, bool visible)
{
    QList<QAction*> la = m_toolBar->actions();
    for (QList<QAction*>::const_iterator it = la.begin(); it != la.end(); it++)
        if (action == (*it)->objectName())
            (*it)->setVisible(visible);
}

void MdiWindow::setToolbar(QToolBar *tb)
{
    m_toolBar = tb;
}
