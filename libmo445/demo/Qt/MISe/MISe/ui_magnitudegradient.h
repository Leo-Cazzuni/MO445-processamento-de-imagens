/********************************************************************************
** Form generated from reading UI file 'magnitudegradient.ui'
**
** Created by: Qt User Interface Compiler version 6.3.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAGNITUDEGRADIENT_H
#define UI_MAGNITUDEGRADIENT_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MagnitudeGradient
{
public:
    QVBoxLayout *verticalLayout;
    QFormLayout *formLayout;
    QLabel *label;
    QDoubleSpinBox *sbAdjRel;

    void setupUi(QWidget *MagnitudeGradient)
    {
        if (MagnitudeGradient->objectName().isEmpty())
            MagnitudeGradient->setObjectName(QString::fromUtf8("MagnitudeGradient"));
        MagnitudeGradient->resize(327, 61);
        verticalLayout = new QVBoxLayout(MagnitudeGradient);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        formLayout = new QFormLayout();
        formLayout->setObjectName(QString::fromUtf8("formLayout"));
        label = new QLabel(MagnitudeGradient);
        label->setObjectName(QString::fromUtf8("label"));

        formLayout->setWidget(0, QFormLayout::LabelRole, label);

        sbAdjRel = new QDoubleSpinBox(MagnitudeGradient);
        sbAdjRel->setObjectName(QString::fromUtf8("sbAdjRel"));
        sbAdjRel->setValue(1.000000000000000);

        formLayout->setWidget(0, QFormLayout::FieldRole, sbAdjRel);


        verticalLayout->addLayout(formLayout);


        retranslateUi(MagnitudeGradient);

        QMetaObject::connectSlotsByName(MagnitudeGradient);
    } // setupUi

    void retranslateUi(QWidget *MagnitudeGradient)
    {
        MagnitudeGradient->setWindowTitle(QCoreApplication::translate("MagnitudeGradient", "Form", nullptr));
        label->setText(QCoreApplication::translate("MagnitudeGradient", "Adjacency relation radius", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MagnitudeGradient: public Ui_MagnitudeGradient {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAGNITUDEGRADIENT_H
