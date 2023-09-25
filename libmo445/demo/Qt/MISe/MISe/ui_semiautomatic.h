/********************************************************************************
** Form generated from reading UI file 'semiautomatic.ui'
**
** Created by: Qt User Interface Compiler version 6.3.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SEMIAUTOMATIC_H
#define UI_SEMIAUTOMATIC_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_SemiAutomatic
{
public:
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout;
    QPushButton *pbReset;
    QPushButton *pbRun;

    void setupUi(QWidget *SemiAutomatic)
    {
        if (SemiAutomatic->objectName().isEmpty())
            SemiAutomatic->setObjectName(QString::fromUtf8("SemiAutomatic"));
        SemiAutomatic->resize(196, 156);
        verticalLayout = new QVBoxLayout(SemiAutomatic);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        pbReset = new QPushButton(SemiAutomatic);
        pbReset->setObjectName(QString::fromUtf8("pbReset"));

        horizontalLayout->addWidget(pbReset);

        pbRun = new QPushButton(SemiAutomatic);
        pbRun->setObjectName(QString::fromUtf8("pbRun"));

        horizontalLayout->addWidget(pbRun);


        verticalLayout->addLayout(horizontalLayout);


        retranslateUi(SemiAutomatic);

        QMetaObject::connectSlotsByName(SemiAutomatic);
    } // setupUi

    void retranslateUi(QWidget *SemiAutomatic)
    {
        SemiAutomatic->setWindowTitle(QCoreApplication::translate("SemiAutomatic", "Form", nullptr));
        pbReset->setText(QCoreApplication::translate("SemiAutomatic", "Reset", nullptr));
        pbRun->setText(QCoreApplication::translate("SemiAutomatic", "Run", nullptr));
    } // retranslateUi

};

namespace Ui {
    class SemiAutomatic: public Ui_SemiAutomatic {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SEMIAUTOMATIC_H
