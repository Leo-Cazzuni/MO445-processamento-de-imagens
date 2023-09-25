/********************************************************************************
** Form generated from reading UI file 'controlled.ui'
**
** Created by: Qt User Interface Compiler version 6.3.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CONTROLLED_H
#define UI_CONTROLLED_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_Controlled
{
public:
    QVBoxLayout *verticalLayout;
    QFormLayout *formLayout;
    QLabel *label;
    QSpinBox *sbThreshold;
    QCheckBox *cbPropagate;
    QHBoxLayout *horizontalLayout;
    QPushButton *pbReset;
    QPushButton *pbRun;

    void setupUi(QWidget *Controlled)
    {
        if (Controlled->objectName().isEmpty())
            Controlled->setObjectName(QString::fromUtf8("Controlled"));
        Controlled->resize(230, 92);
        verticalLayout = new QVBoxLayout(Controlled);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        formLayout = new QFormLayout();
        formLayout->setObjectName(QString::fromUtf8("formLayout"));
        label = new QLabel(Controlled);
        label->setObjectName(QString::fromUtf8("label"));

        formLayout->setWidget(0, QFormLayout::LabelRole, label);

        sbThreshold = new QSpinBox(Controlled);
        sbThreshold->setObjectName(QString::fromUtf8("sbThreshold"));
        sbThreshold->setMaximum(999999);
        sbThreshold->setValue(5);

        formLayout->setWidget(0, QFormLayout::FieldRole, sbThreshold);


        verticalLayout->addLayout(formLayout);

        cbPropagate = new QCheckBox(Controlled);
        cbPropagate->setObjectName(QString::fromUtf8("cbPropagate"));

        verticalLayout->addWidget(cbPropagate);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        pbReset = new QPushButton(Controlled);
        pbReset->setObjectName(QString::fromUtf8("pbReset"));

        horizontalLayout->addWidget(pbReset);

        pbRun = new QPushButton(Controlled);
        pbRun->setObjectName(QString::fromUtf8("pbRun"));

        horizontalLayout->addWidget(pbRun);


        verticalLayout->addLayout(horizontalLayout);


        retranslateUi(Controlled);

        QMetaObject::connectSlotsByName(Controlled);
    } // setupUi

    void retranslateUi(QWidget *Controlled)
    {
        Controlled->setWindowTitle(QCoreApplication::translate("Controlled", "Form", nullptr));
        label->setText(QCoreApplication::translate("Controlled", "Threshold", nullptr));
        cbPropagate->setText(QCoreApplication::translate("Controlled", "Propagate through time", nullptr));
        pbReset->setText(QCoreApplication::translate("Controlled", "Reset", nullptr));
        pbRun->setText(QCoreApplication::translate("Controlled", "Run", nullptr));
    } // retranslateUi

};

namespace Ui {
    class Controlled: public Ui_Controlled {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CONTROLLED_H
