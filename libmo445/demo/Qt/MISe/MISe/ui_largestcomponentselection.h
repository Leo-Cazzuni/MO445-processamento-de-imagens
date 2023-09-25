/********************************************************************************
** Form generated from reading UI file 'largestcomponentselection.ui'
**
** Created by: Qt User Interface Compiler version 6.3.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_LARGESTCOMPONENTSELECTION_H
#define UI_LARGESTCOMPONENTSELECTION_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QLabel>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_LargestComponentSelection
{
public:
    QVBoxLayout *verticalLayout;
    QLabel *label_3;
    QLabel *label;

    void setupUi(QWidget *LargestComponentSelection)
    {
        if (LargestComponentSelection->objectName().isEmpty())
            LargestComponentSelection->setObjectName(QString::fromUtf8("LargestComponentSelection"));
        LargestComponentSelection->resize(348, 40);
        verticalLayout = new QVBoxLayout(LargestComponentSelection);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        label_3 = new QLabel(LargestComponentSelection);
        label_3->setObjectName(QString::fromUtf8("label_3"));

        verticalLayout->addWidget(label_3);

        label = new QLabel(LargestComponentSelection);
        label->setObjectName(QString::fromUtf8("label"));
        QFont font;
        font.setItalic(true);
        label->setFont(font);

        verticalLayout->addWidget(label);


        retranslateUi(LargestComponentSelection);

        QMetaObject::connectSlotsByName(LargestComponentSelection);
    } // setupUi

    void retranslateUi(QWidget *LargestComponentSelection)
    {
        LargestComponentSelection->setWindowTitle(QCoreApplication::translate("LargestComponentSelection", "Form", nullptr));
        label_3->setText(QCoreApplication::translate("LargestComponentSelection", "Select Largest Component", nullptr));
        label->setText(QCoreApplication::translate("LargestComponentSelection", "This method does not require any parameters.", nullptr));
    } // retranslateUi

};

namespace Ui {
    class LargestComponentSelection: public Ui_LargestComponentSelection {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_LARGESTCOMPONENTSELECTION_H
