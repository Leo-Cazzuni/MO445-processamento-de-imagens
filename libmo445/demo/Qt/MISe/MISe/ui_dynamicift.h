/********************************************************************************
** Form generated from reading UI file 'dynamicift.ui'
**
** Created by: Qt User Interface Compiler version 6.3.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_DYNAMICIFT_H
#define UI_DYNAMICIFT_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_DynamicIFT
{
public:
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout;
    QPushButton *pbReset;
    QPushButton *pbRun;

    void setupUi(QWidget *DynamicIFT)
    {
        if (DynamicIFT->objectName().isEmpty())
            DynamicIFT->setObjectName(QString::fromUtf8("DynamicIFT"));
        DynamicIFT->resize(350, 169);
        verticalLayout = new QVBoxLayout(DynamicIFT);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        pbReset = new QPushButton(DynamicIFT);
        pbReset->setObjectName(QString::fromUtf8("pbReset"));

        horizontalLayout->addWidget(pbReset);

        pbRun = new QPushButton(DynamicIFT);
        pbRun->setObjectName(QString::fromUtf8("pbRun"));

        horizontalLayout->addWidget(pbRun);


        verticalLayout->addLayout(horizontalLayout);


        retranslateUi(DynamicIFT);

        QMetaObject::connectSlotsByName(DynamicIFT);
    } // setupUi

    void retranslateUi(QWidget *DynamicIFT)
    {
        DynamicIFT->setWindowTitle(QCoreApplication::translate("DynamicIFT", "Form", nullptr));
        pbReset->setText(QCoreApplication::translate("DynamicIFT", "Reset", nullptr));
        pbRun->setText(QCoreApplication::translate("DynamicIFT", "Run", nullptr));
    } // retranslateUi

};

namespace Ui {
    class DynamicIFT: public Ui_DynamicIFT {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DYNAMICIFT_H
