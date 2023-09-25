/********************************************************************************
** Form generated from reading UI file 'dilation.ui'
**
** Created by: Qt User Interface Compiler version 6.3.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_DILATION_H
#define UI_DILATION_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_Dilation
{
public:
    QVBoxLayout *verticalLayout;
    QLabel *label_2;
    QFormLayout *formLayout_3;
    QLabel *label_3;
    QDoubleSpinBox *sbRadius_3;

    void setupUi(QWidget *Dilation)
    {
        if (Dilation->objectName().isEmpty())
            Dilation->setObjectName(QString::fromUtf8("Dilation"));
        Dilation->resize(244, 51);
        verticalLayout = new QVBoxLayout(Dilation);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        label_2 = new QLabel(Dilation);
        label_2->setObjectName(QString::fromUtf8("label_2"));

        verticalLayout->addWidget(label_2);

        formLayout_3 = new QFormLayout();
        formLayout_3->setObjectName(QString::fromUtf8("formLayout_3"));
        label_3 = new QLabel(Dilation);
        label_3->setObjectName(QString::fromUtf8("label_3"));

        formLayout_3->setWidget(0, QFormLayout::LabelRole, label_3);

        sbRadius_3 = new QDoubleSpinBox(Dilation);
        sbRadius_3->setObjectName(QString::fromUtf8("sbRadius_3"));
        sbRadius_3->setMaximum(1000000.000000000000000);

        formLayout_3->setWidget(0, QFormLayout::FieldRole, sbRadius_3);


        verticalLayout->addLayout(formLayout_3);


        retranslateUi(Dilation);

        QMetaObject::connectSlotsByName(Dilation);
    } // setupUi

    void retranslateUi(QWidget *Dilation)
    {
        Dilation->setWindowTitle(QCoreApplication::translate("Dilation", "Form", nullptr));
        label_2->setText(QCoreApplication::translate("Dilation", "Dilation", nullptr));
        label_3->setText(QCoreApplication::translate("Dilation", "Radius for dilation", nullptr));
    } // retranslateUi

};

namespace Ui {
    class Dilation: public Ui_Dilation {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DILATION_H
