/********************************************************************************
** Form generated from reading UI file 'loadasmarkers.ui'
**
** Created by: Qt User Interface Compiler version 6.3.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_LOADASMARKERS_H
#define UI_LOADASMARKERS_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_LoadAsMarkers
{
public:
    QVBoxLayout *verticalLayout;
    QLabel *label;
    QCheckBox *cbDeleteLabel;

    void setupUi(QWidget *LoadAsMarkers)
    {
        if (LoadAsMarkers->objectName().isEmpty())
            LoadAsMarkers->setObjectName(QString::fromUtf8("LoadAsMarkers"));
        LoadAsMarkers->resize(268, 46);
        verticalLayout = new QVBoxLayout(LoadAsMarkers);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        label = new QLabel(LoadAsMarkers);
        label->setObjectName(QString::fromUtf8("label"));

        verticalLayout->addWidget(label);

        cbDeleteLabel = new QCheckBox(LoadAsMarkers);
        cbDeleteLabel->setObjectName(QString::fromUtf8("cbDeleteLabel"));
        cbDeleteLabel->setChecked(true);

        verticalLayout->addWidget(cbDeleteLabel);


        retranslateUi(LoadAsMarkers);

        QMetaObject::connectSlotsByName(LoadAsMarkers);
    } // setupUi

    void retranslateUi(QWidget *LoadAsMarkers)
    {
        LoadAsMarkers->setWindowTitle(QCoreApplication::translate("LoadAsMarkers", "Form", nullptr));
        label->setText(QCoreApplication::translate("LoadAsMarkers", "Load as markers", nullptr));
        cbDeleteLabel->setText(QCoreApplication::translate("LoadAsMarkers", "Delete label", nullptr));
    } // retranslateUi

};

namespace Ui {
    class LoadAsMarkers: public Ui_LoadAsMarkers {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_LOADASMARKERS_H
