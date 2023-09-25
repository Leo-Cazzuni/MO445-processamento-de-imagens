/********************************************************************************
** Form generated from reading UI file 'altis.ui'
**
** Created by: Qt User Interface Compiler version 6.3.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_ALTIS_H
#define UI_ALTIS_H

#include <QtCore/QLocale>
#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_ALTIS
{
public:
    QVBoxLayout *verticalLayout;
    QPushButton *pbAdvancedOptions;
    QWidget *formAdvancedOptions;
    QFormLayout *formLayout;
    QLabel *lbAdjRel;
    QDoubleSpinBox *sbAdjRel;
    QLabel *lbClosingRadius;
    QDoubleSpinBox *sbClosing;
    QLabel *lbOtsu;
    QLabel *lbTracheaThreshold;
    QDoubleSpinBox *sbTracheaThreshold;
    QLabel *lbDilation;
    QDoubleSpinBox *sbDilation;
    QLabel *lbErosion;
    QDoubleSpinBox *sbErosion;
    QDoubleSpinBox *sbOtsu;
    QLabel *lbRadiusAnomaly;
    QLabel *lbGeodesicAnomaly;
    QDoubleSpinBox *sbRadiusAnomaly;
    QDoubleSpinBox *sbGeodesicAnomaly;
    QCheckBox *cbGaussianFilter;
    QCheckBox *cbRemoveNoise;
    QCheckBox *cbPleuralCorrection;
    QPushButton *pbRun;
    QPushButton *pbCorrect;

    void setupUi(QWidget *ALTIS)
    {
        if (ALTIS->objectName().isEmpty())
            ALTIS->setObjectName(QString::fromUtf8("ALTIS"));
        ALTIS->resize(401, 430);
        QSizePolicy sizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(ALTIS->sizePolicy().hasHeightForWidth());
        ALTIS->setSizePolicy(sizePolicy);
        verticalLayout = new QVBoxLayout(ALTIS);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        pbAdvancedOptions = new QPushButton(ALTIS);
        pbAdvancedOptions->setObjectName(QString::fromUtf8("pbAdvancedOptions"));
        pbAdvancedOptions->setCheckable(true);
        pbAdvancedOptions->setChecked(false);

        verticalLayout->addWidget(pbAdvancedOptions);

        formAdvancedOptions = new QWidget(ALTIS);
        formAdvancedOptions->setObjectName(QString::fromUtf8("formAdvancedOptions"));
        formLayout = new QFormLayout(formAdvancedOptions);
        formLayout->setObjectName(QString::fromUtf8("formLayout"));
        formLayout->setContentsMargins(0, 0, 0, 0);
        lbAdjRel = new QLabel(formAdvancedOptions);
        lbAdjRel->setObjectName(QString::fromUtf8("lbAdjRel"));

        formLayout->setWidget(0, QFormLayout::LabelRole, lbAdjRel);

        sbAdjRel = new QDoubleSpinBox(formAdvancedOptions);
        sbAdjRel->setObjectName(QString::fromUtf8("sbAdjRel"));
        sbAdjRel->setLocale(QLocale(QLocale::English, QLocale::UnitedStates));
        sbAdjRel->setDecimals(3);
        sbAdjRel->setValue(1.000000000000000);

        formLayout->setWidget(0, QFormLayout::FieldRole, sbAdjRel);

        lbClosingRadius = new QLabel(formAdvancedOptions);
        lbClosingRadius->setObjectName(QString::fromUtf8("lbClosingRadius"));

        formLayout->setWidget(1, QFormLayout::LabelRole, lbClosingRadius);

        sbClosing = new QDoubleSpinBox(formAdvancedOptions);
        sbClosing->setObjectName(QString::fromUtf8("sbClosing"));
        sbClosing->setLocale(QLocale(QLocale::English, QLocale::UnitedStates));
        sbClosing->setDecimals(3);
        sbClosing->setValue(2.000000000000000);

        formLayout->setWidget(1, QFormLayout::FieldRole, sbClosing);

        lbOtsu = new QLabel(formAdvancedOptions);
        lbOtsu->setObjectName(QString::fromUtf8("lbOtsu"));

        formLayout->setWidget(2, QFormLayout::LabelRole, lbOtsu);

        lbTracheaThreshold = new QLabel(formAdvancedOptions);
        lbTracheaThreshold->setObjectName(QString::fromUtf8("lbTracheaThreshold"));

        formLayout->setWidget(3, QFormLayout::LabelRole, lbTracheaThreshold);

        sbTracheaThreshold = new QDoubleSpinBox(formAdvancedOptions);
        sbTracheaThreshold->setObjectName(QString::fromUtf8("sbTracheaThreshold"));
        sbTracheaThreshold->setLocale(QLocale(QLocale::English, QLocale::UnitedStates));
        sbTracheaThreshold->setDecimals(3);
        sbTracheaThreshold->setValue(0.500000000000000);

        formLayout->setWidget(3, QFormLayout::FieldRole, sbTracheaThreshold);

        lbDilation = new QLabel(formAdvancedOptions);
        lbDilation->setObjectName(QString::fromUtf8("lbDilation"));

        formLayout->setWidget(4, QFormLayout::LabelRole, lbDilation);

        sbDilation = new QDoubleSpinBox(formAdvancedOptions);
        sbDilation->setObjectName(QString::fromUtf8("sbDilation"));
        sbDilation->setLocale(QLocale(QLocale::English, QLocale::UnitedStates));
        sbDilation->setDecimals(3);
        sbDilation->setValue(9.000000000000000);

        formLayout->setWidget(4, QFormLayout::FieldRole, sbDilation);

        lbErosion = new QLabel(formAdvancedOptions);
        lbErosion->setObjectName(QString::fromUtf8("lbErosion"));

        formLayout->setWidget(5, QFormLayout::LabelRole, lbErosion);

        sbErosion = new QDoubleSpinBox(formAdvancedOptions);
        sbErosion->setObjectName(QString::fromUtf8("sbErosion"));
        sbErosion->setLocale(QLocale(QLocale::English, QLocale::UnitedStates));
        sbErosion->setDecimals(3);
        sbErosion->setMaximum(300.000000000000000);
        sbErosion->setValue(144.000000000000000);

        formLayout->setWidget(5, QFormLayout::FieldRole, sbErosion);

        sbOtsu = new QDoubleSpinBox(formAdvancedOptions);
        sbOtsu->setObjectName(QString::fromUtf8("sbOtsu"));
        sbOtsu->setLocale(QLocale(QLocale::English, QLocale::UnitedStates));
        sbOtsu->setDecimals(3);
        sbOtsu->setValue(1.200000000000000);

        formLayout->setWidget(2, QFormLayout::FieldRole, sbOtsu);

        lbRadiusAnomaly = new QLabel(formAdvancedOptions);
        lbRadiusAnomaly->setObjectName(QString::fromUtf8("lbRadiusAnomaly"));
        lbRadiusAnomaly->setEnabled(false);

        formLayout->setWidget(6, QFormLayout::LabelRole, lbRadiusAnomaly);

        lbGeodesicAnomaly = new QLabel(formAdvancedOptions);
        lbGeodesicAnomaly->setObjectName(QString::fromUtf8("lbGeodesicAnomaly"));
        lbGeodesicAnomaly->setEnabled(false);

        formLayout->setWidget(7, QFormLayout::LabelRole, lbGeodesicAnomaly);

        sbRadiusAnomaly = new QDoubleSpinBox(formAdvancedOptions);
        sbRadiusAnomaly->setObjectName(QString::fromUtf8("sbRadiusAnomaly"));
        sbRadiusAnomaly->setEnabled(false);
        sbRadiusAnomaly->setMinimum(0.000000000000000);
        sbRadiusAnomaly->setMaximum(1000.000000000000000);
        sbRadiusAnomaly->setValue(20.000000000000000);

        formLayout->setWidget(6, QFormLayout::FieldRole, sbRadiusAnomaly);

        sbGeodesicAnomaly = new QDoubleSpinBox(formAdvancedOptions);
        sbGeodesicAnomaly->setObjectName(QString::fromUtf8("sbGeodesicAnomaly"));
        sbGeodesicAnomaly->setEnabled(false);
        sbGeodesicAnomaly->setValue(0.150000000000000);

        formLayout->setWidget(7, QFormLayout::FieldRole, sbGeodesicAnomaly);


        verticalLayout->addWidget(formAdvancedOptions);

        cbGaussianFilter = new QCheckBox(ALTIS);
        cbGaussianFilter->setObjectName(QString::fromUtf8("cbGaussianFilter"));

        verticalLayout->addWidget(cbGaussianFilter);

        cbRemoveNoise = new QCheckBox(ALTIS);
        cbRemoveNoise->setObjectName(QString::fromUtf8("cbRemoveNoise"));

        verticalLayout->addWidget(cbRemoveNoise);

        cbPleuralCorrection = new QCheckBox(ALTIS);
        cbPleuralCorrection->setObjectName(QString::fromUtf8("cbPleuralCorrection"));

        verticalLayout->addWidget(cbPleuralCorrection);

        pbRun = new QPushButton(ALTIS);
        pbRun->setObjectName(QString::fromUtf8("pbRun"));

        verticalLayout->addWidget(pbRun);

        pbCorrect = new QPushButton(ALTIS);
        pbCorrect->setObjectName(QString::fromUtf8("pbCorrect"));
        pbCorrect->setEnabled(false);

        verticalLayout->addWidget(pbCorrect);


        retranslateUi(ALTIS);

        QMetaObject::connectSlotsByName(ALTIS);
    } // setupUi

    void retranslateUi(QWidget *ALTIS)
    {
        ALTIS->setWindowTitle(QCoreApplication::translate("ALTIS", "Form", nullptr));
        pbAdvancedOptions->setText(QCoreApplication::translate("ALTIS", "Advanced Options", nullptr));
        lbAdjRel->setText(QCoreApplication::translate("ALTIS", "Adjacency relation radius", nullptr));
        lbClosingRadius->setText(QCoreApplication::translate("ALTIS", "Morphological closing radius", nullptr));
        lbOtsu->setText(QCoreApplication::translate("ALTIS", "Respiratory System detection factor", nullptr));
        lbTracheaThreshold->setText(QCoreApplication::translate("ALTIS", "Threshold for thrachea estimation", nullptr));
        lbDilation->setText(QCoreApplication::translate("ALTIS", "Squared radius for external marker estimation", nullptr));
        lbErosion->setText(QCoreApplication::translate("ALTIS", "Squared radius for internal marker estimation", nullptr));
        lbRadiusAnomaly->setText(QCoreApplication::translate("ALTIS", "Radius for anomaly detection", nullptr));
        lbGeodesicAnomaly->setText(QCoreApplication::translate("ALTIS", "Geodesic Distance factor", nullptr));
        cbGaussianFilter->setText(QCoreApplication::translate("ALTIS", "Smooth image", nullptr));
        cbRemoveNoise->setText(QCoreApplication::translate("ALTIS", "Remove noise", nullptr));
        cbPleuralCorrection->setText(QCoreApplication::translate("ALTIS", "Pleural anomaly correction", nullptr));
        pbRun->setText(QCoreApplication::translate("ALTIS", "Run", nullptr));
        pbCorrect->setText(QCoreApplication::translate("ALTIS", "Correct segmentation", nullptr));
    } // retranslateUi

};

namespace Ui {
    class ALTIS: public Ui_ALTIS {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_ALTIS_H
