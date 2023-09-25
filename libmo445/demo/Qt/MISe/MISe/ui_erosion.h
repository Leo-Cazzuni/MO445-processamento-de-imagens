/********************************************************************************
** Form generated from reading UI file 'erosion.ui'
**
** Created by: Qt User Interface Compiler version 6.3.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_EROSION_H
#define UI_EROSION_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_Erosion
{
public:
    QVBoxLayout *verticalLayout;
    QLabel *label_2;
    QFormLayout *formLayout_3;
    QDoubleSpinBox *sbRadius;
    QLabel *label_3;

    void setupUi(QWidget *Erosion)
    {
        if (Erosion->objectName().isEmpty())
            Erosion->setObjectName(QString::fromUtf8("Erosion"));
        Erosion->resize(240, 51);
        verticalLayout = new QVBoxLayout(Erosion);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        label_2 = new QLabel(Erosion);
        label_2->setObjectName(QString::fromUtf8("label_2"));

        verticalLayout->addWidget(label_2);

        formLayout_3 = new QFormLayout();
        formLayout_3->setObjectName(QString::fromUtf8("formLayout_3"));
        sbRadius = new QDoubleSpinBox(Erosion);
        sbRadius->setObjectName(QString::fromUtf8("sbRadius"));
        sbRadius->setMaximum(1000000.000000000000000);

        formLayout_3->setWidget(0, QFormLayout::FieldRole, sbRadius);

        label_3 = new QLabel(Erosion);
        label_3->setObjectName(QString::fromUtf8("label_3"));

        formLayout_3->setWidget(0, QFormLayout::LabelRole, label_3);


        verticalLayout->addLayout(formLayout_3);


        retranslateUi(Erosion);

        QMetaObject::connectSlotsByName(Erosion);
    } // setupUi

    void retranslateUi(QWidget *Erosion)
    {
        Erosion->setWindowTitle(QCoreApplication::translate("Erosion", "Form", nullptr));
        label_2->setText(QCoreApplication::translate("Erosion", "Erosion", nullptr));
        label_3->setText(QCoreApplication::translate("Erosion", "Radius for erosion", nullptr));
    } // retranslateUi

};

namespace Ui {
    class Erosion: public Ui_Erosion {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_EROSION_H
