/********************************************************************************
** Form generated from reading UI file 'threshold.ui'
**
** Created by: Qt User Interface Compiler version 6.3.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_THRESHOLD_H
#define UI_THRESHOLD_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSlider>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_Threshold
{
public:
    QFormLayout *formLayout;
    QPushButton *pbDisplayHistogram;
    QLabel *lbMin;
    QSpinBox *sbMin;
    QHBoxLayout *horizontalLayout;
    QSlider *horizontalSlider;
    QLabel *label;
    QLabel *lbMax;
    QSpinBox *sbMax;
    QHBoxLayout *horizontalLayout_2;
    QSlider *horizontalSlider_2;
    QLabel *label_2;
    QPushButton *pbRun;

    void setupUi(QWidget *Threshold)
    {
        if (Threshold->objectName().isEmpty())
            Threshold->setObjectName(QString::fromUtf8("Threshold"));
        Threshold->resize(274, 170);
        QSizePolicy sizePolicy(QSizePolicy::Minimum, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(Threshold->sizePolicy().hasHeightForWidth());
        Threshold->setSizePolicy(sizePolicy);
        formLayout = new QFormLayout(Threshold);
        formLayout->setObjectName(QString::fromUtf8("formLayout"));
        formLayout->setContentsMargins(0, 0, 0, 0);
        pbDisplayHistogram = new QPushButton(Threshold);
        pbDisplayHistogram->setObjectName(QString::fromUtf8("pbDisplayHistogram"));

        formLayout->setWidget(0, QFormLayout::SpanningRole, pbDisplayHistogram);

        lbMin = new QLabel(Threshold);
        lbMin->setObjectName(QString::fromUtf8("lbMin"));

        formLayout->setWidget(1, QFormLayout::LabelRole, lbMin);

        sbMin = new QSpinBox(Threshold);
        sbMin->setObjectName(QString::fromUtf8("sbMin"));
        sbMin->setMaximum(4095);

        formLayout->setWidget(1, QFormLayout::FieldRole, sbMin);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        horizontalSlider = new QSlider(Threshold);
        horizontalSlider->setObjectName(QString::fromUtf8("horizontalSlider"));
        horizontalSlider->setOrientation(Qt::Horizontal);

        horizontalLayout->addWidget(horizontalSlider);

        label = new QLabel(Threshold);
        label->setObjectName(QString::fromUtf8("label"));

        horizontalLayout->addWidget(label);


        formLayout->setLayout(2, QFormLayout::FieldRole, horizontalLayout);

        lbMax = new QLabel(Threshold);
        lbMax->setObjectName(QString::fromUtf8("lbMax"));

        formLayout->setWidget(3, QFormLayout::LabelRole, lbMax);

        sbMax = new QSpinBox(Threshold);
        sbMax->setObjectName(QString::fromUtf8("sbMax"));
        sbMax->setMaximum(4095);
        sbMax->setValue(4095);

        formLayout->setWidget(3, QFormLayout::FieldRole, sbMax);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        horizontalSlider_2 = new QSlider(Threshold);
        horizontalSlider_2->setObjectName(QString::fromUtf8("horizontalSlider_2"));
        horizontalSlider_2->setOrientation(Qt::Horizontal);

        horizontalLayout_2->addWidget(horizontalSlider_2);

        label_2 = new QLabel(Threshold);
        label_2->setObjectName(QString::fromUtf8("label_2"));

        horizontalLayout_2->addWidget(label_2);


        formLayout->setLayout(4, QFormLayout::FieldRole, horizontalLayout_2);

        pbRun = new QPushButton(Threshold);
        pbRun->setObjectName(QString::fromUtf8("pbRun"));

        formLayout->setWidget(5, QFormLayout::FieldRole, pbRun);


        retranslateUi(Threshold);

        QMetaObject::connectSlotsByName(Threshold);
    } // setupUi

    void retranslateUi(QWidget *Threshold)
    {
        Threshold->setWindowTitle(QCoreApplication::translate("Threshold", "Form", nullptr));
        pbDisplayHistogram->setText(QCoreApplication::translate("Threshold", "Display Image Histogram", nullptr));
        lbMin->setText(QCoreApplication::translate("Threshold", "Minimum Value", nullptr));
        label->setText(QCoreApplication::translate("Threshold", "TextLabel", nullptr));
        lbMax->setText(QCoreApplication::translate("Threshold", "Maximum Value", nullptr));
        label_2->setText(QCoreApplication::translate("Threshold", "TextLabel", nullptr));
        pbRun->setText(QCoreApplication::translate("Threshold", "Run", nullptr));
    } // retranslateUi

};

namespace Ui {
    class Threshold: public Ui_Threshold {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_THRESHOLD_H
