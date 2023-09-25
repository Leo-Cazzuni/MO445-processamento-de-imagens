/********************************************************************************
** Form generated from reading UI file 'curvesview.ui'
**
** Created by: Qt User Interface Compiler version 6.3.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CURVESVIEW_H
#define UI_CURVESVIEW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QSlider>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include "segmentation/autoece/chartview.h"

QT_BEGIN_NAMESPACE

class Ui_CurvesView
{
public:
    QVBoxLayout *verticalLayout;
    QSlider *hsSelectedCurves;
    ChartView *graphicsView;

    void setupUi(QWidget *CurvesView)
    {
        if (CurvesView->objectName().isEmpty())
            CurvesView->setObjectName(QString::fromUtf8("CurvesView"));
        CurvesView->resize(400, 300);
        verticalLayout = new QVBoxLayout(CurvesView);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        hsSelectedCurves = new QSlider(CurvesView);
        hsSelectedCurves->setObjectName(QString::fromUtf8("hsSelectedCurves"));
        hsSelectedCurves->setMaximum(500);
        hsSelectedCurves->setOrientation(Qt::Horizontal);
        hsSelectedCurves->setTickPosition(QSlider::TicksAbove);
        hsSelectedCurves->setTickInterval(1);

        verticalLayout->addWidget(hsSelectedCurves);

        graphicsView = new ChartView(CurvesView);
        graphicsView->setObjectName(QString::fromUtf8("graphicsView"));

        verticalLayout->addWidget(graphicsView);


        retranslateUi(CurvesView);

        QMetaObject::connectSlotsByName(CurvesView);
    } // setupUi

    void retranslateUi(QWidget *CurvesView)
    {
        CurvesView->setWindowTitle(QCoreApplication::translate("CurvesView", "ECE Curves", nullptr));
    } // retranslateUi

};

namespace Ui {
    class CurvesView: public Ui_CurvesView {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CURVESVIEW_H
