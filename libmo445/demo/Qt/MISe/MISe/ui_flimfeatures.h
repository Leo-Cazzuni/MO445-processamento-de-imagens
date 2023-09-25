/********************************************************************************
** Form generated from reading UI file 'flimfeatures.ui'
**
** Created by: Qt User Interface Compiler version 6.3.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_FLIMFEATURES_H
#define UI_FLIMFEATURES_H

#include <QtCore/QVariant>
#include <QtGui/QIcon>
#include <QtWidgets/QApplication>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QToolButton>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_FLIMFeatures
{
public:
    QVBoxLayout *verticalLayout_2;
    QHBoxLayout *horizontalLayout;
    QComboBox *cbFLIM;
    QToolButton *pbCreate;
    QToolButton *pbRemove;
    QFormLayout *formLayout;
    QDoubleSpinBox *sbFactor;
    QLabel *label;
    QDoubleSpinBox *sbAdj;
    QLabel *label_2;

    void setupUi(QWidget *FLIMFeatures)
    {
        if (FLIMFeatures->objectName().isEmpty())
            FLIMFeatures->setObjectName(QString::fromUtf8("FLIMFeatures"));
        FLIMFeatures->resize(244, 93);
        verticalLayout_2 = new QVBoxLayout(FLIMFeatures);
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        verticalLayout_2->setContentsMargins(0, 0, 0, 0);
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        cbFLIM = new QComboBox(FLIMFeatures);
        cbFLIM->setObjectName(QString::fromUtf8("cbFLIM"));

        horizontalLayout->addWidget(cbFLIM);

        pbCreate = new QToolButton(FLIMFeatures);
        pbCreate->setObjectName(QString::fromUtf8("pbCreate"));
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/Images/icons/add.svg"), QSize(), QIcon::Normal, QIcon::Off);
        pbCreate->setIcon(icon);

        horizontalLayout->addWidget(pbCreate);

        pbRemove = new QToolButton(FLIMFeatures);
        pbRemove->setObjectName(QString::fromUtf8("pbRemove"));
        QIcon icon1;
        icon1.addFile(QString::fromUtf8(":/Images/icons/remove.svg"), QSize(), QIcon::Normal, QIcon::Off);
        pbRemove->setIcon(icon1);

        horizontalLayout->addWidget(pbRemove);


        verticalLayout_2->addLayout(horizontalLayout);

        formLayout = new QFormLayout();
        formLayout->setObjectName(QString::fromUtf8("formLayout"));
        sbFactor = new QDoubleSpinBox(FLIMFeatures);
        sbFactor->setObjectName(QString::fromUtf8("sbFactor"));
        sbFactor->setMaximum(1.000000000000000);
        sbFactor->setSingleStep(0.100000000000000);
        sbFactor->setValue(0.700000000000000);

        formLayout->setWidget(0, QFormLayout::FieldRole, sbFactor);

        label = new QLabel(FLIMFeatures);
        label->setObjectName(QString::fromUtf8("label"));

        formLayout->setWidget(1, QFormLayout::LabelRole, label);

        sbAdj = new QDoubleSpinBox(FLIMFeatures);
        sbAdj->setObjectName(QString::fromUtf8("sbAdj"));
        sbAdj->setValue(1.000000000000000);

        formLayout->setWidget(1, QFormLayout::FieldRole, sbAdj);

        label_2 = new QLabel(FLIMFeatures);
        label_2->setObjectName(QString::fromUtf8("label_2"));

        formLayout->setWidget(0, QFormLayout::LabelRole, label_2);


        verticalLayout_2->addLayout(formLayout);


        retranslateUi(FLIMFeatures);

        QMetaObject::connectSlotsByName(FLIMFeatures);
    } // setupUi

    void retranslateUi(QWidget *FLIMFeatures)
    {
        FLIMFeatures->setWindowTitle(QCoreApplication::translate("FLIMFeatures", "Form", nullptr));
        pbCreate->setText(QCoreApplication::translate("FLIMFeatures", "...", nullptr));
        pbRemove->setText(QCoreApplication::translate("FLIMFeatures", "...", nullptr));
        label->setText(QCoreApplication::translate("FLIMFeatures", "Adjacency relation radius", nullptr));
        label_2->setText(QCoreApplication::translate("FLIMFeatures", "FLIM activations factor", nullptr));
    } // retranslateUi

};

namespace Ui {
    class FLIMFeatures: public Ui_FLIMFeatures {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_FLIMFEATURES_H
