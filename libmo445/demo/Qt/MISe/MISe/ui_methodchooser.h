/********************************************************************************
** Form generated from reading UI file 'methodchooser.ui'
**
** Created by: Qt User Interface Compiler version 6.3.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_METHODCHOOSER_H
#define UI_METHODCHOOSER_H

#include <QtCore/QVariant>
#include <QtWidgets/QAbstractButton>
#include <QtWidgets/QApplication>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_MethodChooser
{
public:
    QVBoxLayout *verticalLayout;
    QComboBox *cbMethods;
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *MethodChooser)
    {
        if (MethodChooser->objectName().isEmpty())
            MethodChooser->setObjectName(QString::fromUtf8("MethodChooser"));
        MethodChooser->resize(312, 74);
        verticalLayout = new QVBoxLayout(MethodChooser);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        cbMethods = new QComboBox(MethodChooser);
        cbMethods->setObjectName(QString::fromUtf8("cbMethods"));

        verticalLayout->addWidget(cbMethods);

        buttonBox = new QDialogButtonBox(MethodChooser);
        buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

        verticalLayout->addWidget(buttonBox);


        retranslateUi(MethodChooser);
        QObject::connect(buttonBox, &QDialogButtonBox::accepted, MethodChooser, qOverload<>(&QDialog::accept));
        QObject::connect(buttonBox, &QDialogButtonBox::rejected, MethodChooser, qOverload<>(&QDialog::reject));

        QMetaObject::connectSlotsByName(MethodChooser);
    } // setupUi

    void retranslateUi(QDialog *MethodChooser)
    {
        MethodChooser->setWindowTitle(QCoreApplication::translate("MethodChooser", "Dialog", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MethodChooser: public Ui_MethodChooser {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_METHODCHOOSER_H
