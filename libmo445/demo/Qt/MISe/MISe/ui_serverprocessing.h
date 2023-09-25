/********************************************************************************
** Form generated from reading UI file 'serverprocessing.ui'
**
** Created by: Qt User Interface Compiler version 6.3.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SERVERPROCESSING_H
#define UI_SERVERPROCESSING_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_ServerProcessing
{
public:
    QVBoxLayout *verticalLayout;
    QLineEdit *lnServer;
    QHBoxLayout *horizontalLayout;
    QLabel *label;
    QDoubleSpinBox *sbAdjRel;

    void setupUi(QWidget *ServerProcessing)
    {
        if (ServerProcessing->objectName().isEmpty())
            ServerProcessing->setObjectName(QString::fromUtf8("ServerProcessing"));
        ServerProcessing->resize(262, 77);
        verticalLayout = new QVBoxLayout(ServerProcessing);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        lnServer = new QLineEdit(ServerProcessing);
        lnServer->setObjectName(QString::fromUtf8("lnServer"));

        verticalLayout->addWidget(lnServer);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        label = new QLabel(ServerProcessing);
        label->setObjectName(QString::fromUtf8("label"));

        horizontalLayout->addWidget(label);

        sbAdjRel = new QDoubleSpinBox(ServerProcessing);
        sbAdjRel->setObjectName(QString::fromUtf8("sbAdjRel"));
        sbAdjRel->setValue(1.000000000000000);

        horizontalLayout->addWidget(sbAdjRel);


        verticalLayout->addLayout(horizontalLayout);


        retranslateUi(ServerProcessing);

        QMetaObject::connectSlotsByName(ServerProcessing);
    } // setupUi

    void retranslateUi(QWidget *ServerProcessing)
    {
        ServerProcessing->setWindowTitle(QCoreApplication::translate("ServerProcessing", "Form", nullptr));
        lnServer->setText(QCoreApplication::translate("ServerProcessing", "http://athena.lids.ic.unicamp.br:9992", nullptr));
        label->setText(QCoreApplication::translate("ServerProcessing", "Adjacency relation radius", nullptr));
    } // retranslateUi

};

namespace Ui {
    class ServerProcessing: public Ui_ServerProcessing {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SERVERPROCESSING_H
