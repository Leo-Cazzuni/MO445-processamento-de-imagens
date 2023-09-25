/********************************************************************************
** Form generated from reading UI file 'manualsegmentation.ui'
**
** Created by: Qt User Interface Compiler version 6.3.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MANUALSEGMENTATION_H
#define UI_MANUALSEGMENTATION_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_ManualSegmentation
{
public:
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout;
    QPushButton *pbReset;
    QPushButton *pbDone;

    void setupUi(QWidget *ManualSegmentation)
    {
        if (ManualSegmentation->objectName().isEmpty())
            ManualSegmentation->setObjectName(QString::fromUtf8("ManualSegmentation"));
        ManualSegmentation->resize(400, 300);
        verticalLayout = new QVBoxLayout(ManualSegmentation);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        pbReset = new QPushButton(ManualSegmentation);
        pbReset->setObjectName(QString::fromUtf8("pbReset"));

        horizontalLayout->addWidget(pbReset);

        pbDone = new QPushButton(ManualSegmentation);
        pbDone->setObjectName(QString::fromUtf8("pbDone"));

        horizontalLayout->addWidget(pbDone);


        verticalLayout->addLayout(horizontalLayout);


        retranslateUi(ManualSegmentation);

        QMetaObject::connectSlotsByName(ManualSegmentation);
    } // setupUi

    void retranslateUi(QWidget *ManualSegmentation)
    {
        ManualSegmentation->setWindowTitle(QCoreApplication::translate("ManualSegmentation", "Form", nullptr));
        pbReset->setText(QCoreApplication::translate("ManualSegmentation", "Reset", nullptr));
        pbDone->setText(QCoreApplication::translate("ManualSegmentation", "Done", nullptr));
    } // retranslateUi

};

namespace Ui {
    class ManualSegmentation: public Ui_ManualSegmentation {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MANUALSEGMENTATION_H
