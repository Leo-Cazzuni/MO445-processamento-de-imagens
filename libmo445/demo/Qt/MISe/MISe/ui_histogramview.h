/********************************************************************************
** Form generated from reading UI file 'histogramview.ui'
**
** Created by: Qt User Interface Compiler version 6.3.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_HISTOGRAMVIEW_H
#define UI_HISTOGRAMVIEW_H

#include <QtCharts/QChartView>
#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_HistogramView
{
public:
    QVBoxLayout *verticalLayout;
    QChartView *chart;
    QHBoxLayout *horizontalLayout;
    QSpacerItem *horizontalSpacer;
    QLabel *lbMin;
    QSpinBox *sbMin;
    QHBoxLayout *horizontalLayout_3;
    QSpacerItem *horizontalSpacer_2;
    QLabel *lbMax;
    QSpinBox *sbMax;

    void setupUi(QWidget *HistogramView)
    {
        if (HistogramView->objectName().isEmpty())
            HistogramView->setObjectName(QString::fromUtf8("HistogramView"));
        HistogramView->resize(720, 480);
        verticalLayout = new QVBoxLayout(HistogramView);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        chart = new QChartView(HistogramView);
        chart->setObjectName(QString::fromUtf8("chart"));

        verticalLayout->addWidget(chart);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);

        lbMin = new QLabel(HistogramView);
        lbMin->setObjectName(QString::fromUtf8("lbMin"));

        horizontalLayout->addWidget(lbMin);

        sbMin = new QSpinBox(HistogramView);
        sbMin->setObjectName(QString::fromUtf8("sbMin"));

        horizontalLayout->addWidget(sbMin);


        verticalLayout->addLayout(horizontalLayout);

        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));
        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_3->addItem(horizontalSpacer_2);

        lbMax = new QLabel(HistogramView);
        lbMax->setObjectName(QString::fromUtf8("lbMax"));

        horizontalLayout_3->addWidget(lbMax);

        sbMax = new QSpinBox(HistogramView);
        sbMax->setObjectName(QString::fromUtf8("sbMax"));

        horizontalLayout_3->addWidget(sbMax);


        verticalLayout->addLayout(horizontalLayout_3);


        retranslateUi(HistogramView);

        QMetaObject::connectSlotsByName(HistogramView);
    } // setupUi

    void retranslateUi(QWidget *HistogramView)
    {
        HistogramView->setWindowTitle(QCoreApplication::translate("HistogramView", "Form", nullptr));
        lbMin->setText(QCoreApplication::translate("HistogramView", "Start value", nullptr));
        lbMax->setText(QCoreApplication::translate("HistogramView", "End value", nullptr));
    } // retranslateUi

};

namespace Ui {
    class HistogramView: public Ui_HistogramView {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_HISTOGRAMVIEW_H
