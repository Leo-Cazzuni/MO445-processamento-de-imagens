/********************************************************************************
** Form generated from reading UI file 'markersettings.ui'
**
** Created by: Qt User Interface Compiler version 6.3.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MARKERSETTINGS_H
#define UI_MARKERSETTINGS_H

#include <QtCore/QVariant>
#include <QtGui/QIcon>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QGraphicsView>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSlider>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include "views/qmywidget.h"

QT_BEGIN_NAMESPACE

class Ui_MarkerSettings
{
public:
    QWidget *centralwidget;
    QGridLayout *gridLayout;
    QHBoxLayout *horizontalLayout_2;
    QSpacerItem *horizontalSpacer;
    QPushButton *pbOk;
    QGroupBox *gpBrush;
    QVBoxLayout *verticalLayout_2;
    QHBoxLayout *horizontalLayout_4;
    QGraphicsView *gvBrush;
    QVBoxLayout *verticalLayout_3;
    QHBoxLayout *horizontalLayout_5;
    QLabel *label_2;
    QDoubleSpinBox *sbSize;
    QSlider *hsSize;
    QCheckBox *cbSpheric;
    QFormLayout *formLayout;
    QLabel *lblLabel;
    QLabel *lblObject;
    QLineEdit *leObject;
    QLabel *lblColor;
    QHBoxLayout *horizontalLayout;
    QMyWidget *wColor;
    QCheckBox *cbVisible;
    QSpacerItem *horizontalSpacer_2;
    QSpinBox *sbLabel;
    QSpacerItem *verticalSpacer;
    QGroupBox *gbAnnotation;
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout_3;
    QPushButton *pbFreeFormAnnotation;
    QPushButton *pbEraseMarker;
    QPushButton *pbBoxAnnotation;
    QSpacerItem *horizontalSpacer_4;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MarkerSettings)
    {
        if (MarkerSettings->objectName().isEmpty())
            MarkerSettings->setObjectName(QString::fromUtf8("MarkerSettings"));
        MarkerSettings->resize(357, 414);
        QSizePolicy sizePolicy(QSizePolicy::Maximum, QSizePolicy::Minimum);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(MarkerSettings->sizePolicy().hasHeightForWidth());
        MarkerSettings->setSizePolicy(sizePolicy);
        centralwidget = new QWidget(MarkerSettings);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        gridLayout = new QGridLayout(centralwidget);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setSpacing(0);
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        horizontalLayout_2->setSizeConstraint(QLayout::SetDefaultConstraint);
        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer);

        pbOk = new QPushButton(centralwidget);
        pbOk->setObjectName(QString::fromUtf8("pbOk"));
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/Images/icons/ok.svg"), QSize(), QIcon::Normal, QIcon::Off);
        pbOk->setIcon(icon);

        horizontalLayout_2->addWidget(pbOk);


        gridLayout->addLayout(horizontalLayout_2, 5, 0, 1, 1);

        gpBrush = new QGroupBox(centralwidget);
        gpBrush->setObjectName(QString::fromUtf8("gpBrush"));
        verticalLayout_2 = new QVBoxLayout(gpBrush);
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        horizontalLayout_4 = new QHBoxLayout();
        horizontalLayout_4->setSpacing(3);
        horizontalLayout_4->setObjectName(QString::fromUtf8("horizontalLayout_4"));
        gvBrush = new QGraphicsView(gpBrush);
        gvBrush->setObjectName(QString::fromUtf8("gvBrush"));
        gvBrush->setMinimumSize(QSize(64, 64));
        gvBrush->setMaximumSize(QSize(64, 64));
        gvBrush->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        gvBrush->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

        horizontalLayout_4->addWidget(gvBrush);

        verticalLayout_3 = new QVBoxLayout();
        verticalLayout_3->setObjectName(QString::fromUtf8("verticalLayout_3"));
        horizontalLayout_5 = new QHBoxLayout();
        horizontalLayout_5->setObjectName(QString::fromUtf8("horizontalLayout_5"));
        label_2 = new QLabel(gpBrush);
        label_2->setObjectName(QString::fromUtf8("label_2"));

        horizontalLayout_5->addWidget(label_2);

        sbSize = new QDoubleSpinBox(gpBrush);
        sbSize->setObjectName(QString::fromUtf8("sbSize"));
        sbSize->setMinimum(0.000000000000000);

        horizontalLayout_5->addWidget(sbSize);


        verticalLayout_3->addLayout(horizontalLayout_5);

        hsSize = new QSlider(gpBrush);
        hsSize->setObjectName(QString::fromUtf8("hsSize"));
        hsSize->setMinimum(0);
        hsSize->setOrientation(Qt::Horizontal);
        hsSize->setInvertedAppearance(false);
        hsSize->setInvertedControls(false);
        hsSize->setTickPosition(QSlider::TicksAbove);

        verticalLayout_3->addWidget(hsSize);


        horizontalLayout_4->addLayout(verticalLayout_3);


        verticalLayout_2->addLayout(horizontalLayout_4);

        cbSpheric = new QCheckBox(gpBrush);
        cbSpheric->setObjectName(QString::fromUtf8("cbSpheric"));

        verticalLayout_2->addWidget(cbSpheric);


        gridLayout->addWidget(gpBrush, 3, 0, 1, 1);

        formLayout = new QFormLayout();
        formLayout->setObjectName(QString::fromUtf8("formLayout"));
        lblLabel = new QLabel(centralwidget);
        lblLabel->setObjectName(QString::fromUtf8("lblLabel"));

        formLayout->setWidget(0, QFormLayout::LabelRole, lblLabel);

        lblObject = new QLabel(centralwidget);
        lblObject->setObjectName(QString::fromUtf8("lblObject"));

        formLayout->setWidget(1, QFormLayout::LabelRole, lblObject);

        leObject = new QLineEdit(centralwidget);
        leObject->setObjectName(QString::fromUtf8("leObject"));

        formLayout->setWidget(1, QFormLayout::FieldRole, leObject);

        lblColor = new QLabel(centralwidget);
        lblColor->setObjectName(QString::fromUtf8("lblColor"));

        formLayout->setWidget(2, QFormLayout::LabelRole, lblColor);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        wColor = new QMyWidget(centralwidget);
        wColor->setObjectName(QString::fromUtf8("wColor"));
        wColor->setMinimumSize(QSize(50, 0));

        horizontalLayout->addWidget(wColor);

        cbVisible = new QCheckBox(centralwidget);
        cbVisible->setObjectName(QString::fromUtf8("cbVisible"));

        horizontalLayout->addWidget(cbVisible);

        horizontalSpacer_2 = new QSpacerItem(150, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer_2);


        formLayout->setLayout(2, QFormLayout::FieldRole, horizontalLayout);

        sbLabel = new QSpinBox(centralwidget);
        sbLabel->setObjectName(QString::fromUtf8("sbLabel"));

        formLayout->setWidget(0, QFormLayout::FieldRole, sbLabel);


        gridLayout->addLayout(formLayout, 0, 0, 1, 1);

        verticalSpacer = new QSpacerItem(20, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);

        gridLayout->addItem(verticalSpacer, 4, 0, 1, 1);

        gbAnnotation = new QGroupBox(centralwidget);
        gbAnnotation->setObjectName(QString::fromUtf8("gbAnnotation"));
        QSizePolicy sizePolicy1(QSizePolicy::Preferred, QSizePolicy::Fixed);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(gbAnnotation->sizePolicy().hasHeightForWidth());
        gbAnnotation->setSizePolicy(sizePolicy1);
        gbAnnotation->setFlat(false);
        gbAnnotation->setCheckable(false);
        verticalLayout = new QVBoxLayout(gbAnnotation);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));
        pbFreeFormAnnotation = new QPushButton(gbAnnotation);
        pbFreeFormAnnotation->setObjectName(QString::fromUtf8("pbFreeFormAnnotation"));
        QIcon icon1;
        icon1.addFile(QString::fromUtf8(":/Images/icons/freeform.png"), QSize(), QIcon::Normal, QIcon::Off);
        pbFreeFormAnnotation->setIcon(icon1);
        pbFreeFormAnnotation->setIconSize(QSize(20, 20));
        pbFreeFormAnnotation->setCheckable(true);
        pbFreeFormAnnotation->setChecked(false);

        horizontalLayout_3->addWidget(pbFreeFormAnnotation);

        pbEraseMarker = new QPushButton(gbAnnotation);
        pbEraseMarker->setObjectName(QString::fromUtf8("pbEraseMarker"));
        QIcon icon2;
        icon2.addFile(QString::fromUtf8(":/Images/icons/eraser.png"), QSize(), QIcon::Normal, QIcon::Off);
        pbEraseMarker->setIcon(icon2);
        pbEraseMarker->setCheckable(true);

        horizontalLayout_3->addWidget(pbEraseMarker);

        pbBoxAnnotation = new QPushButton(gbAnnotation);
        pbBoxAnnotation->setObjectName(QString::fromUtf8("pbBoxAnnotation"));
        pbBoxAnnotation->setEnabled(false);
        QSizePolicy sizePolicy2(QSizePolicy::Minimum, QSizePolicy::Fixed);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(pbBoxAnnotation->sizePolicy().hasHeightForWidth());
        pbBoxAnnotation->setSizePolicy(sizePolicy2);
        QIcon icon3;
        icon3.addFile(QString::fromUtf8(":/Images/icons/box.png"), QSize(), QIcon::Normal, QIcon::Off);
        pbBoxAnnotation->setIcon(icon3);
        pbBoxAnnotation->setIconSize(QSize(20, 20));
        pbBoxAnnotation->setCheckable(true);

        horizontalLayout_3->addWidget(pbBoxAnnotation);

        horizontalSpacer_4 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_3->addItem(horizontalSpacer_4);


        verticalLayout->addLayout(horizontalLayout_3);


        gridLayout->addWidget(gbAnnotation, 2, 0, 1, 1);

        MarkerSettings->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MarkerSettings);
        menubar->setObjectName(QString::fromUtf8("menubar"));
        menubar->setGeometry(QRect(0, 0, 357, 22));
        MarkerSettings->setMenuBar(menubar);
        statusbar = new QStatusBar(MarkerSettings);
        statusbar->setObjectName(QString::fromUtf8("statusbar"));
        MarkerSettings->setStatusBar(statusbar);

        retranslateUi(MarkerSettings);

        QMetaObject::connectSlotsByName(MarkerSettings);
    } // setupUi

    void retranslateUi(QMainWindow *MarkerSettings)
    {
        MarkerSettings->setWindowTitle(QCoreApplication::translate("MarkerSettings", "Marker's Settings", nullptr));
        pbOk->setText(QCoreApplication::translate("MarkerSettings", "Ok", nullptr));
        gpBrush->setTitle(QCoreApplication::translate("MarkerSettings", "Brush", nullptr));
        label_2->setText(QCoreApplication::translate("MarkerSettings", "Size", nullptr));
        cbSpheric->setText(QCoreApplication::translate("MarkerSettings", "Spheric", nullptr));
        lblLabel->setText(QCoreApplication::translate("MarkerSettings", "Label", nullptr));
        lblObject->setText(QCoreApplication::translate("MarkerSettings", "Object", nullptr));
        lblColor->setText(QCoreApplication::translate("MarkerSettings", "Color", nullptr));
        cbVisible->setText(QCoreApplication::translate("MarkerSettings", "Visible", nullptr));
        gbAnnotation->setTitle(QCoreApplication::translate("MarkerSettings", "Annotation", nullptr));
        pbFreeFormAnnotation->setText(QString());
        pbEraseMarker->setText(QString());
        pbBoxAnnotation->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class MarkerSettings: public Ui_MarkerSettings {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MARKERSETTINGS_H
