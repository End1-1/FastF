/********************************************************************************
** Form generated from reading UI file 'dlgcalcchange.ui'
**
** Created by: Qt User Interface Compiler version 5.10.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_DLGCALCCHANGE_H
#define UI_DLGCALCCHANGE_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDialog>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_DlgCalcChange
{
public:
    QVBoxLayout *verticalLayout;
    QWidget *widget;
    QHBoxLayout *horizontalLayout_2;
    QGridLayout *gridLayout;
    QLineEdit *leMoney;
    QPushButton *btn1;
    QPushButton *btn2;
    QPushButton *btn4;
    QPushButton *pushButton_17;
    QLineEdit *leAmount;
    QLineEdit *leChange;
    QPushButton *pushButton_18;
    QPushButton *pushButton_19;
    QPushButton *pushButton_23;
    QLabel *label;
    QLabel *label_3;
    QLabel *label_2;
    QPushButton *pushButton_9;
    QPushButton *pushButton_10;
    QPushButton *pushButton_12;
    QPushButton *pushButton_15;
    QPushButton *pushButton_14;
    QPushButton *pushButton_16;
    QPushButton *pushButton_11;
    QPushButton *pushButton_13;
    QSpacerItem *verticalSpacer;
    QPushButton *btn3;
    QPushButton *btn6;

    void setupUi(QDialog *DlgCalcChange)
    {
        if (DlgCalcChange->objectName().isEmpty())
            DlgCalcChange->setObjectName(QLatin1String("DlgCalcChange"));
        DlgCalcChange->resize(955, 738);
        verticalLayout = new QVBoxLayout(DlgCalcChange);
        verticalLayout->setObjectName(QLatin1String("verticalLayout"));
        widget = new QWidget(DlgCalcChange);
        widget->setObjectName(QLatin1String("widget"));
        horizontalLayout_2 = new QHBoxLayout(widget);
        horizontalLayout_2->setObjectName(QLatin1String("horizontalLayout_2"));
        gridLayout = new QGridLayout();
        gridLayout->setObjectName(QLatin1String("gridLayout"));
        leMoney = new QLineEdit(widget);
        leMoney->setObjectName(QLatin1String("leMoney"));
        QFont font;
        font.setPointSize(20);
        leMoney->setFont(font);
        leMoney->setReadOnly(true);

        gridLayout->addWidget(leMoney, 2, 2, 1, 2);

        btn1 = new QPushButton(widget);
        btn1->setObjectName(QLatin1String("btn1"));
        btn1->setMinimumSize(QSize(0, 50));
        btn1->setFont(font);

        gridLayout->addWidget(btn1, 3, 0, 1, 1);

        btn2 = new QPushButton(widget);
        btn2->setObjectName(QLatin1String("btn2"));
        btn2->setMinimumSize(QSize(0, 50));
        btn2->setFont(font);

        gridLayout->addWidget(btn2, 3, 1, 1, 1);

        btn4 = new QPushButton(widget);
        btn4->setObjectName(QLatin1String("btn4"));
        btn4->setMinimumSize(QSize(0, 50));
        btn4->setFont(font);

        gridLayout->addWidget(btn4, 3, 3, 1, 1);

        pushButton_17 = new QPushButton(widget);
        pushButton_17->setObjectName(QLatin1String("pushButton_17"));
        pushButton_17->setMinimumSize(QSize(0, 50));
        pushButton_17->setFont(font);

        gridLayout->addWidget(pushButton_17, 5, 1, 1, 1);

        leAmount = new QLineEdit(widget);
        leAmount->setObjectName(QLatin1String("leAmount"));
        leAmount->setFont(font);
        leAmount->setReadOnly(true);

        gridLayout->addWidget(leAmount, 2, 0, 1, 2);

        leChange = new QLineEdit(widget);
        leChange->setObjectName(QLatin1String("leChange"));
        leChange->setFont(font);
        leChange->setReadOnly(true);

        gridLayout->addWidget(leChange, 2, 5, 1, 2);

        pushButton_18 = new QPushButton(widget);
        pushButton_18->setObjectName(QLatin1String("pushButton_18"));
        pushButton_18->setMinimumSize(QSize(0, 50));
        pushButton_18->setFont(font);

        gridLayout->addWidget(pushButton_18, 5, 2, 1, 1);

        pushButton_19 = new QPushButton(widget);
        pushButton_19->setObjectName(QLatin1String("pushButton_19"));
        pushButton_19->setMinimumSize(QSize(0, 50));
        pushButton_19->setFont(font);

        gridLayout->addWidget(pushButton_19, 4, 1, 1, 1);

        pushButton_23 = new QPushButton(widget);
        pushButton_23->setObjectName(QLatin1String("pushButton_23"));
        pushButton_23->setMinimumSize(QSize(0, 50));
        pushButton_23->setFont(font);

        gridLayout->addWidget(pushButton_23, 4, 2, 1, 1);

        label = new QLabel(widget);
        label->setObjectName(QLatin1String("label"));
        label->setFont(font);

        gridLayout->addWidget(label, 0, 0, 1, 2);

        label_3 = new QLabel(widget);
        label_3->setObjectName(QLatin1String("label_3"));
        label_3->setFont(font);

        gridLayout->addWidget(label_3, 0, 2, 1, 2);

        label_2 = new QLabel(widget);
        label_2->setObjectName(QLatin1String("label_2"));
        label_2->setFont(font);

        gridLayout->addWidget(label_2, 0, 5, 1, 2);

        pushButton_9 = new QPushButton(widget);
        pushButton_9->setObjectName(QLatin1String("pushButton_9"));
        pushButton_9->setMinimumSize(QSize(0, 50));
        pushButton_9->setFont(font);

        gridLayout->addWidget(pushButton_9, 4, 0, 1, 1);

        pushButton_10 = new QPushButton(widget);
        pushButton_10->setObjectName(QLatin1String("pushButton_10"));
        pushButton_10->setMinimumSize(QSize(0, 50));
        pushButton_10->setFont(font);

        gridLayout->addWidget(pushButton_10, 4, 3, 1, 1);

        pushButton_12 = new QPushButton(widget);
        pushButton_12->setObjectName(QLatin1String("pushButton_12"));
        pushButton_12->setMinimumSize(QSize(0, 50));
        QIcon icon;
        icon.addFile(QLatin1String(":/res/cancel.png"), QSize(), QIcon::Normal, QIcon::Off);
        pushButton_12->setIcon(icon);
        pushButton_12->setIconSize(QSize(45, 45));

        gridLayout->addWidget(pushButton_12, 4, 6, 1, 1);

        pushButton_15 = new QPushButton(widget);
        pushButton_15->setObjectName(QLatin1String("pushButton_15"));
        pushButton_15->setMinimumSize(QSize(0, 50));
        pushButton_15->setFont(font);

        gridLayout->addWidget(pushButton_15, 5, 5, 1, 1);

        pushButton_14 = new QPushButton(widget);
        pushButton_14->setObjectName(QLatin1String("pushButton_14"));
        pushButton_14->setMinimumSize(QSize(0, 50));
        pushButton_14->setFont(font);

        gridLayout->addWidget(pushButton_14, 5, 3, 1, 1);

        pushButton_16 = new QPushButton(widget);
        pushButton_16->setObjectName(QLatin1String("pushButton_16"));
        pushButton_16->setMinimumSize(QSize(0, 50));
        QIcon icon1;
        icon1.addFile(QLatin1String(":/res/exit.png"), QSize(), QIcon::Normal, QIcon::Off);
        pushButton_16->setIcon(icon1);
        pushButton_16->setIconSize(QSize(45, 45));

        gridLayout->addWidget(pushButton_16, 5, 6, 1, 1);

        pushButton_11 = new QPushButton(widget);
        pushButton_11->setObjectName(QLatin1String("pushButton_11"));
        pushButton_11->setMinimumSize(QSize(0, 50));
        pushButton_11->setFont(font);

        gridLayout->addWidget(pushButton_11, 4, 5, 1, 1);

        pushButton_13 = new QPushButton(widget);
        pushButton_13->setObjectName(QLatin1String("pushButton_13"));
        pushButton_13->setMinimumSize(QSize(0, 50));
        pushButton_13->setFont(font);

        gridLayout->addWidget(pushButton_13, 5, 0, 1, 1);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        gridLayout->addItem(verticalSpacer, 6, 0, 1, 1);

        btn3 = new QPushButton(widget);
        btn3->setObjectName(QLatin1String("btn3"));
        btn3->setMinimumSize(QSize(0, 50));
        btn3->setFont(font);

        gridLayout->addWidget(btn3, 3, 2, 1, 1);

        btn6 = new QPushButton(widget);
        btn6->setObjectName(QLatin1String("btn6"));
        btn6->setMinimumSize(QSize(0, 50));
        btn6->setFont(font);

        gridLayout->addWidget(btn6, 3, 5, 1, 2);


        horizontalLayout_2->addLayout(gridLayout);


        verticalLayout->addWidget(widget);


        retranslateUi(DlgCalcChange);

        QMetaObject::connectSlotsByName(DlgCalcChange);
    } // setupUi

    void retranslateUi(QDialog *DlgCalcChange)
    {
        DlgCalcChange->setWindowTitle(QApplication::translate("DlgCalcChange", "Dialog", nullptr));
        leMoney->setText(QApplication::translate("DlgCalcChange", "0", nullptr));
        btn1->setText(QApplication::translate("DlgCalcChange", "5000", nullptr));
        btn2->setText(QApplication::translate("DlgCalcChange", "10000", nullptr));
        btn4->setText(QApplication::translate("DlgCalcChange", "20000", nullptr));
        pushButton_17->setText(QApplication::translate("DlgCalcChange", "7", nullptr));
        leAmount->setText(QApplication::translate("DlgCalcChange", "0", nullptr));
        leChange->setText(QApplication::translate("DlgCalcChange", "0", nullptr));
        pushButton_18->setText(QApplication::translate("DlgCalcChange", "8", nullptr));
        pushButton_19->setText(QApplication::translate("DlgCalcChange", "2", nullptr));
        pushButton_23->setText(QApplication::translate("DlgCalcChange", "3", nullptr));
        label->setText(QApplication::translate("DlgCalcChange", "Amount", nullptr));
        label_3->setText(QApplication::translate("DlgCalcChange", "Money", nullptr));
        label_2->setText(QApplication::translate("DlgCalcChange", "Change", nullptr));
        pushButton_9->setText(QApplication::translate("DlgCalcChange", "1", nullptr));
        pushButton_10->setText(QApplication::translate("DlgCalcChange", "4", nullptr));
        pushButton_12->setText(QString());
        pushButton_15->setText(QApplication::translate("DlgCalcChange", "0", nullptr));
        pushButton_14->setText(QApplication::translate("DlgCalcChange", "9", nullptr));
        pushButton_16->setText(QString());
        pushButton_11->setText(QApplication::translate("DlgCalcChange", "5", nullptr));
        pushButton_13->setText(QApplication::translate("DlgCalcChange", "6", nullptr));
        btn3->setText(QApplication::translate("DlgCalcChange", "15000", nullptr));
        btn6->setText(QApplication::translate("DlgCalcChange", "Random", nullptr));
    } // retranslateUi

};

namespace Ui {
    class DlgCalcChange: public Ui_DlgCalcChange {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DLGCALCCHANGE_H
