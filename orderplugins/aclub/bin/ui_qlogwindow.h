/********************************************************************************
** Form generated from reading UI file 'qlogwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.6.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_QLOGWINDOW_H
#define UI_QLOGWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_QLogWindow
{
public:
    QWidget *centralwidget;
    QHBoxLayout *horizontalLayout;
    QTextEdit *txtLogWindow;

    void setupUi(QMainWindow *QLogWindow)
    {
        if (QLogWindow->objectName().isEmpty())
            QLogWindow->setObjectName(QStringLiteral("QLogWindow"));
        QLogWindow->resize(1015, 830);
        centralwidget = new QWidget(QLogWindow);
        centralwidget->setObjectName(QStringLiteral("centralwidget"));
        horizontalLayout = new QHBoxLayout(centralwidget);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        txtLogWindow = new QTextEdit(centralwidget);
        txtLogWindow->setObjectName(QStringLiteral("txtLogWindow"));

        horizontalLayout->addWidget(txtLogWindow);

        QLogWindow->setCentralWidget(centralwidget);

        retranslateUi(QLogWindow);

        QMetaObject::connectSlotsByName(QLogWindow);
    } // setupUi

    void retranslateUi(QMainWindow *QLogWindow)
    {
        QLogWindow->setWindowTitle(QApplication::translate("QLogWindow", "Log window", 0));
    } // retranslateUi

};

namespace Ui {
    class QLogWindow: public Ui_QLogWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_QLOGWINDOW_H
