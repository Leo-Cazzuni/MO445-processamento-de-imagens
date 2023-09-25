/********************************************************************************
** Form generated from reading UI file 'morphopening.ui'
**
** Created by: Qt User Interface Compiler version 6.3.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MORPHOPENING_H
#define UI_MORPHOPENING_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MorphOpening
{
public:
    QVBoxLayout *verticalLayout;
    QLabel *label_2;
    QFormLayout *formLayout;
    QLabel *label;
    QDoubleSpinBox *sbRadius;

    void setupUi(QWidget *MorphOpening)
    {
        if (MorphOpening->objectName().isEmpty())
            MorphOpening->setObjectName(QString::fromUtf8("MorphOpening"));
        MorphOpening->resize(244, 51);
        verticalLayout = new QVBoxLayout(MorphOpening);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        label_2 = new QLabel(MorphOpening);
        label_2->setObjectName(QString::fromUtf8("label_2"));

        verticalLayout->addWidget(label_2);

        formLayout = new QFormLayout();
        formLayout->setObjectName(QString::fromUtf8("formLayout"));
        label = new QLabel(MorphOpening);
        label->setObjectName(QString::fromUtf8("label"));

        formLayout->setWidget(0, QFormLayout::LabelRole, label);

        sbRadius = new QDoubleSpinBox(MorphOpening);
        sbRadius->setObjectName(QString::fromUtf8("sbRadius"));
        sbRadius->setMaximum(1000000.000000000000000);

        formLayout->setWidget(0, QFormLayout::FieldRole, sbRadius);


        verticalLayout->addLayout(formLayout);


        retranslateUi(MorphOpening);

        QMetaObject::connectSlotsByName(MorphOpening);
    } // setupUi

    void retranslateUi(QWidget *MorphOpening)
    {
        MorphOpening->setWindowTitle(QCoreApplication::translate("MorphOpening", "Form", nullptr));
        label_2->setText(QCoreApplication::translate("MorphOpening", "Morphological Opening", nullptr));
        label->setText(QCoreApplication::translate("MorphOpening", "Radius for opening", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MorphOpening: public Ui_MorphOpening {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MORPHOPENING_H
