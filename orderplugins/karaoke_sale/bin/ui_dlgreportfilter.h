/********************************************************************************
** Form generated from reading UI file 'dlgreportfilter.ui'
**
** Created by: Qt User Interface Compiler version 5.10.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_DLGREPORTFILTER_H
#define UI_DLGREPORTFILTER_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDateEdit>
#include <QtWidgets/QDialog>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_DlgReportFilter
{
public:
    QHBoxLayout *horizontalLayout;
    QGridLayout *gridLayout;
    QLabel *label_2;
    QDateEdit *date1;
    QDateEdit *date2;
    QWidget *widget;
    QHBoxLayout *horizontalLayout_2;
    QSpacerItem *horizontalSpacer;
    QPushButton *pushButton;
    QPushButton *pushButton_2;
    QSpacerItem *horizontalSpacer_2;

    void setupUi(QDialog *DlgReportFilter)
    {
        if (DlgReportFilter->objectName().isEmpty())
            DlgReportFilter->setObjectName(QLatin1String("DlgReportFilter"));
        DlgReportFilter->resize(610, 108);
        horizontalLayout = new QHBoxLayout(DlgReportFilter);
        horizontalLayout->setObjectName(QLatin1String("horizontalLayout"));
        gridLayout = new QGridLayout();
        gridLayout->setObjectName(QLatin1String("gridLayout"));
        label_2 = new QLabel(DlgReportFilter);
        label_2->setObjectName(QLatin1String("label_2"));

        gridLayout->addWidget(label_2, 0, 0, 1, 1);

        date1 = new QDateEdit(DlgReportFilter);
        date1->setObjectName(QLatin1String("date1"));
        date1->setCalendarPopup(true);

        gridLayout->addWidget(date1, 0, 1, 1, 1);

        date2 = new QDateEdit(DlgReportFilter);
        date2->setObjectName(QLatin1String("date2"));
        date2->setCalendarPopup(true);

        gridLayout->addWidget(date2, 0, 2, 1, 1);

        widget = new QWidget(DlgReportFilter);
        widget->setObjectName(QLatin1String("widget"));
        horizontalLayout_2 = new QHBoxLayout(widget);
        horizontalLayout_2->setObjectName(QLatin1String("horizontalLayout_2"));
        horizontalSpacer = new QSpacerItem(197, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer);

        pushButton = new QPushButton(widget);
        pushButton->setObjectName(QLatin1String("pushButton"));
        pushButton->setMinimumSize(QSize(0, 0));
        QIcon icon;
        icon.addFile(QLatin1String(":/res/ok.png"), QSize(), QIcon::Normal, QIcon::Off);
        pushButton->setIcon(icon);
        pushButton->setIconSize(QSize(36, 36));

        horizontalLayout_2->addWidget(pushButton);

        pushButton_2 = new QPushButton(widget);
        pushButton_2->setObjectName(QLatin1String("pushButton_2"));
        pushButton_2->setMinimumSize(QSize(0, 0));
        QIcon icon1;
        icon1.addFile(QLatin1String(":/res/cancel.png"), QSize(), QIcon::Normal, QIcon::Off);
        pushButton_2->setIcon(icon1);
        pushButton_2->setIconSize(QSize(36, 36));

        horizontalLayout_2->addWidget(pushButton_2);

        horizontalSpacer_2 = new QSpacerItem(196, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer_2);


        gridLayout->addWidget(widget, 1, 0, 1, 3);


        horizontalLayout->addLayout(gridLayout);


        retranslateUi(DlgReportFilter);

        QMetaObject::connectSlotsByName(DlgReportFilter);
    } // setupUi

    void retranslateUi(QDialog *DlgReportFilter)
    {
        DlgReportFilter->setWindowTitle(QApplication::translate("DlgReportFilter", "Dialog", nullptr));
        label_2->setText(QApplication::translate("DlgReportFilter", "Date range", nullptr));
        pushButton->setText(QApplication::translate("DlgReportFilter", "OK", nullptr));
        pushButton_2->setText(QApplication::translate("DlgReportFilter", "Cancel", nullptr));
    } // retranslateUi

};

namespace Ui {
    class DlgReportFilter: public Ui_DlgReportFilter {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DLGREPORTFILTER_H
