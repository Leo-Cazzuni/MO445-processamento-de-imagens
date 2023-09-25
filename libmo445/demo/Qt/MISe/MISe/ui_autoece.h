/********************************************************************************
** Form generated from reading UI file 'autoece.ui'
**
** Created by: Qt User Interface Compiler version 6.3.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_AUTOECE_H
#define UI_AUTOECE_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSlider>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QTimeEdit>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_AutoECE
{
public:
    QVBoxLayout *verticalLayout;
    QPushButton *pbAdvancedOptions;
    QWidget *formAdvancedOptions;
    QFormLayout *formLayout;
    QLabel *lbDilation;
    QDoubleSpinBox *sbDilation;
    QLabel *lbFinalNSuperpixels;
    QSpinBox *sbFinalNSuperpixels;
    QLabel *label;
    QComboBox *cBoxSupervoxel;
    QLabel *label_2;
    QComboBox *cBoxSegmentation;
    QLabel *lbInitialNSuperpixels;
    QSpinBox *sbInitialNSuperpixels;
    QComboBox *cBoxRun;
    QLabel *label_3;
    QLabel *lbTimeofImages;
    QHBoxLayout *horizontalLayout;
    QSlider *sliderTimeImages;
    QLabel *lbImage;
    QLabel *lbCurrentImageIndex;
    QTimeEdit *teImages;
    QPushButton *pbBackgroundSeeds;
    QPushButton *pushButton;
    QPushButton *pbRun;
    QPushButton *pbViewECECurves;

    void setupUi(QWidget *AutoECE)
    {
        if (AutoECE->objectName().isEmpty())
            AutoECE->setObjectName(QString::fromUtf8("AutoECE"));
        AutoECE->resize(415, 376);
        verticalLayout = new QVBoxLayout(AutoECE);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        pbAdvancedOptions = new QPushButton(AutoECE);
        pbAdvancedOptions->setObjectName(QString::fromUtf8("pbAdvancedOptions"));
        pbAdvancedOptions->setCheckable(true);
        pbAdvancedOptions->setChecked(false);

        verticalLayout->addWidget(pbAdvancedOptions);

        formAdvancedOptions = new QWidget(AutoECE);
        formAdvancedOptions->setObjectName(QString::fromUtf8("formAdvancedOptions"));
        formLayout = new QFormLayout(formAdvancedOptions);
        formLayout->setObjectName(QString::fromUtf8("formLayout"));
        formLayout->setContentsMargins(0, 0, 0, 0);
        lbDilation = new QLabel(formAdvancedOptions);
        lbDilation->setObjectName(QString::fromUtf8("lbDilation"));

        formLayout->setWidget(0, QFormLayout::LabelRole, lbDilation);

        sbDilation = new QDoubleSpinBox(formAdvancedOptions);
        sbDilation->setObjectName(QString::fromUtf8("sbDilation"));
        sbDilation->setValue(1.800000000000000);

        formLayout->setWidget(0, QFormLayout::FieldRole, sbDilation);

        lbFinalNSuperpixels = new QLabel(formAdvancedOptions);
        lbFinalNSuperpixels->setObjectName(QString::fromUtf8("lbFinalNSuperpixels"));
        lbFinalNSuperpixels->setEnabled(true);

        formLayout->setWidget(3, QFormLayout::LabelRole, lbFinalNSuperpixels);

        sbFinalNSuperpixels = new QSpinBox(formAdvancedOptions);
        sbFinalNSuperpixels->setObjectName(QString::fromUtf8("sbFinalNSuperpixels"));
        sbFinalNSuperpixels->setMaximum(1000000);
        sbFinalNSuperpixels->setValue(500);

        formLayout->setWidget(3, QFormLayout::FieldRole, sbFinalNSuperpixels);

        label = new QLabel(formAdvancedOptions);
        label->setObjectName(QString::fromUtf8("label"));

        formLayout->setWidget(5, QFormLayout::LabelRole, label);

        cBoxSupervoxel = new QComboBox(formAdvancedOptions);
        cBoxSupervoxel->addItem(QString());
        cBoxSupervoxel->addItem(QString());
        cBoxSupervoxel->setObjectName(QString::fromUtf8("cBoxSupervoxel"));
        cBoxSupervoxel->setEditable(false);

        formLayout->setWidget(5, QFormLayout::FieldRole, cBoxSupervoxel);

        label_2 = new QLabel(formAdvancedOptions);
        label_2->setObjectName(QString::fromUtf8("label_2"));

        formLayout->setWidget(6, QFormLayout::LabelRole, label_2);

        cBoxSegmentation = new QComboBox(formAdvancedOptions);
        cBoxSegmentation->addItem(QString());
        cBoxSegmentation->addItem(QString());
        cBoxSegmentation->setObjectName(QString::fromUtf8("cBoxSegmentation"));

        formLayout->setWidget(6, QFormLayout::FieldRole, cBoxSegmentation);

        lbInitialNSuperpixels = new QLabel(formAdvancedOptions);
        lbInitialNSuperpixels->setObjectName(QString::fromUtf8("lbInitialNSuperpixels"));
        lbInitialNSuperpixels->setEnabled(true);

        formLayout->setWidget(2, QFormLayout::LabelRole, lbInitialNSuperpixels);

        sbInitialNSuperpixels = new QSpinBox(formAdvancedOptions);
        sbInitialNSuperpixels->setObjectName(QString::fromUtf8("sbInitialNSuperpixels"));
        sbInitialNSuperpixels->setMaximum(1000000);
        sbInitialNSuperpixels->setValue(10000);

        formLayout->setWidget(2, QFormLayout::FieldRole, sbInitialNSuperpixels);

        cBoxRun = new QComboBox(formAdvancedOptions);
        cBoxRun->addItem(QString());
        cBoxRun->addItem(QString());
        cBoxRun->addItem(QString());
        cBoxRun->addItem(QString());
        cBoxRun->setObjectName(QString::fromUtf8("cBoxRun"));

        formLayout->setWidget(7, QFormLayout::FieldRole, cBoxRun);

        label_3 = new QLabel(formAdvancedOptions);
        label_3->setObjectName(QString::fromUtf8("label_3"));

        formLayout->setWidget(7, QFormLayout::LabelRole, label_3);


        verticalLayout->addWidget(formAdvancedOptions);

        lbTimeofImages = new QLabel(AutoECE);
        lbTimeofImages->setObjectName(QString::fromUtf8("lbTimeofImages"));

        verticalLayout->addWidget(lbTimeofImages);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        sliderTimeImages = new QSlider(AutoECE);
        sliderTimeImages->setObjectName(QString::fromUtf8("sliderTimeImages"));
        sliderTimeImages->setMinimum(1);
        sliderTimeImages->setMaximum(7);
        sliderTimeImages->setTracking(true);
        sliderTimeImages->setOrientation(Qt::Horizontal);
        sliderTimeImages->setInvertedAppearance(false);
        sliderTimeImages->setInvertedControls(false);
        sliderTimeImages->setTickPosition(QSlider::TicksBelow);
        sliderTimeImages->setTickInterval(1);

        horizontalLayout->addWidget(sliderTimeImages);

        lbImage = new QLabel(AutoECE);
        lbImage->setObjectName(QString::fromUtf8("lbImage"));

        horizontalLayout->addWidget(lbImage);

        lbCurrentImageIndex = new QLabel(AutoECE);
        lbCurrentImageIndex->setObjectName(QString::fromUtf8("lbCurrentImageIndex"));

        horizontalLayout->addWidget(lbCurrentImageIndex);

        teImages = new QTimeEdit(AutoECE);
        teImages->setObjectName(QString::fromUtf8("teImages"));
        teImages->setCurrentSection(QDateTimeEdit::MinuteSection);
        teImages->setDisplayFormat(QString::fromUtf8("mm:ss"));

        horizontalLayout->addWidget(teImages);


        verticalLayout->addLayout(horizontalLayout);

        pbBackgroundSeeds = new QPushButton(AutoECE);
        pbBackgroundSeeds->setObjectName(QString::fromUtf8("pbBackgroundSeeds"));
        pbBackgroundSeeds->setCheckable(true);
        pbBackgroundSeeds->setChecked(false);

        verticalLayout->addWidget(pbBackgroundSeeds);

        pushButton = new QPushButton(AutoECE);
        pushButton->setObjectName(QString::fromUtf8("pushButton"));

        verticalLayout->addWidget(pushButton);

        pbRun = new QPushButton(AutoECE);
        pbRun->setObjectName(QString::fromUtf8("pbRun"));
        pbRun->setEnabled(true);
        pbRun->setCheckable(false);

        verticalLayout->addWidget(pbRun);

        pbViewECECurves = new QPushButton(AutoECE);
        pbViewECECurves->setObjectName(QString::fromUtf8("pbViewECECurves"));

        verticalLayout->addWidget(pbViewECECurves);


        retranslateUi(AutoECE);

        QMetaObject::connectSlotsByName(AutoECE);
    } // setupUi

    void retranslateUi(QWidget *AutoECE)
    {
        AutoECE->setWindowTitle(QCoreApplication::translate("AutoECE", "Form", nullptr));
        pbAdvancedOptions->setText(QCoreApplication::translate("AutoECE", "Advanced Options", nullptr));
        lbDilation->setText(QCoreApplication::translate("AutoECE", "Dilation radius", nullptr));
        lbFinalNSuperpixels->setText(QCoreApplication::translate("AutoECE", "Final number of superpixels (N_f << N_0)", nullptr));
        label->setText(QCoreApplication::translate("AutoECE", "Supervoxel Method", nullptr));
        cBoxSupervoxel->setItemText(0, QCoreApplication::translate("AutoECE", "DISF", nullptr));
        cBoxSupervoxel->setItemText(1, QCoreApplication::translate("AutoECE", "SICLE", nullptr));

        label_2->setText(QCoreApplication::translate("AutoECE", "Segmentation Method", nullptr));
        cBoxSegmentation->setItemText(0, QCoreApplication::translate("AutoECE", "DynamicTrees", nullptr));
        cBoxSegmentation->setItemText(1, QCoreApplication::translate("AutoECE", "Watershed", nullptr));

        lbInitialNSuperpixels->setText(QCoreApplication::translate("AutoECE", "Initial number of seeds (N_0 > 0)", nullptr));
        cBoxRun->setItemText(0, QCoreApplication::translate("AutoECE", "All", nullptr));
        cBoxRun->setItemText(1, QCoreApplication::translate("AutoECE", "Dilatation", nullptr));
        cBoxRun->setItemText(2, QCoreApplication::translate("AutoECE", "Superspels", nullptr));
        cBoxRun->setItemText(3, QCoreApplication::translate("AutoECE", "Select Labels", nullptr));

        label_3->setText(QCoreApplication::translate("AutoECE", "Run", nullptr));
        lbTimeofImages->setText(QCoreApplication::translate("AutoECE", "Time of Images (s)", nullptr));
        lbImage->setText(QCoreApplication::translate("AutoECE", "Image", nullptr));
        lbCurrentImageIndex->setText(QCoreApplication::translate("AutoECE", "1", nullptr));
        pbBackgroundSeeds->setText(QCoreApplication::translate("AutoECE", "Select Background Seeds", nullptr));
        pushButton->setText(QCoreApplication::translate("AutoECE", "Render Beautifully", nullptr));
        pbRun->setText(QCoreApplication::translate("AutoECE", "Run", nullptr));
        pbViewECECurves->setText(QCoreApplication::translate("AutoECE", "View ECE Curves", nullptr));
    } // retranslateUi

};

namespace Ui {
    class AutoECE: public Ui_AutoECE {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_AUTOECE_H
