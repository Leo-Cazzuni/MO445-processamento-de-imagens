/********************************************************************************
** Form generated from reading UI file 'morphclosing.ui'
**
** Created by: Qt User Interface Compiler version 6.3.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MORPHCLOSING_H
#define UI_MORPHCLOSING_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MorphClosing
{
public:
    QVBoxLayout *verticalLayout;
    QLabel *label_2;
    QFormLayout *formLayout;
    QDoubleSpinBox *sbRadius;
    QLabel *label;

    void setupUi(QWidget *MorphClosing)
    {
        if (MorphClosing->objectName().isEmpty())
            MorphClosing->setObjectName(QString::fromUtf8("MorphClosing"));
        MorphClosing->resize(247, 51);
        verticalLayout = new QVBoxLayout(MorphClosing);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        label_2 = new QLabel(MorphClosing);
        label_2->setObjectName(QString::fromUtf8("label_2"));

        verticalLayout->addWidget(label_2);

        formLayout = new QFormLayout();
        formLayout->setObjectName(QString::fromUtf8("formLayout"));
        sbRadius = new QDoubleSpinBox(MorphClosing);
        sbRadius->setObjectName(QString::fromUtf8("sbRadius"));
        sbRadius->setMaximum(1000000.000000000000000);

        formLayout->setWidget(0, QFormLayout::FieldRole, sbRadius);

        label = new QLabel(MorphClosing);
        label->setObjectName(QString::fromUtf8("label"));

        formLayout->setWidget(0, QFormLayout::LabelRole, label);


        verticalLayout->addLayout(formLayout);


        retranslateUi(MorphClosing);

        QMetaObject::connectSlotsByName(MorphClosing);
    } // setupUi

    void retranslateUi(QWidget *MorphClosing)
    {
        MorphClosing->setWindowTitle(QCoreApplication::translate("MorphClosing", "Form", nullptr));
        label_2->setText(QCoreApplication::translate("MorphClosing", "Morphological Closing", nullptr));
        label->setText(QCoreApplication::translate("MorphClosing", "Radius for closing", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MorphClosing: public Ui_MorphClosing {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MORPHCLOSING_H
