/********************************************************************************
** Form generated from reading UI file 'brain.ui'
**
** Created by: Qt User Interface Compiler version 6.3.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_BRAIN_H
#define UI_BRAIN_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_Brain
{
public:
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout;

    void setupUi(QWidget *Brain)
    {
        if (Brain->objectName().isEmpty())
            Brain->setObjectName(QString::fromUtf8("Brain"));
        Brain->resize(291, 32);
        verticalLayout = new QVBoxLayout(Brain);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));

        verticalLayout->addLayout(horizontalLayout);


        retranslateUi(Brain);

        QMetaObject::connectSlotsByName(Brain);
    } // setupUi

    void retranslateUi(QWidget *Brain)
    {
        Brain->setWindowTitle(QCoreApplication::translate("Brain", "Form", nullptr));
    } // retranslateUi

};

namespace Ui {
    class Brain: public Ui_Brain {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_BRAIN_H
