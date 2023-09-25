/********************************************************************************
** Form generated from reading UI file 'manualarcweight.ui'
**
** Created by: Qt User Interface Compiler version 6.3.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MANUALARCWEIGHT_H
#define UI_MANUALARCWEIGHT_H

#include <QtCore/QVariant>
#include <QtGui/QIcon>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QToolButton>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_ManualArcWeight
{
public:
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout;
    QLineEdit *leMimg;
    QToolButton *pbFind;
    QFormLayout *formLayout;
    QDoubleSpinBox *sbAdj;
    QLabel *label;

    void setupUi(QWidget *ManualArcWeight)
    {
        if (ManualArcWeight->objectName().isEmpty())
            ManualArcWeight->setObjectName(QString::fromUtf8("ManualArcWeight"));
        ManualArcWeight->resize(262, 79);
        verticalLayout = new QVBoxLayout(ManualArcWeight);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        leMimg = new QLineEdit(ManualArcWeight);
        leMimg->setObjectName(QString::fromUtf8("leMimg"));
        leMimg->setReadOnly(true);

        horizontalLayout->addWidget(leMimg);

        pbFind = new QToolButton(ManualArcWeight);
        pbFind->setObjectName(QString::fromUtf8("pbFind"));
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/Images/icons/folder_open.svg"), QSize(), QIcon::Normal, QIcon::Off);
        pbFind->setIcon(icon);

        horizontalLayout->addWidget(pbFind);


        verticalLayout->addLayout(horizontalLayout);

        formLayout = new QFormLayout();
        formLayout->setObjectName(QString::fromUtf8("formLayout"));
        sbAdj = new QDoubleSpinBox(ManualArcWeight);
        sbAdj->setObjectName(QString::fromUtf8("sbAdj"));
        sbAdj->setValue(1.000000000000000);

        formLayout->setWidget(0, QFormLayout::FieldRole, sbAdj);

        label = new QLabel(ManualArcWeight);
        label->setObjectName(QString::fromUtf8("label"));

        formLayout->setWidget(0, QFormLayout::LabelRole, label);


        verticalLayout->addLayout(formLayout);


        retranslateUi(ManualArcWeight);

        QMetaObject::connectSlotsByName(ManualArcWeight);
    } // setupUi

    void retranslateUi(QWidget *ManualArcWeight)
    {
        ManualArcWeight->setWindowTitle(QCoreApplication::translate("ManualArcWeight", "Form", nullptr));
        leMimg->setPlaceholderText(QCoreApplication::translate("ManualArcWeight", "Load activations", nullptr));
        pbFind->setText(QCoreApplication::translate("ManualArcWeight", "...", nullptr));
        label->setText(QCoreApplication::translate("ManualArcWeight", "Adjacency relation radius", nullptr));
    } // retranslateUi

};

namespace Ui {
    class ManualArcWeight: public Ui_ManualArcWeight {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MANUALARCWEIGHT_H
