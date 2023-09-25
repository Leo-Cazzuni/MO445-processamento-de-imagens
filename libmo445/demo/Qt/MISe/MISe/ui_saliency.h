/********************************************************************************
** Form generated from reading UI file 'saliency.ui'
**
** Created by: Qt User Interface Compiler version 6.3.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SALIENCY_H
#define UI_SALIENCY_H

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

class Ui_Saliency
{
public:
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout_4;
    QLineEdit *lePath;
    QToolButton *pbLoad;
    QFormLayout *formLayout;
    QLabel *label_2;
    QDoubleSpinBox *sbSaliencyFactor;
    QLabel *label;
    QDoubleSpinBox *sbAdjRel;

    void setupUi(QWidget *Saliency)
    {
        if (Saliency->objectName().isEmpty())
            Saliency->setObjectName(QString::fromUtf8("Saliency"));
        Saliency->resize(244, 99);
        verticalLayout = new QVBoxLayout(Saliency);
        verticalLayout->setSpacing(6);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        horizontalLayout_4 = new QHBoxLayout();
        horizontalLayout_4->setSpacing(6);
        horizontalLayout_4->setObjectName(QString::fromUtf8("horizontalLayout_4"));
        lePath = new QLineEdit(Saliency);
        lePath->setObjectName(QString::fromUtf8("lePath"));
        lePath->setReadOnly(true);

        horizontalLayout_4->addWidget(lePath);

        pbLoad = new QToolButton(Saliency);
        pbLoad->setObjectName(QString::fromUtf8("pbLoad"));
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/Images/icons/folder_open.svg"), QSize(), QIcon::Normal, QIcon::Off);
        pbLoad->setIcon(icon);

        horizontalLayout_4->addWidget(pbLoad);


        verticalLayout->addLayout(horizontalLayout_4);

        formLayout = new QFormLayout();
        formLayout->setObjectName(QString::fromUtf8("formLayout"));
        label_2 = new QLabel(Saliency);
        label_2->setObjectName(QString::fromUtf8("label_2"));

        formLayout->setWidget(1, QFormLayout::LabelRole, label_2);

        sbSaliencyFactor = new QDoubleSpinBox(Saliency);
        sbSaliencyFactor->setObjectName(QString::fromUtf8("sbSaliencyFactor"));
        sbSaliencyFactor->setMaximum(1.000000000000000);
        sbSaliencyFactor->setSingleStep(0.100000000000000);
        sbSaliencyFactor->setValue(0.300000000000000);

        formLayout->setWidget(1, QFormLayout::FieldRole, sbSaliencyFactor);

        label = new QLabel(Saliency);
        label->setObjectName(QString::fromUtf8("label"));

        formLayout->setWidget(2, QFormLayout::LabelRole, label);

        sbAdjRel = new QDoubleSpinBox(Saliency);
        sbAdjRel->setObjectName(QString::fromUtf8("sbAdjRel"));
        sbAdjRel->setValue(1.000000000000000);

        formLayout->setWidget(2, QFormLayout::FieldRole, sbAdjRel);


        verticalLayout->addLayout(formLayout);


        retranslateUi(Saliency);

        QMetaObject::connectSlotsByName(Saliency);
    } // setupUi

    void retranslateUi(QWidget *Saliency)
    {
        Saliency->setWindowTitle(QCoreApplication::translate("Saliency", "Form", nullptr));
        lePath->setPlaceholderText(QCoreApplication::translate("Saliency", "Salience map path", nullptr));
        pbLoad->setText(QCoreApplication::translate("Saliency", "...", nullptr));
        label_2->setText(QCoreApplication::translate("Saliency", "Salience map factor", nullptr));
        label->setText(QCoreApplication::translate("Saliency", "Adjacency relation radius", nullptr));
    } // retranslateUi

};

namespace Ui {
    class Saliency: public Ui_Saliency {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SALIENCY_H
