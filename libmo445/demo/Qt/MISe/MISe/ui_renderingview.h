/********************************************************************************
** Form generated from reading UI file 'renderingview.ui'
**
** Created by: Qt User Interface Compiler version 6.3.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_RENDERINGVIEW_H
#define UI_RENDERINGVIEW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include "views/rendering/qrenderinggraphicsview.h"

QT_BEGIN_NAMESPACE

class Ui_RenderingView
{
public:
    QVBoxLayout *verticalLayout;
    QRenderingGraphicsView *gvRendering;
    QWidget *wSubMenuSagittal;
    QHBoxLayout *horizontalLayout_8;
    QPushButton *pbPlanes;
    QPushButton *pbWireframe;
    QPushButton *pbSaveProjection;
    QLabel *lblRendering;
    QSpacerItem *horizontalSpacer_8;

    void setupUi(QWidget *RenderingView)
    {
        if (RenderingView->objectName().isEmpty())
            RenderingView->setObjectName(QString::fromUtf8("RenderingView"));
        RenderingView->resize(533, 354);
        verticalLayout = new QVBoxLayout(RenderingView);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        gvRendering = new QRenderingGraphicsView(RenderingView);
        gvRendering->setObjectName(QString::fromUtf8("gvRendering"));

        verticalLayout->addWidget(gvRendering);

        wSubMenuSagittal = new QWidget(RenderingView);
        wSubMenuSagittal->setObjectName(QString::fromUtf8("wSubMenuSagittal"));
        QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(wSubMenuSagittal->sizePolicy().hasHeightForWidth());
        wSubMenuSagittal->setSizePolicy(sizePolicy);
        wSubMenuSagittal->setMaximumSize(QSize(16777215, 50));
        horizontalLayout_8 = new QHBoxLayout(wSubMenuSagittal);
        horizontalLayout_8->setObjectName(QString::fromUtf8("horizontalLayout_8"));
        horizontalLayout_8->setContentsMargins(2, 2, 2, 2);
        pbPlanes = new QPushButton(wSubMenuSagittal);
        pbPlanes->setObjectName(QString::fromUtf8("pbPlanes"));

        horizontalLayout_8->addWidget(pbPlanes);

        pbWireframe = new QPushButton(wSubMenuSagittal);
        pbWireframe->setObjectName(QString::fromUtf8("pbWireframe"));

        horizontalLayout_8->addWidget(pbWireframe);

        pbSaveProjection = new QPushButton(wSubMenuSagittal);
        pbSaveProjection->setObjectName(QString::fromUtf8("pbSaveProjection"));

        horizontalLayout_8->addWidget(pbSaveProjection);

        lblRendering = new QLabel(wSubMenuSagittal);
        lblRendering->setObjectName(QString::fromUtf8("lblRendering"));

        horizontalLayout_8->addWidget(lblRendering);

        horizontalSpacer_8 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_8->addItem(horizontalSpacer_8);


        verticalLayout->addWidget(wSubMenuSagittal);


        retranslateUi(RenderingView);

        QMetaObject::connectSlotsByName(RenderingView);
    } // setupUi

    void retranslateUi(QWidget *RenderingView)
    {
        RenderingView->setWindowTitle(QCoreApplication::translate("RenderingView", "Form", nullptr));
        pbPlanes->setText(QString());
        pbWireframe->setText(QString());
        pbSaveProjection->setText(QString());
        lblRendering->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class RenderingView: public Ui_RenderingView {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_RENDERINGVIEW_H
