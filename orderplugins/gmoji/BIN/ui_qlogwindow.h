/********************************************************************************
** Form generated from reading UI file 'qlogwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.12.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_QLOGWINDOW_H
#define UI_QLOGWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QHBoxLayout>
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
            QLogWindow->setObjectName(QString::fromUtf8("QLogWindow"));
        QLogWindow->resize(1015, 830);
        centralwidget = new QWidget(QLogWindow);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        horizontalLayout = new QHBoxLayout(centralwidget);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        txtLogWindow = new QTextEdit(centralwidget);
        txtLogWindow->setObjectName(QString::fromUtf8("txtLogWindow"));

        horizontalLayout->addWidget(txtLogWindow);

        QLogWindow->setCentralWidget(centralwidget);

        retranslateUi(QLogWindow);

        QMetaObject::connectSlotsByName(QLogWindow);
    } // setupUi

    void retranslateUi(QMainWindow *QLogWindow)
    {
        QLogWindow->setWindowTitle(QApplication::translate("QLogWindow", "Log window", nullptr));
    } // retranslateUi

};

namespace Ui {
    class QLogWindow: public Ui_QLogWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_QLOGWINDOW_H
